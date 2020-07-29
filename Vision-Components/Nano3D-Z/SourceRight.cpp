// *****************************************************************************
//
// Copyright (c) 2018, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "Defines.h"
#include "SourceRight.h"
#include "Utilities.h"

#include <PvSampleUtils.h>


///
/// \brief Constructor
///

SourceRight::SourceRight( uint32_t aSourceIndex )
    : mSourceIndex( aSourceIndex )
    , mWidth( WIDTH_DEFAULT )
    , mHeight( HEIGHT_DEFAULT )
    , mPixelType( PvPixelMono8 )
    , mBufferCount( 0 )
    , mAcquisitionBuffer( NULL )
    , mSeed( 0 )
    , mFrameCount( 0 )
{
}


///
/// \brief Destructor
///

SourceRight::~SourceRight()
{
}


///
/// \brief Width info query
///

void SourceRight::GetWidthInfo( uint32_t &aMin, uint32_t &aMax, uint32_t &aInc ) const
{
    aMin = WIDTH_MIN;
    aMax = WIDTH_MAX;
    aInc = WIDTH_INC;
}


///
/// \brief Height info query
///

void SourceRight::GetHeightInfo( uint32_t &aMin, uint32_t &aMax, uint32_t &aInc ) const
{
    aMin = HEIGHT_MIN;
    aMax = HEIGHT_MAX;
    aInc = HEIGHT_INC;
}


///
/// \brief Supported pixel types query
///

PvResult SourceRight::GetSupportedPixelType( int aIndex, PvPixelType &aPixelType ) const
{
    switch ( aIndex )
    {
    case 0:
        aPixelType = PvPixelMono8;
        return PvResult::Code::OK;

    case 1:
        aPixelType = PvPixelMono12;
        return PvResult::Code::OK;

    case 2:
        aPixelType = PvPixelMono16;
        return PvResult::Code::OK;

    default:
        break;
    }

    return PvResult::Code::INVALID_PARAMETER;
}


///
/// \brief Width setter
///

PvResult SourceRight::SetWidth( uint32_t aWidth )
{
    mWidth = aWidth;
    return PvResult::Code::OK;
}


///
/// \brief Height setter
///

PvResult SourceRight::SetHeight( uint32_t aHeight )
{
    mHeight = aHeight;
    return PvResult::Code::OK;
}


///
/// \brief Pixel type setter
///

PvResult SourceRight::SetPixelType( PvPixelType aPixelType )
{
    mPixelType = aPixelType;
    return PvResult::Code::OK;
}


///
/// \brief Request for a new buffer.
///
/// Return a new buffer until buffer count is reached, then return NULL to 
/// signal that all buffers have been allocated.
///

PvBuffer *SourceRight::AllocBuffer()
{
    if ( mBufferCount < BUFFERCOUNT )
    {
        mBufferCount++;
        return new PvBuffer;
    }

    return NULL;
}


///
/// \brief Request to free one of the buffers allocated with AllocBuffer
///

void SourceRight::FreeBuffer( PvBuffer *aBuffer )
{
    PVDELETE( aBuffer );
    mBufferCount--;
}


///
/// \brief Request to queue a buffer for acquisition.
///
/// Return OK if the buffer is queued or any error if no more room in acquisition queue
///

PvResult SourceRight::QueueBuffer( PvBuffer *aBuffer )
{
    // We use mAcqusitionBuffer as a 1-deep acquisition pipeline
    if ( mAcquisitionBuffer == NULL )
    {
        // No buffer queued, accept it
        mAcquisitionBuffer = aBuffer;

        // Acquire buffer - could be done in another thread
        ResizeBufferIfNeeded( mAcquisitionBuffer );
        switch ( GetPixelType() )
        {
        case PvPixelMono8:
            FillTestPatternMono8( mAcquisitionBuffer );
            break;

        default:
            break;
        }
        mFrameCount++;

        return PvResult::Code::OK;
    }

    // We already have a buffer queued for acquisition
    return PvResult::Code::BUSY;
}


///
/// \brief Request to give back a buffer ready for transmission.
///
/// Either block until a buffer is available or return any error
///

PvResult SourceRight::RetrieveBuffer( PvBuffer **aBuffer )
{
    if ( mAcquisitionBuffer == NULL )
    {
        // No buffer queued for acquisition
        return PvResult::Code::NO_AVAILABLE_DATA;
    }

    while ( !mStabilizer.IsTimeToDisplay( DEFAULT_FPS ) )
    {
        PvSleepMs( 1 );
    }

    // Remove buffer from 1-deep pipeline
    *aBuffer = mAcquisitionBuffer;
    mAcquisitionBuffer = NULL;

    return PvResult::Code::OK;
}


///
/// \brief Signal to abort all buffers waiting for acquisition.
///
/// Nothing to do here // with our simple 1-deep with in-line test pattern 
/// generation acquisition sample.
///

void SourceRight::AbortQueuedBuffers()
{
}


///
/// \brief Resizes a buffer for acquisition with the current configuration
///

void SourceRight::ResizeBufferIfNeeded( PvBuffer *aBuffer )
{
    PvImage *lImage = aBuffer->GetImage();
    if ( ( lImage->GetWidth() != mWidth ) ||
        ( lImage->GetHeight() != mHeight ) ||
        ( lImage->GetPixelType() != mPixelType ) )
    {
        lImage->Alloc( mWidth, mHeight, mPixelType );
    }
}


///
/// \brief Generate a greyscale test pattern in a PvBuffer
///

void SourceRight::FillTestPatternMono8( PvBuffer *aBuffer )
{
    PvImage *lImage = aBuffer->GetImage();
    uint32_t lHeight = lImage->GetHeight();
    uint32_t lWidth = lImage->GetWidth();

    for ( uint32_t y = 0; y < lHeight; y++ )
    {
        uint8_t lValue = static_cast<uint8_t>( mSeed + y );
        uint8_t *lPtr = aBuffer->GetDataPointer();
        lPtr += ( y * lImage->GetWidth() ) + lImage->GetOffsetX();

        for ( uint32_t x = 0; x < lWidth; x++ )
        {
            *lPtr++ = lValue--;
        }
    }

    mSeed++;
}

