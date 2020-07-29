// *****************************************************************************
//
// Copyright (c) 2018, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include <PvStreamingChannelSourceDefault.h>
#include <PvFPSStabilizer.h>


///
/// \brief This class shows how to implement a streaming channel source
///

class SourceLeft
    : public PvStreamingChannelSourceDefault
{
public:

    SourceLeft( uint32_t aSourceIndex );
    virtual ~SourceLeft();

    // IPvStreamingChannelSource implementation
    uint32_t GetWidth() const { return mWidth; }
    uint32_t GetHeight() const { return mHeight; }
    void GetWidthInfo( uint32_t &aMin, uint32_t &aMax, uint32_t &aInc ) const;
    void GetHeightInfo( uint32_t &aMin, uint32_t &aMax, uint32_t &aInc ) const;
    PvResult GetSupportedPixelType( int aIndex, PvPixelType &aPixelType ) const;
    PvResult SetWidth( uint32_t aWidth );
    PvResult SetHeight( uint32_t aHeight );
    PvResult SetPixelType( PvPixelType aPixelType );
    PvBuffer *AllocBuffer();
    void FreeBuffer( PvBuffer *aBuffer );
    PvResult QueueBuffer( PvBuffer *aBuffer );
    PvResult RetrieveBuffer( PvBuffer **aBuffer );
    void AbortQueuedBuffers();

protected:

    void ResizeBufferIfNeeded( PvBuffer *aBuffer );
    void FillTestPatternMono8( PvBuffer *aBuffer );

private:

    uint32_t mSourceIndex;

    uint32_t mWidth;
    uint32_t mHeight;
    PvPixelType mPixelType;

    PvFPSStabilizer mStabilizer;

    uint32_t mBufferCount;
    PvBuffer *mAcquisitionBuffer;

    uint32_t mSeed;
    uint32_t mFrameCount;

};
