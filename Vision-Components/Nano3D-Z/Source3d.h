// *****************************************************************************
//
// Copyright (c) 2018, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include <PvSoftDeviceGEVInterfaces.h>
#include <PvFPSStabilizer.h>

#include "Region.h"
#include "Scan3dExtraction.h"
#include "Scan3dPattern.h"
#include "Camera.h"

///
/// \brief This class shows how to implement a streaming channel source
///

class Source3d
    : public IPvStreamingChannelSource
    , public IPvRegisterEventSink
{
public:

    Source3d( uint32_t aSourceIndex );
    virtual ~Source3d();

    // IPvStreamingChannelSource implementation
    uint32_t GetWidth() const { return mRegions[ mRegionSelector ].GetWidth(); }
    uint32_t GetHeight() const { return mRegions[ mRegionSelector ].GetHeight(); }
    uint32_t GetOffsetX() const { return mRegions[ mRegionSelector ].GetOffsetX(); }
    uint32_t GetOffsetY() const { return mRegions[ mRegionSelector ].GetOffsetY(); }
    PvPixelType GetPixelType() const { return mRegions[ mRegionSelector ].GetPixelType( mComponentSelector ); }
    void GetWidthInfo( uint32_t &aMin, uint32_t &aMax, uint32_t &aInc ) const;
    void GetHeightInfo( uint32_t &aMin, uint32_t &aMax, uint32_t &aInc ) const;
    uint32_t GetChunksSize() const { return GetRequiredChunkSize(); }
    uint32_t GetPayloadSize() const;
    PvScanType GetScanType() const { return PvScanTypeArea3D; }
    PvResult GetSupportedPixelType( int aIndex, PvPixelType &aPixelType ) const;
    PvResult GetSupportedChunk( int aIndex, uint32_t &aID, PvString &aName ) const;
    bool GetChunkModeActive() const { return mChunkModeActive; }
    bool GetChunkEnable( uint32_t aChunkID ) const;
    //PvResult SetWidth( uint32_t aWidth ) { mRegions[ mRegionSelector ].SetWidth( aWidth ); return PvResult::Code::OK; }
    //PvResult SetHeight( uint32_t aHeight ) { mRegions[ mRegionSelector ].SetHeight( aHeight ); return PvResult::Code::OK; }
    PvResult SetWidth( uint32_t aWidth );
    PvResult SetHeight( uint32_t aHeight );
    PvResult SetOffsetX( uint32_t aOffsetX )  { mRegions[ mRegionSelector ].SetOffsetX( aOffsetX ); return PvResult::Code::OK; }
    PvResult SetOffsetY( uint32_t aOffsetY )  { mRegions[mRegionSelector].SetOffsetY( aOffsetY ); return PvResult::Code::OK; }
    PvResult SetPixelType( PvPixelType aPixelType );
    PvResult SetChunkModeActive( bool aEnabled ) { mChunkModeActive = aEnabled; return PvResult::Code::OK; }
    PvResult SetChunkEnable( uint32_t aChunkID, bool aEnabled );
    void OnOpen( const PvString &aDestIP, uint16_t aDestPort );
    void OnClose();
    void OnStreamingStart();
    void OnStreamingStop();
    PvBuffer *AllocBuffer();
    void FreeBuffer( PvBuffer *aBuffer );
    PvResult QueueBuffer( PvBuffer *aBuffer );
    PvResult RetrieveBuffer( PvBuffer **aBuffer );
    void AbortQueuedBuffers();
    void CreateRegisters( IPvRegisterMap *aRegisterMap, IPvRegisterFactory *aFactory );
    void CreateGenApiFeatures( IPvRegisterMap *aRegisterMap, IPvGenApiFactory *aFactory );
    bool IsPayloadTypeSupported( PvPayloadType aPayloadType );
    void SetMultiPartAllowed( bool aAllowed );
    PvResult SetTestPayloadFormatMode( PvPayloadType aPayloadType );

    // IPvRegisterEventSink
    PvResult PreRead( IPvRegister *aRegister );
    PvResult PreWrite( IPvRegister *aRegister );
    void PostWrite( IPvRegister *aRegister );
    PvResult Persist( IPvRegister *aRegister, IPvRegisterStore *aStore );

protected:

    uint32_t GetRequiredChunkSize() const;
    void SetupIfNeeded( PvBuffer *aBuffer );
    void SetupMultiPartIfNeeded( PvBuffer *aBuffer );
    void AddChunkSample( IPvChunkData *aChunkData );

    PvResult PersistRegion( IPvRegister *aRegister, IPvRegisterStore *aStore );
    PvResult PersistRegionComponent( IPvRegister *aRegister, IPvRegisterStore *aStore );
    PvResult PersistPattern( IPvRegister *aRegister, IPvRegisterStore *aStore );
    PvResult PersistExtraction( IPvRegister *aRegister, IPvRegisterStore *aStore );
    PvResult PersistExtractionPattern( IPvRegister *aRegister, IPvRegisterStore *aStore );
    PvResult PersistExtractionCoordinate( IPvRegister *aRegister, IPvRegisterStore *aStore );
    PvResult PersistExtractionCoordinateTransform( IPvRegister *aRegister, IPvRegisterStore *aStore );
    PvResult PersistExposureTime( IPvRegister *aRegister, IPvRegisterStore *aStore );

private:

    uint32_t mSourceIndex;

    PvFPSStabilizer mStabilizer;

    uint32_t mBufferCount;
    PvBuffer *mAcquisitionBuffer;

    uint32_t mFrameCount;

    bool mChunkModeActive;
    bool mChunkSampleEnabled;
    bool mMultiPartAllowed;
    bool mLaserOn;

    PatternEnum mPatternSelector;
    Scan3dExtractionEnum mExtractionSelector;
    CoordinateEnum mCoordinateSelector;
    TransformEnum mCoordinateTransformSelector;
    PatternEnum mExposureTimeSelector;
    ComponentEnum mComponentSelector;
    RegionEnum mRegionSelector;

    Region mRegions[ REGIONCOUNT ];
    Scan3dExtraction mExtractions[ EXTRACTIONCOUNT ];
    Scan3dPattern mPattern[ PATTERNCOUNT ];

    // VC_nano_3D Camera output
    // detected valid laser point
    int mDetectedLaserPoints;
    // accurate detected laser position X in WORLD coordinates [mm] 
    float *mpLaserPosX_MM;
    // accurate detected laser position Z in WORLD coordinates [mm] 
    float *mpLaserPosZ_MM;
 
};
