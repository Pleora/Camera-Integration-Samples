// *****************************************************************************
//
// Copyright (c) 2018, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "Defines.h"
#include "MyEventSink.h"
#include "Utilities.h"
#include "Enums.h"

#include <PvPixelType.h>

#include <limits>
#include <sstream>


///
/// \brief Constructor
///

MyEventSink::MyEventSink( IPvRegisterEventSink *aRegisterEventSink )
    : mRegisterEventSink( aRegisterEventSink )
{
}


///
/// \brief Destructor
///

MyEventSink::~MyEventSink()
{

}


///
/// \brief Application connection notification
///

void MyEventSink::OnApplicationConnect( IPvSoftDeviceGEV *aDevice, const PvString &aIPAddress, uint16_t aPort, PvAccessType aAccessType )
{
    PVUNREFPARAM( aDevice );
    PVUNREFPARAM( aAccessType );
    std::cout << "Application connected from " << aIPAddress.GetAscii() << ":" << aPort << std::endl;
}


///
/// \brief Application disconnection notification
///

void MyEventSink::OnApplicationDisconnect( IPvSoftDeviceGEV *aDevice )
{
    PVUNREFPARAM( aDevice );
    std::cout << "Application disconnected" << std::endl;
}


///
/// \brief Control channel start notification
///

void MyEventSink::OnControlChannelStart( IPvSoftDeviceGEV *aDevice, const PvString &aMACAddress, const PvString &aIPAddress, const PvString &aMask, const PvString &aGateway, uint16_t aPort )
{
    PVUNREFPARAM( aDevice );
    std::cout << "Control channel started on [" << aMACAddress.GetAscii() << "] ";
    std::cout << aIPAddress.GetAscii() << ":" << aPort << " ";
    std::cout << "Mask:" << aMask.GetAscii() << ", ";
    std::cout << "Gateway:" << aGateway.GetAscii() << std::endl;

//    DumpRegisters( aDevice->GetRegisterMap() );
}


///
/// \brief Control channel stop notification
///

void MyEventSink::OnControlChannelStop( IPvSoftDeviceGEV *aDevice )
{
    PVUNREFPARAM( aDevice );
    std::cout << "Control channel stopped" << std::endl;
}


///
/// \brief Device reset notification
///

void MyEventSink::OnDeviceResetFull( IPvSoftDeviceGEV *aDevice )
{
    PVUNREFPARAM( aDevice );
    std::cout << "Device reset" << std::endl;
}


///
/// \brief Network reset notification
///

void MyEventSink::OnDeviceResetNetwork( IPvSoftDeviceGEV *aDevice )
{
    PVUNREFPARAM( aDevice );
    std::cout << "Network reset" << std::endl;
}


///
/// \brief Callback used to initiate custom registers creation
///

void MyEventSink::OnCreateCustomRegisters( IPvSoftDeviceGEV *aDevice, IPvRegisterFactory *aFactory )
{
    PVUNREFPARAM( aDevice );

    aFactory->AddRegister( SAMPLEINTEGERNAME, SAMPLEINTEGER_ADDR, 4, PvGenAccessModeReadWrite, mRegisterEventSink );
    aFactory->AddRegister( SAMPLEFLOATNAME, SAMPLEFLOAT_ADDR, 4, PvGenAccessModeReadWrite, mRegisterEventSink );
    aFactory->AddRegister( SAMPLESTRINGNAME, SAMPLESTRING_ADDR, 16, PvGenAccessModeReadWrite, mRegisterEventSink );
    aFactory->AddRegister( SAMPLEBOOLEANNAME, SAMPLEBOOLEAN_ADDR, 4, PvGenAccessModeReadWrite, mRegisterEventSink );
    aFactory->AddRegister( SAMPLECOMMANDNAME, SAMPLECOMMAND_ADDR, 4, PvGenAccessModeReadWrite, mRegisterEventSink );
    aFactory->AddRegister( SAMPLEENUMNAME, SAMPLEENUM_ADDR, 4, PvGenAccessModeReadWrite, mRegisterEventSink );
}


///
/// \brief Callback used to initiate GenApi features creation
///

void MyEventSink::OnCreateCustomGenApiFeatures( IPvSoftDeviceGEV *aDevice, IPvGenApiFactory *aFactory )
{
    IPvRegisterMap *lMap = aDevice->GetRegisterMap();

    CreateSampleParameters( lMap, aFactory );
    CreateChunkParameters( aFactory );
    CreateEventParameters( aFactory );
    CustomizePixelFormat( aFactory );
}


///
/// \brief Shows how to create custom GenApi parameters
///

void MyEventSink::CreateSampleParameters( IPvRegisterMap *aMap, IPvGenApiFactory *aFactory )
{
    // Create GenApi feature used to map the sample command register
    aFactory->SetName( SAMPLEENUMNAME );
    aFactory->SetDescription( DESCRIPTION );
    aFactory->SetToolTip( TOOLTIP );
    aFactory->SetCategory( SAMPLECATEGORY );
    aFactory->AddEnumEntry( "EnumEntry1", 0 );
    aFactory->AddEnumEntry( "EnumEntry2", 1 );
    aFactory->AddEnumEntry( "EnumEntry3", 2 );
    aFactory->AddSelected( SAMPLEINTEGERNAME );
    aFactory->AddSelected( SAMPLEFLOATNAME );
    aFactory->CreateEnum( aMap->GetRegisterByAddress( SAMPLEENUM_ADDR ) );

    // Create GenApi feature used to map the sample integer register
    aFactory->SetName( SAMPLEINTEGERNAME );
    aFactory->SetDescription( DESCRIPTION );
    aFactory->SetToolTip( TOOLTIP );
    aFactory->SetCategory( SAMPLECATEGORY );
    aFactory->SetRepresentation( PvGenRepresentationLogarithmic );
    aFactory->CreateInteger( aMap->GetRegisterByAddress( SAMPLEINTEGER_ADDR ), 0, 1024, 1 );

    // Create GenApi feature used to map the sample float register
    aFactory->SetName( SAMPLEFLOATNAME );
    aFactory->SetDescription( DESCRIPTION );
    aFactory->SetToolTip( TOOLTIP );
    aFactory->SetCategory( SAMPLECATEGORY );
    aFactory->SetRepresentation( PvGenRepresentationPureNumber );
    aFactory->CreateFloat( aMap->GetRegisterByAddress( SAMPLEFLOAT_ADDR ), 0.0, 100.0 );

    // Create GenApi feature used to map the sample string register
    aFactory->SetName( SAMPLESTRINGNAME );
    aFactory->SetDescription( DESCRIPTION );
    aFactory->SetToolTip( TOOLTIP );
    aFactory->SetCategory( SAMPLECATEGORY );
    aFactory->AddInvalidator( SAMPLECOMMANDNAME );
    aFactory->CreateString( aMap->GetRegisterByAddress( SAMPLESTRING_ADDR ) );

    // Create GenApi feature used to map the sample Boolean register
    aFactory->SetName( SAMPLEBOOLEANNAME );
    aFactory->SetDescription( DESCRIPTION );
    aFactory->SetToolTip( TOOLTIP );
    aFactory->SetCategory( SAMPLECATEGORY );
    aFactory->CreateBoolean( aMap->GetRegisterByAddress( SAMPLEBOOLEAN_ADDR ) );

    // Create GenApi feature used to map the sample command register
    aFactory->SetName( SAMPLECOMMANDNAME );
    aFactory->SetDescription( DESCRIPTION );
    aFactory->SetToolTip( TOOLTIP );
    aFactory->SetCategory( SAMPLECATEGORY );
    aFactory->CreateCommand( aMap->GetRegisterByAddress( SAMPLECOMMAND_ADDR ) );
}


///
/// \brief Shows how to create custom GenApi parameters for chunk data mapping
///

void MyEventSink::CreateChunkParameters( IPvGenApiFactory *aFactory )
{
    // Create GenApi feature used to map the chunk data count field
    aFactory->SetName( "LaserPointesCount" );
    aFactory->SetDescription( "The number of the valid detected laser point" );
 //   aFactory->SetToolTip( TOOLTIP );
    aFactory->SetCategory( CHUNKCATEGORY );
    aFactory->MapChunk( CHUNKID, 0, 4, PvGenEndiannessLittle );
    aFactory->CreateInteger( NULL, 0, std::numeric_limits<uint32_t>::max() );
#if 0
    // Create GenApi feature used to map the chunk data time field
    aFactory->SetName( CHUNKTIMENAME );
    aFactory->SetDescription( DESCRIPTION );
    aFactory->SetToolTip( TOOLTIP );
    aFactory->SetCategory( CHUNKCATEGORY );
    aFactory->MapChunk( CHUNKID, 4, 32, PvGenEndiannessLittle );
    aFactory->CreateString( NULL );
#endif

}


///
/// \brief Shows how to create custom GenApi parameters for event data mapping
///

void MyEventSink::CreateEventParameters( IPvGenApiFactory *aFactory )
{
    // Create GenApi feature used to map the event data count field
    aFactory->SetName( EVENTCOUNTNAME );
    aFactory->SetDescription( DESCRIPTION );
    aFactory->SetToolTip( TOOLTIP );
    aFactory->SetCategory( EVENTCATEGORY );
    aFactory->MapEvent( EVENTDATAID, 0, 4 );
    aFactory->CreateInteger( NULL, 0, std::numeric_limits<uint32_t>::max() );

    // Create GenApi feature used to map the event data time field
    aFactory->SetName( EVENTTIMENAME );
    aFactory->SetDescription( DESCRIPTION );
    aFactory->SetToolTip( TOOLTIP );
    aFactory->SetCategory( EVENTCATEGORY );
    aFactory->MapEvent( EVENTDATAID, 4, 32 );
    aFactory->CreateString( NULL );
}


///
/// \brief Customizes the PixelFormat feature for our camera model
///
/// The PixelFormat feature is handled by PvSoftDeviceGEV but needs customization to work
/// with our current camera model. The GenApi factory allows us to do so.
///

void MyEventSink::CustomizePixelFormat( IPvGenApiFactory *aFactory )
{
    // Create MonoAvailableExpr SwissKnife for:
    //     Mono8, Mono12, Mono16 enabled if
    //     - SourceSelector is Source0, Source1, Source2: handled by streaming source GetSupportedPixelType
    //     - RegionSelector is Region0
    //     - ComponentSelector is Intensity
    aFactory->SetName( "MonoAvailableExpr" );
    aFactory->AddVariable( "RegionSelector" );
    aFactory->AddVariable( "ComponentSelector" );
    std::stringstream lSK1;
    lSK1 << "(RegionSelector <= " << Region1 << ") && (ComponentSelector = " << ComponentIntensity << ")";
    aFactory->CreateIntSwissKnife( lSK1.str().c_str() );

    // Create CoordAvailableExpr SwissKnife for:
    //     Coord3d_C32f, Coord3dABC32f enabled if
    //     - SourceSelector is Source2: handled by streaming source GetSupportedPixelType
    //     - RegionSelector is Scan3dExtraction0
    //     - ComponentSelector is Range
    aFactory->SetName( "CoordAvailableExpr" );
    aFactory->AddVariable( "SourceSelector" );
    aFactory->AddVariable( "RegionSelector" );
    aFactory->AddVariable( "ComponentSelector" );
    std::stringstream lSK2;
    lSK2 << "(SourceSelector = 2) && (RegionSelector >= " << RegionScan3dExtraction0 << ") && (ComponentSelector = " << ComponentRange << ")";
    aFactory->CreateIntSwissKnife( lSK2.str().c_str() );

    // Create CoordAvailableExpr SwissKnife for:
    //     Confidence1, Confidence1p, Confidence8 enabled if
    //     - SourceSelector is Source2: handled by streaming source GetSupportedPixelType
    //     - RegionSelector is Scan3dExtraction0
    //     - ComponentSelector is Confidence
    aFactory->SetName( "ConfidenceAvailableExpr" );
    aFactory->AddVariable( "SourceSelector" );
    aFactory->AddVariable( "RegionSelector" );
    aFactory->AddVariable( "ComponentSelector" );
    std::stringstream lSK3;
    lSK3 << "(SourceSelector = 2) && (RegionSelector >= " << RegionScan3dExtraction0 << ") && (ComponentSelector = " << ComponentConfidence << ")";
    aFactory->CreateIntSwissKnife( lSK3.str().c_str() );

    // Assign pIsAvailable expressions to pixel types
    aFactory->SetPIsAvailableForEnumEntry( "PixelFormat", "Mono8", "MonoAvailableExpr" );
    aFactory->SetPIsAvailableForEnumEntry( "PixelFormat", "Mono12", "MonoAvailableExpr" );
    aFactory->SetPIsAvailableForEnumEntry( "PixelFormat", "Mono16", "MonoAvailableExpr" );
    aFactory->SetPIsAvailableForEnumEntry( "PixelFormat", "Coord3D_C32f", "CoordAvailableExpr" );
    aFactory->SetPIsAvailableForEnumEntry( "PixelFormat", "Coord3D_ABC32f", "CoordAvailableExpr" );
    aFactory->SetPIsAvailableForEnumEntry( "PixelFormat", "Confidence1", "ConfidenceAvailableExpr" );
    aFactory->SetPIsAvailableForEnumEntry( "PixelFormat", "Confidence1p", "ConfidenceAvailableExpr" );
    aFactory->SetPIsAvailableForEnumEntry( "PixelFormat", "Confidence8", "ConfidenceAvailableExpr" );

    // Image format features are available if we have at least one valid PixelFormat enum entry available
    aFactory->SetName( "ImageFormatAvailableExpr" );
    aFactory->AddVariable( "MonoAvailableExpr" );
    aFactory->AddVariable( "CoordAvailableExpr" );
    aFactory->AddVariable( "ConfidenceAvailableExpr" );
    std::stringstream lSK4;
    lSK4 << "MonoAvailableExpr || CoordAvailableExpr || ConfidenceAvailableExpr";
    aFactory->CreateIntSwissKnife( lSK4.str().c_str() );

    // Set image format features availability
    aFactory->SetPIsAvailableFor( "PixelFormat", "ImageFormatAvailableExpr" );
    aFactory->SetPIsAvailableFor( "WidthMax", "ImageFormatAvailableExpr" );
    aFactory->SetPIsAvailableFor( "HeightMax", "ImageFormatAvailableExpr" );
    aFactory->SetPIsAvailableFor( "Width", "ImageFormatAvailableExpr" );
    aFactory->SetPIsAvailableFor( "Height", "ImageFormatAvailableExpr" );
    aFactory->SetPIsAvailableFor( "OffsetX", "ImageFormatAvailableExpr" );
    aFactory->SetPIsAvailableFor( "OffsetY", "ImageFormatAvailableExpr" );
    aFactory->SetPIsAvailableFor( "PixelFormat", "ImageFormatAvailableExpr" );
    aFactory->SetPIsAvailableFor( "PixelSize", "ImageFormatAvailableExpr" );

    // Add invalidators to features owned by PvSoftDeviceGEV.
    aFactory->AddInvalidatorTo( "PixelFormat", "ComponentSelector" );
    aFactory->AddInvalidatorTo( "PixelFormat", "RegionSelector" );
    aFactory->AddInvalidatorTo( "WidthMax", "RegionSelector" );
    aFactory->AddInvalidatorTo( "HeightMax", "RegionSelector" );
    aFactory->AddInvalidatorTo( "Width", "RegionSelector" );
    aFactory->AddInvalidatorTo( "Height", "RegionSelector" );
    aFactory->AddInvalidatorTo( "OffsetX", "RegionSelector" );
    aFactory->AddInvalidatorTo( "OffsetY", "RegionSelector" );
    aFactory->AddInvalidatorTo( "PixelFormat", "RegionSelector" );
    aFactory->AddInvalidatorTo( "PixelSize", "RegionSelector" );
}

