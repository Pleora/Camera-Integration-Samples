// *****************************************************************************
//
// Copyright (c) 2019, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "Defines.h"
#include "Region.h"

#include <PvImage.h>


///
/// \brief Constructor
///

Region::Region()
    : mMode( true )
    , mDataType( PvMultiPart2DImage )
    , mWidth( WIDTH_DEFAULT )
    , mHeight( HEIGHT_DEFAULT )
    , mOffsetX( 0 )
    , mOffsetY( 0 )
{

}


///
/// \brief Destructor
///

Region::~Region()
{

}


///
/// \brief Adds a component to the region
///

void Region::AddComponent( ComponentEnum aComponent, PvPixelType aDefaultPixelType )
{
    mComponents[ aComponent ].PixelType = aDefaultPixelType;
    mComponentValues.push_back( aComponent );
}


///
/// \brief Sets the region IDs
///

void Region::SetIDs( uint32_t aRegionID, uint32_t aSourceID )
{
    mRegionID = aRegionID;
    mSourceID = aSourceID;
}


///
/// \brief Gets the enabled state of a specific component
///

bool Region::GetComponentEnable( ComponentEnum aComponent )
{
    auto lIt = mComponents.find( aComponent );
    if ( lIt == mComponents.end() )
    {
        // We don't even support this component, let's say it is not enabled
        return false;
    }

    return lIt->second.Enabled;
}


void Region::SetComponentEnable( ComponentEnum aComponent, bool aValue )
{
    auto lIt = mComponents.find( aComponent );
    if ( lIt == mComponents.end() )
    {
        // Component not supported, silently discard
        return;
    }

    lIt->second.Enabled = aValue;
}


///
/// \brief Pixel type for a specific component
///

PvPixelType Region::GetPixelType( ComponentEnum aComponent ) const
{
    auto lIt = mComponents.find( aComponent );
    if ( lIt == mComponents.end() )
    {
        return PvPixelUndefined;
    }

    return lIt->second.PixelType;
}


///
/// \brief Sets the pixel type for a specific component
///

void Region::SetPixelType( ComponentEnum aComponent, PvPixelType aValue )
{
    auto lIt = mComponents.find( aComponent );
    if ( lIt == mComponents.end() )
    {
        return;
    }

    lIt->second.PixelType = aValue;
}



///
/// \brief Returns true if the multi-part section matches the definition of this region.
///

bool Region::Matches( uint32_t aComponentIndex, IPvMultiPartSection *aMPS )
{
    IPvImage *lImage = aMPS->GetImage();
    bool lMatch = ( aMPS->GetDataType() == mDataType ) &&
        ( lImage != NULL ) &&
        ( lImage->GetWidth() == mWidth ) &&
        ( lImage->GetHeight() == mHeight ) &&
        ( lImage->GetPixelType() == mComponents[ mComponentValues[ aComponentIndex ] ].PixelType ) &&
        ( lImage->GetOffsetX() == mOffsetX ) &&
        ( lImage->GetOffsetY() == mOffsetY );

    return lMatch;
}


///
/// \brief Adds a part configured for this region to the container
///

void Region::AddPartsTo( IPvMultiPartContainer *aMPC )
{
    for ( auto lComponent = mComponentValues.begin(); lComponent != mComponentValues.end(); ++lComponent )
    {
        if ( !GetComponentEnable( *lComponent ) )
        {
            continue;
        }

        // Create part in part index
        aMPC->AddImagePart( mDataType,
            mWidth, mHeight, mComponents[ *lComponent ].PixelType,
            mOffsetX, mOffsetY );

        // Set part IDs
        aMPC->SetPartIDs( aMPC->GetPartCount() - 1, mSourceID, *lComponent, mRegionID );
    }
}


///
/// \brief Fills a multi-part section with our region data
///

void Region::Fill( uint32_t aComponentIndex, IPvMultiPartSection *aMPS )
{
    if ( ( aMPS == NULL ) ||
         !Matches( aComponentIndex, aMPS ) )
    {
        return;
    }

    IPvImage *lImage = aMPS->GetImage();
    Fill( lImage );
}


///
/// \brief Fills an image with our region data
///

void Region::Fill( IPvImage *aImage )
{
    if ( aImage == NULL )
    {
        return;
    }
}


///
/// \brief Returns payload size as configured. Can be zero if not active.
///

uint32_t Region::GetPayloadSize() const
{
    if ( !mMode )
    {
        return 0;
    }

    uint32_t lPayloadSize = 0;
    for ( auto lComponent = mComponents.begin(); lComponent != mComponents.end(); ++lComponent )
    {
        if ( !lComponent->second.Enabled )
        {
            continue;
        }

        uint32_t lPixelSizeInBits = PvImage::GetPixelSize( lComponent->second.PixelType );
        lPayloadSize += ( ( mWidth * lPixelSizeInBits ) / 8 ) * mHeight;
    }

    return lPayloadSize;
}

