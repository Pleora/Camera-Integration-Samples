// *****************************************************************************
//
// Copyright (c) 2018, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "Defines.h"
#include "Source3d.h"
#include "Utilities.h"
#include "Camera.h"
#include <PvSampleUtils.h>


///
/// \brief Constructor
///

Source3d::Source3d( uint32_t aSourceIndex )
    : mSourceIndex( aSourceIndex )
    , mBufferCount( 0 )
    , mAcquisitionBuffer( NULL )
    , mFrameCount( 0 )
    , mChunkModeActive( false )
    , mChunkSampleEnabled( false )
    , mMultiPartAllowed( false )
    , mLaserOn(false)

#ifdef SCAN3D_FEATURE_ENABLE
    , mPatternSelector( PatternGraycode )
    , mExtractionSelector( Scan3dExtraction0 )
    , mCoordinateSelector( CoordinateA )
    , mCoordinateTransformSelector( TransformRotationX )
#endif

    , mExposureTimeSelector( PatternGraycode )
    , mComponentSelector( ComponentIntensity )
    , mRegionSelector( Region0 )
{
    // Complete regions definition
    mRegions[ 0 ].SetIDs( 0, 0 ); // Region 0, source 0
    mRegions[ 0 ].AddComponent( ComponentIntensity, PvPixelMono8 );

#ifdef SCAN3D_FEATURE_ENABLE
    mRegions[ 1 ].SetIDs( 1, 1 ); // Region 1, source 1
    mRegions[ 1 ].AddComponent( ComponentIntensity, PvPixelMono8 );
    mRegions[ 2 ].SetIDs( 2, 0 ); // Region 2, source 0
    mRegions[ 2 ].AddComponent( ComponentRange, PvPixelCoord3D_C32f );
    mRegions[ 2 ].AddComponent( ComponentConfidence, PvPixelConfidence1 );
    mRegions[ 3 ].SetIDs( 3, 1 ); // Region 3, source 1
    mRegions[ 3 ].AddComponent( ComponentRange, PvPixelCoord3D_C32f );
    mRegions[ 3 ].AddComponent( ComponentConfidence, PvPixelConfidence1 );
#endif

    // VC_nano_3D Camera  
    camera_init();
    mDetectedLaserPoints = 0; 
    mpLaserPosX_MM = (float *)malloc(get_camera_laser_maxpoints() * sizeof(float));;
    mpLaserPosZ_MM = (float *)malloc(get_camera_laser_maxpoints() * sizeof(float));;
}


///
/// \brief Destructor
///

Source3d::~Source3d()
{
  // VC_nano_3D Camera clear up 
    camera_delete();
    free(mpLaserPosX_MM);
    free(mpLaserPosZ_MM);
}

/// \brief Width info query
///

void Source3d::GetWidthInfo( uint32_t &aMin, uint32_t &aMax, uint32_t &aInc ) const
{
    aMin = WIDTH_MIN;
    aMax = WIDTH_MAX;
    aInc = WIDTH_INC;
}


///
/// \brief Height info query
///

void Source3d::GetHeightInfo( uint32_t &aMin, uint32_t &aMax, uint32_t &aInc ) const
{
    aMin = HEIGHT_MIN;
    aMax = HEIGHT_MAX;
    aInc = HEIGHT_INC;
}
// Width setter
PvResult Source3d::SetWidth( uint32_t aWidth )
{
    if ( ( aWidth < WIDTH_MIN ) || ( aWidth > WIDTH_MAX ) )
    {
        return PvResult::Code::INVALID_PARAMETER;
    }
    mRegions[ mRegionSelector ].SetWidth( aWidth ); 
    set_camera_width(aWidth);
    return PvResult::Code::OK;
}

// Hight setter
PvResult Source3d::SetHeight( uint32_t aHeight )
{
    if ( ( aHeight < HEIGHT_MIN ) || ( aHeight > HEIGHT_MAX ) )
    {
        return PvResult::Code::INVALID_PARAMETER;
    }
    mRegions[ mRegionSelector ].SetHeight( aHeight ); 
    set_camera_height(aHeight);
    return PvResult::Code::OK;
}


//p/
/// \brief Returns the active payload size: sum of all active regions + chunks
///

uint32_t Source3d::GetPayloadSize() const
{
    uint32_t lPayloadSize = 0;

    if ( mMultiPartAllowed )
    {
        // Sum payload size of all regions
        for ( uint32_t i = 0; i < REGIONCOUNT; i++ )
        {
            lPayloadSize += mRegions[ i ].GetPayloadSize();
        }
    }
    else
    {
        // When multi-part is not allowed, we fall back on region 0
        lPayloadSize += mRegions[ 0 ].GetPayloadSize();
    }

    // Add chunk size if chunks are enabled
    if ( mChunkModeActive && mChunkSampleEnabled )
    {
        lPayloadSize += CHUNKSIZE;
    }

    return lPayloadSize;
}


///
/// \brief Pixel type setter
///

PvResult Source3d::SetPixelType( PvPixelType aPixelType )
{
    mRegions[ mRegionSelector ].SetPixelType( mComponentSelector, aPixelType );
    return PvResult::Code::OK;
}


///
/// \brief Supported pixel types query
///

PvResult Source3d::GetSupportedPixelType( int aIndex, PvPixelType &aPixelType ) const
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

    case 3:
        aPixelType = PvPixelCoord3D_C32f;
        return PvResult::Code::OK;

    case 4:
        aPixelType = PvPixelCoord3D_ABC32f;
        return PvResult::Code::OK;

    case 5:
        aPixelType = PvPixelConfidence1;
        return PvResult::Code::OK;

    case 6:
        aPixelType = PvPixelConfidence1p;
        return PvResult::Code::OK;

    case 7:
        aPixelType = PvPixelConfidence8;
        return PvResult::Code::OK;

    default:
        break;
    }

    return PvResult::Code::INVALID_PARAMETER;
}


///
/// \brief Chunk support
///

PvResult Source3d::GetSupportedChunk( int aIndex, uint32_t &aID, PvString &aName ) const
{
    switch ( aIndex )
    {
    case 0:
        aID = CHUNKID;
        aName = "Sample";
        return PvResult::Code::OK;

    default:
        break;
    }

    return PvResult::Code::INVALID_PARAMETER;
}


///
/// \brief Chunk enabled getter, by chunk type
///

bool Source3d::GetChunkEnable( uint32_t aChunkID ) const
{
    switch ( aChunkID )
    {
    case CHUNKID:
        return mChunkSampleEnabled;

    default:
        break;
    }

    return false;
}


///
/// \brief Chunk enabled setter, by chunk type
///

PvResult Source3d::SetChunkEnable( uint32_t aChunkID, bool aEnabled )
{
    switch ( aChunkID )
    {
    case CHUNKID:
        mChunkSampleEnabled = aEnabled;
        return PvResult::Code::OK;

    default:
        break;
    }

    return PvResult::Code::INVALID_PARAMETER;
}


///
/// \brief Stream channel open notification
///

void Source3d::OnOpen( const PvString &aDestIP, uint16_t aDestPort )
{
    std::cout << "Streaming channel opened to " << aDestIP.GetAscii() << ":" << aDestPort << std::endl;
}


///
/// \brief Stream channel close notification
///

void Source3d::OnClose()
{
    std::cout << "Streaming channel closed" << std::endl;
}


///
/// \brief Streaming start notification
///

void Source3d::OnStreamingStart()
{
    std::cout << "Streaming start" << std::endl;
    //for VC_Nano3D_V camera
    SetMultiPartAllowed(mLaserOn);
    mStabilizer.Reset();
}


///
/// \brief Streaming stop notification
///

void Source3d::OnStreamingStop()
{
    std::cout << "Streaming stop" << std::endl;
}


///
/// \brief Request for a new buffer.
///
/// Return a new buffer until buffer count is reached, then return NULL to 
/// signal that all buffers have been allocated.
///

PvBuffer *Source3d::AllocBuffer()
{
    if ( mBufferCount < BUFFERCOUNT )
    {
        mBufferCount++;
        return new PvBuffer( PvPayloadTypeMultiPart );
    }

    return NULL;
}


///
/// \brief Request to free one of the buffers allocated with AllocBuffer
///

void Source3d::FreeBuffer( PvBuffer *aBuffer )
{
    PVDELETE( aBuffer );
    mBufferCount--;
}


///
/// \brief Request to queue a buffer for acquisition.
///
/// Return OK if the buffer is queued or any error if no more room in acquisition queue
///

PvResult Source3d::QueueBuffer( PvBuffer *aBuffer )
{
    // We use mAcqusitionBuffer as a 1-deep acquisition pipeline
    uint8_t *pBuffPointer = NULL;
    uint8_t *cameraBuffer = get_camera_image_3D_data(&mDetectedLaserPoints, mpLaserPosX_MM, mpLaserPosZ_MM);
    if (cameraBuffer == NULL) return PvResult::Code::NO_AVAILABLE_DATA;
    if ( mAcquisitionBuffer == NULL )
    {
        // No buffer queued, accept it
        mAcquisitionBuffer = aBuffer;

        if ( mMultiPartAllowed )
        {
            // Ensure the buffer is setup for our region setup
            SetupMultiPartIfNeeded( mAcquisitionBuffer );

            // Acquire buffer - could be done in another thread
            uint32_t lPartIndex = 0;
            IPvMultiPartContainer *lMPC = mAcquisitionBuffer->GetMultiPartContainer();
#ifdef SCAN3D_FEATURE_ENABLE
            for ( uint32_t i = 0; i < REGIONCOUNT; i++ )
            {
                uint32_t lComponentCount = mRegions[ i ].GetComponentCount();
                for ( uint32_t j = 0; j < lComponentCount; j++ )
                {

                    if ( mRegions[ i ].GetComponentEnable( j ) )
                    {
                        mRegions[ i ].Fill( j, lMPC->GetPart( lPartIndex++ ) );
                    }
                }
            }
#else
            pBuffPointer = lMPC->GetPart( 0 )->GetImage()->GetDataPointer();
            memcpy (pBuffPointer, cameraBuffer, (lMPC->GetPart(0)->GetImage()->GetImageSize()));  
           // Add chunks if needed
           if ( mChunkModeActive || mChunkSampleEnabled )
           {
                AddChunkSample(lMPC->GetPart(1)->GetChunkData() );
           }
#endif
        }
        else
        {

           // Acquire buffer - could be done in another thread
           SetupIfNeeded( mAcquisitionBuffer );
#ifdef SCAN3D_FEATURE_ENABLE
           mRegions[ 0 ].Fill( mAcquisitionBuffer->GetImage() );
#else
           pBuffPointer = mAcquisitionBuffer->GetDataPointer();
           memcpy (pBuffPointer, cameraBuffer, (mAcquisitionBuffer->GetImage()->GetImageSize()));  
#endif
           // Add chunks if needed
           if ( mChunkModeActive || mChunkSampleEnabled )
           {
                AddChunkSample( mAcquisitionBuffer );
           }
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

PvResult Source3d::RetrieveBuffer( PvBuffer **aBuffer )
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

void Source3d::AbortQueuedBuffers()
{
}


///
/// \brief For GigE Vision Validation Framework and TestPayloadFormatMode GenICam XML definition
///

bool Source3d::IsPayloadTypeSupported( PvPayloadType aPayloadType )
{
    return ( aPayloadType == PvPayloadTypeMultiPart );
}


///
/// \brief Allows the streaming source to transmit multi-part data or not.
///
/// The streaming source is not allowed to stream multi-part data until allowed by the controlling application.
/// We have to assume it is false (not allowed) until this method is called with aAllowed set to true.
///

void Source3d::SetMultiPartAllowed( bool aAllowed )
{
    mMultiPartAllowed = aAllowed;
    if ( mMultiPartAllowed )
    {
        std::cout << "Multi-part allowed" << std::endl;
    }
    else
    {
        std::cout << "Multi-part not allowed" << std::endl;
    }
}


///
/// \brief When this method is called for multi-part, we have to make sure we can stream valid multi-part buffers.
///
/// Required for GigE Vision Validation Framework certification.
/// If aPayloadType is PvPayloadTypeMultiPart we have to go in multi-part validation mode.
/// If aPayloadType is PvPayloadTypeNone, we have to go back to the normal multi-part operation mode.
/// In our case we are already using a ready-to-use test pattern, both modes are the same, there is nothing specific to do.
///

PvResult Source3d::SetTestPayloadFormatMode( PvPayloadType aPayloadType )
{
    switch ( aPayloadType )
    {
    case PvPayloadTypeMultiPart:
        // Here we would make sure multi-part can be streamed from this source
        std::cout << "Setting TestPayloadFormatMode to PvPayloadTypeMultiPart" << std::endl;
        return PvResult::Code::OK;

    case PvPayloadTypeNone:
        // Here we would return the source in normal operation mode
        std::cout << "Disabling TestPayloadFormatMode" << std::endl;
        return PvResult::Code::OK;
        break;

    default:
        break;
    }

    std::cout << "Attempt at setting TestPayloadFormatMode to unexpected mode: " << aPayloadType << std::endl;
    return PvResult::Code::NOT_SUPPORTED;
}


///
/// \brief Request to create source-specific registers
///

void Source3d::CreateRegisters( IPvRegisterMap *aRegisterMap, IPvRegisterFactory *aFactory )
{
    PVUNREFPARAM( aRegisterMap );

    // Create new registers
    aFactory->AddRegister( "RegionSelector", REGIONSELECTOR_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "RegionMode", REGIONMODE_ADDR, 4, PvGenAccessModeReadWrite, this );
#ifdef SCAN3D_FEATURE_ENABLE
    aFactory->AddRegister( "Scan3dPatternSelector", SCAN3DPATTERNSELECTOR_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "Scan3dPatternCount", SCAN3DPATTERNCOUNT_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "Scan3dPatternPeriod", SCAN3DPATTERNPERIOD_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "Scan3dExtractionSelector", SCAN3DEXTRACTIONSELECTOR_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "Scan3dExtractionSource", SCAN3DEXTRACTIONSOURCE_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "Scan3dExtractionMethod", SCAN3DEXTRACTIONMETHOD_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "Scan3dCoordinateSystem", SCAN3DCOORDINATESYSTEM_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "Scan3dOutputMode", SCAN3DOUTPUTMODE_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "Scan3dDistanceUnit", SCAN3DDISTANCEUNIT_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "Scan3dPatternSelector", SCAN3DPATTERNSELECTOR_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "Scan3dExtractionPatternContrast", SCAN3DEXTRACTIONPATTERNCONTRAST_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "Scan3dCoordinateSelector", SCAN3DCOORDINATESELECTOR_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "Scan3dCoordinateScale", SCAN3DCOORDINATESCALE_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "Scan3dCoordinateOffset", SCAN3DCOORDINATEOFFSET_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "Scan3dCoordinateTransformSelector", SCAN3DCOORDINATETRANSFORMSELECTOR_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "Scan3dTransformValue", SCAN3DTRANSFORMVALUE_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "Scan3dCoordinateSystemReference", SCAN3DCOORDINATESYSTEMREFERENCE_ADDR, 4, PvGenAccessModeReadWrite, this );
#endif   
    aFactory->AddRegister( "ComponentSelector", COMPONENTSELECTOR_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "ComponentEnable", COMPONENTENABLE_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "ExposureTimeSelector", EXPOSURETIMESELECTOR_ADDR, 4, PvGenAccessModeReadWrite, this );
    aFactory->AddRegister( "ExposureTime", EXPOSURETIME_ADDR, 4, PvGenAccessModeReadWrite, this );

    // For VC_nano3d_z camera
    aFactory->AddRegister("Shutter", SHUTTER_ADDR, 4, PvGenAccessModeReadWrite, this);
    aFactory->AddRegister("Gain", GAIN_ADDR, 4, PvGenAccessModeReadWrite, this);
    aFactory->AddRegister("LaserMode", LASERMODE_ADDR, 4, PvGenAccessModeReadWrite, this);
    aFactory->AddRegister("ExposureMode", EXPOSUREMODE_ADDR, 4, PvGenAccessModeReadWrite, this);
}


///
/// \brief Request to create source-specific GenApi features
///
/// Important: features affected by selectors need to have their selectors reported
/// as 'invalidators' so that their host-side GenApi cache gets invalidated when
/// the value of one of their selector changes.
///

void Source3d::CreateGenApiFeatures( IPvRegisterMap *aRegisterMap, IPvGenApiFactory *aFactory )
{
    aFactory->SetName( "RegionSelector" );
    aFactory->SetDescription( "Selects the Region of interest to control. The RegionSelector feature allows devices "
        "that are able to extract multiple regions out of an image, to configure the features of those individual regions independently. " );
    aFactory->SetCategory( "ImageFormatControl" );
    aFactory->SetNameSpace( PvGenNameSpaceStandard );
    aFactory->SetStreamable( false );
    aFactory->AddEnumEntry( "Region0", Region0 );
#ifdef  SCAN3D_FEATURE_ENABLE
    aFactory->AddEnumEntry( "Region1", Region1 );
#endif
    aFactory->AddEnumEntry( "Scan3dExtraction0", RegionScan3dExtraction0 );
#ifdef  SCAN3D_FEATURE_ENABLE
    aFactory->AddEnumEntry( "Scan3dExtraction1", RegionScan3dExtraction1 );
#endif
    aFactory->AddSelected( "RegionMode" );
    aFactory->AddSelected( "WidthMax" );
    aFactory->AddSelected( "HeightMax" );
    aFactory->AddSelected( "Width" );
    aFactory->AddSelected( "Height" );
    aFactory->AddSelected( "OffsetX" );
    aFactory->AddSelected( "OffsetY" );
    aFactory->AddSelected( "PixelFormat" );
    aFactory->AddSelected( "PixelSize" );
    aFactory->CreateEnum( aRegisterMap->GetRegisterByAddress( REGIONSELECTOR_ADDR ) );

    aFactory->SetName( "RegionMode" );
    aFactory->SetDescription( "Controls if the selected Region of interest is active and streaming." );
    aFactory->SetCategory( "ImageFormatControl" );
    aFactory->SetNameSpace( PvGenNameSpaceStandard );
    aFactory->SetTLLocked( true );
    aFactory->SetStreamable( true );
    aFactory->AddEnumEntry( "Off", 0 );
    aFactory->AddEnumEntry( "On", 1 );
    aFactory->AddInvalidator( "RegionSelector" );
    aFactory->CreateEnum( aRegisterMap->GetRegisterByAddress( REGIONMODE_ADDR ) );

#ifdef  SCAN3D_FEATURE_ENABLE
    aFactory->SetName( "Scan3dPatternSelector" );
    aFactory->SetDescription( DESCRIPTION );
    aFactory->SetCategory( "Scan3dControl" );
    aFactory->SetNameSpace( PvGenNameSpaceCustom );
    aFactory->SetStreamable( false );
    aFactory->AddEnumEntry( "Graycode", PatternGraycode );
    aFactory->AddEnumEntry( "Sine", PatternSine );
    aFactory->AddSelected( "Scan3dPatternCount" );
    aFactory->AddSelected( "Scan3dPatternPeriod" );
    aFactory->AddSelected( "Scan3dExtractionPatternContrast" );
    aFactory->CreateEnum( aRegisterMap->GetRegisterByAddress( SCAN3DPATTERNSELECTOR_ADDR ) );

    aFactory->SetName( "Scan3dPatternCount" );
    aFactory->SetDescription( DESCRIPTION );
    aFactory->SetCategory( "Scan3dControl" );
    aFactory->SetNameSpace( PvGenNameSpaceCustom );
    aFactory->SetTLLocked( true );
    aFactory->SetStreamable( true );
    aFactory->AddInvalidator( "Scan3dPatternSelector" );
    aFactory->CreateInteger( aRegisterMap->GetRegisterByAddress( SCAN3DPATTERNCOUNT_ADDR ), 0, 256 );

    aFactory->SetName( "Scan3dPatternPeriod" );
    aFactory->SetDescription( DESCRIPTION );
    aFactory->SetCategory( "Scan3dControl" );
    aFactory->SetNameSpace( PvGenNameSpaceCustom );
    aFactory->SetTLLocked( true );
    aFactory->SetStreamable( true );
    aFactory->AddInvalidator( "Scan3dPatternSelector" );
    aFactory->CreateInteger( aRegisterMap->GetRegisterByAddress( SCAN3DPATTERNPERIOD_ADDR ), 0, 256 );

    aFactory->SetName( "Scan3dExtractionSelector" );
    aFactory->SetDescription( "Selects the 3DExtraction processing module to control (if multiple ones are present)." );
    aFactory->SetCategory( "Scan3dControl" );
    aFactory->SetNameSpace( PvGenNameSpaceStandard );
    aFactory->SetStreamable( false );
    for ( int i = 0; i < EXTRACTIONCOUNT; i++ )
    {
        std::stringstream lSS;
        lSS << "Scan3dExtraction" << i;
        aFactory->AddEnumEntry( lSS.str().c_str(), i );
    }
    aFactory->AddSelected( "Scan3dExtractionSource" );
    aFactory->AddSelected( "Scan3dExtractionMethod" );
    aFactory->AddSelected( "Scan3dCoordinateSystem" );
    aFactory->AddSelected( "Scan3dOutputMode" );
    aFactory->AddSelected( "Scan3dDistanceUnit" );
    aFactory->AddSelected( "Scan3dExtractionPatternContrast" );
    aFactory->AddSelected( "Scan3dCoordinateScale" );
    aFactory->AddSelected( "Scan3dCoordinateOffset" );
    aFactory->AddSelected( "Scan3dTransformValue" );
    aFactory->AddSelected( "Scan3dCoordinateSystemReference" );
    aFactory->CreateEnum( aRegisterMap->GetRegisterByAddress( SCAN3DEXTRACTIONSELECTOR_ADDR ) );

    aFactory->SetName( "Scan3dExtractionSource" );
    aFactory->SetDescription( "Selects the sensor's data source region for 3D Extraction module. Typically these are Sensor Regions, but it could also be another ProcessingModule." );
    aFactory->SetCategory( "Scan3dControl" );
    aFactory->SetNameSpace( PvGenNameSpaceStandard );
    aFactory->SetTLLocked( true );
    aFactory->SetStreamable( true );
    aFactory->AddEnumEntry( "Region0", Region0 );
    aFactory->AddEnumEntry( "Region1", Region1 );
    aFactory->AddEnumEntry( "Scan3dExtraction0", RegionScan3dExtraction0 );
    aFactory->AddEnumEntry( "Scan3dExtraction1", RegionScan3dExtraction1 );
    aFactory->AddInvalidator( "Scan3dExtractionSelector" );
    aFactory->CreateEnum( aRegisterMap->GetRegisterByAddress( SCAN3DEXTRACTIONSOURCE_ADDR ) );

    aFactory->SetName( "Scan3dExtractionMethod" );
    aFactory->SetDescription( "Selects the method for extracting 3D from the input sensor data." );
    aFactory->SetCategory( "Scan3dControl" );
    aFactory->SetNameSpace( PvGenNameSpaceStandard );
    aFactory->SetTLLocked( true );
    aFactory->SetStreamable( true );
    aFactory->AddEnumEntry( "Default", MethodDefault );
    aFactory->AddEnumEntry( "Filtered", MethodFiltered);
    aFactory->AddEnumEntry( "Binning3d", MethodBinning3d );
    aFactory->AddEnumEntry( "HighSpeed", MethodHighSpeed );
    aFactory->AddEnumEntry( "HighResolution", MethodHighResolution );
    aFactory->AddInvalidator( "Scan3dExtractionSelector" );
    aFactory->CreateEnum( aRegisterMap->GetRegisterByAddress( SCAN3DEXTRACTIONMETHOD_ADDR ) );

    aFactory->SetName( "Scan3dCoordinateSystem" );
    aFactory->SetDescription( "Specifies the Coordinate system to use for the device" );
    aFactory->SetCategory( "Scan3dControl" );
    aFactory->SetNameSpace( PvGenNameSpaceStandard );
    aFactory->SetTLLocked( true );
    aFactory->SetStreamable( true );
    aFactory->AddInvalidator( "Scan3dExtractionSelector" );
    aFactory->AddEnumEntry( "Cartesian", CoordinateSystemCartesian );
    aFactory->AddEnumEntry( "Spherical", CoordinateSystemSpherical );
    aFactory->AddEnumEntry( "Cylindrical", CoordinateSystemCylindrical );
    aFactory->AddInvalidator( "Scan3dExtractionSelector" );
    aFactory->CreateEnum( aRegisterMap->GetRegisterByAddress( SCAN3DCOORDINATESYSTEM_ADDR ) );

    aFactory->SetName( "Scan3dCoordinateSystemReference" );
    aFactory->SetDescription( "Defines coordinate system reference location." );
    aFactory->SetCategory( "Scan3dControl" );
    aFactory->SetNameSpace( PvGenNameSpaceStandard );
    aFactory->SetTLLocked( true );
    aFactory->SetStreamable( true );
    aFactory->AddEnumEntry( "Anchored", CoordinateSystemReferenceAnchored );
    aFactory->AddEnumEntry( "Transformed", CoordinateSystemReferenceTransformed );
    aFactory->AddInvalidator( "Scan3dExtractionSelector" );
    aFactory->CreateEnum( aRegisterMap->GetRegisterByAddress( SCAN3DCOORDINATESYSTEMREFERENCE_ADDR ) );

    aFactory->SetName( "Scan3dOutputMode" );
    aFactory->SetDescription( "Calibrated Mode of the payload image." );
    aFactory->SetCategory( "Scan3dControl" );
    aFactory->SetNameSpace( PvGenNameSpaceStandard );
    aFactory->SetTLLocked( true );
    aFactory->SetStreamable( true );
    aFactory->AddInvalidator( "Scan3dExtractionSelector" );
    aFactory->AddEnumEntry( "UncalibratedC", OutputModeUncalibratedC );
    aFactory->AddEnumEntry( "CalibratedABC_Grid", OutputModeCalibratedABC_Grid );
    aFactory->AddEnumEntry( "CalibratedABC_PointCloud", OutputModeCalibratedABC_PointCloud );
    aFactory->AddEnumEntry( "CalibratedAC", OutputModeCalibratedAC );
    aFactory->AddEnumEntry( "CalibratedAC_Linescan", OutputModeCalibratedAC_Linescan );
    aFactory->AddEnumEntry( "CalibratedC", OutputModeCalibratedC );
    aFactory->AddEnumEntry( "CalibratedC_Linescan", OutputModeCalibratedC_Linescan );
    aFactory->AddEnumEntry( "RectifiedC", OutputModeRectifiedC );
    aFactory->AddEnumEntry( "RectifiedC_LInescan", OutputModeRectifiedC_Linescan );
    aFactory->AddEnumEntry( "DisparityC", OutputModeDisparityC );
    aFactory->AddInvalidator( "Scan3dExtractionSelector" );
    aFactory->CreateEnum( aRegisterMap->GetRegisterByAddress( SCAN3DOUTPUTMODE_ADDR ) );

    aFactory->SetName( "Scan3dDistanceUnit" );
    aFactory->SetDescription( "Returns the Distance Unit of the payload image." );
    aFactory->SetCategory( "Scan3dControl" );
    aFactory->SetNameSpace( PvGenNameSpaceStandard );
    aFactory->SetTLLocked( true );
    aFactory->SetStreamable( true );
    aFactory->AddEnumEntry( "Millimeter", DistanceUnitMillimeter );
    aFactory->AddEnumEntry( "Inch", DistanceUnitInch );
    aFactory->AddInvalidator( "Scan3dExtractionSelector" );
    aFactory->CreateEnum( aRegisterMap->GetRegisterByAddress( SCAN3DDISTANCEUNIT_ADDR ) );

    aFactory->SetName( "Scan3dExtractionPatternContrast" );
    aFactory->SetDescription( DESCRIPTION );
    aFactory->SetCategory( "Scan3dControl" );
    aFactory->SetNameSpace( PvGenNameSpaceCustom );
    aFactory->SetTLLocked( true );
    aFactory->SetStreamable( true );
    aFactory->AddInvalidator( "Scan3dExtractionSelector" );
    aFactory->AddInvalidator( "Scan3dPatternSelector" );
    aFactory->CreateInteger( aRegisterMap->GetRegisterByAddress( SCAN3DEXTRACTIONPATTERNCONTRAST_ADDR ), 0, 256 );

    aFactory->SetName( "Scan3dCoordinateSelector" );
    aFactory->SetDescription( "Selects the individual coordinates in the vectors for 3D information/transformation." );
    aFactory->SetCategory( "Scan3dControl" );
    aFactory->SetNameSpace( PvGenNameSpaceStandard );
    aFactory->SetStreamable( false );
    aFactory->AddEnumEntry( "CoordinateA", CoordinateA );
    aFactory->AddEnumEntry( "CoordinateB", CoordinateB );
    aFactory->AddEnumEntry( "CoordinateC", CoordinateC );
    aFactory->AddSelected( "Scan3dCoordinateScale" );
    aFactory->AddSelected( "Scan3dCoordinateOffset" );
    aFactory->CreateEnum( aRegisterMap->GetRegisterByAddress( SCAN3DCOORDINATESELECTOR_ADDR ) );

    aFactory->SetName( "Scan3dCoordinateScale" );
    aFactory->SetDescription( "Scale factor when transforming a pixel from relative coordinates to world coordinates." );
    aFactory->SetCategory( "Scan3dControl" );
    aFactory->SetNameSpace( PvGenNameSpaceStandard );
    aFactory->SetTLLocked( true );
    aFactory->SetStreamable( true );
    aFactory->AddInvalidator( "Scan3dExtractionSelector" );
    aFactory->AddInvalidator( "Scan3dCoordinateSelector" );
    aFactory->CreateFloat( aRegisterMap->GetRegisterByAddress( SCAN3DCOORDINATESCALE_ADDR ), -1000.0, 1000.0 );

    aFactory->SetName( "Scan3dCoordinateOffset" );
    aFactory->SetDescription( "Offset when transforming a pixel from relative coordinates to world coordinates." );
    aFactory->SetCategory( "Scan3dControl" );
    aFactory->SetNameSpace( PvGenNameSpaceStandard );
    aFactory->SetTLLocked( true );
    aFactory->SetStreamable( true );
    aFactory->AddInvalidator( "Scan3dExtractionSelector" );
    aFactory->AddInvalidator( "Scan3dCoordinateSelector" );
    aFactory->CreateFloat( aRegisterMap->GetRegisterByAddress( SCAN3DCOORDINATEOFFSET_ADDR ), -1000.0, 1000.0 );

    aFactory->SetName( "Scan3dCoordinateTransformSelector" );
    aFactory->SetDescription( "Sets the index to read/write a coordinate transform value." );
    aFactory->SetCategory( "Scan3dControl" );
    aFactory->SetNameSpace( PvGenNameSpaceStandard );
    aFactory->SetStreamable( false );
    aFactory->AddEnumEntry( "RotationX", TransformRotationX );
    aFactory->AddEnumEntry( "RotationY", TransformRotationY );
    aFactory->AddEnumEntry( "RotationZ", TransformRotationZ );
    aFactory->AddEnumEntry( "TranslationX", TransformTranslationX );
    aFactory->AddEnumEntry( "TranslationY", TransformTranslationY );
    aFactory->AddEnumEntry( "TranslationZ", TransformTranslationZ );
    aFactory->AddSelected( "Scan3dTransformValue" );
    aFactory->CreateEnum( aRegisterMap->GetRegisterByAddress( SCAN3DCOORDINATETRANSFORMSELECTOR_ADDR ) );

    aFactory->SetName( "Scan3dTransformValue" );
    aFactory->SetDescription( "Specifies the transform value selected." );
    aFactory->SetCategory( "Scan3dControl" );
    aFactory->SetNameSpace( PvGenNameSpaceStandard );
    aFactory->SetTLLocked( true );
    aFactory->SetStreamable( true );
    aFactory->AddInvalidator( "Scan3dExtractionSelector" );
    aFactory->AddInvalidator( "Scan3dCoordinateTransformSelector" );
    aFactory->CreateFloat( aRegisterMap->GetRegisterByAddress( SCAN3DTRANSFORMVALUE_ADDR ), -1000.0, 1000.0 );
#endif
    aFactory->SetName( "ComponentSelector" );
    aFactory->SetDescription( "Selects a component to activate/deactivate its data streaming." );
    aFactory->SetCategory( "ImageFormatControl" );
    aFactory->SetNameSpace( PvGenNameSpaceStandard );
    aFactory->SetStreamable( false );
    aFactory->AddEnumEntry( "Intensity", ComponentIntensity );
#ifdef SCAN3D_FEATURE_ENABLE 
    aFactory->AddEnumEntry( "Range", ComponentRange );
    aFactory->AddEnumEntry( "Confidence", ComponentConfidence );
#endif
    aFactory->AddSelected( "ComponentEnable" );
    aFactory->AddSelected( "PixelFormat" );
    aFactory->CreateEnum( aRegisterMap->GetRegisterByAddress( COMPONENTSELECTOR_ADDR ) );

    aFactory->SetName( "ComponentEnable" );
    aFactory->SetDescription( "Controls if the selected component streaming is active." );
    aFactory->SetCategory( "ImageFormatControl" );
    aFactory->SetNameSpace( PvGenNameSpaceStandard );
    aFactory->SetTLLocked( true );
    aFactory->SetStreamable( true );
    aFactory->AddInvalidator( "RegionSelector" );
    aFactory->AddInvalidator( "ComponentSelector" );
    aFactory->CreateBoolean( aRegisterMap->GetRegisterByAddress( COMPONENTENABLE_ADDR ) );
#ifdef SCAN3D_FEATURE_ENABLE 
    aFactory->SetName( "ExposureTimeSelector" );
    aFactory->SetDescription( "Selects which exposure time is controlled by the ExposureTime feature. This allows for independent control over the exposure components." );
    aFactory->SetCategory( "AcquisitionControl" );
    aFactory->SetNameSpace( PvGenNameSpaceStandard );
    aFactory->SetStreamable( false );
    aFactory->AddEnumEntry( "Graycode", PatternGraycode );
    aFactory->AddEnumEntry( "Sine", PatternSine );
    aFactory->AddSelected( "ExposureTime" );
    aFactory->CreateEnum( aRegisterMap->GetRegisterByAddress( EXPOSURETIMESELECTOR_ADDR ) );

    aFactory->SetName( "ExposureTime" );
    aFactory->SetDescription( "Sets the Exposure time when ExposureMode is Timed and ExposureAuto is Off. This controls the duration where the photosensitive cells are exposed to light." );
    aFactory->SetCategory( "AcquisitionControl" );
    aFactory->SetNameSpace( PvGenNameSpaceStandard );
    aFactory->SetTLLocked( true );
    aFactory->SetStreamable( true );
    aFactory->AddInvalidator( "ExposureTimeSelector" );
    aFactory->CreateFloat( aRegisterMap->GetRegisterByAddress( EXPOSURETIME_ADDR ), 0.0, 1000.0 );
#endif
    // 
    //Special options for VC_nano3D_Z camera registers
    //
    aFactory->SetName("LaseMode");
    aFactory->SetDescription("LaserMode on/off.");
    aFactory->AddEnumEntry("OFF", 0);
    aFactory->AddEnumEntry("ON", 1);
    aFactory->AddEnumEntry("PULSE", 2);
    aFactory->SetCategory( "AcquisitionControl\\Vc3dCameraControl" );
    aFactory->SetNameSpace( PvGenNameSpaceCustom );
    aFactory->SetTLLocked(true);
    aFactory->CreateEnum( aRegisterMap->GetRegisterByAddress(LASERMODE_ADDR) );

    aFactory->SetName("ExposureMode");
    aFactory->SetDescription("exposure mode description. Selects Fixed exposure, auto exposure and double exposure.");
    aFactory->SetCategory("AcquisitionControl\\Vc3dCameraControl");
    aFactory->AddEnumEntry("FixedExposure", 0);
    aFactory->AddEnumEntry("AutoExposure", 1);
    aFactory->AddEnumEntry("DoubleExposure", 2);
    aFactory->SetNameSpace( PvGenNameSpaceCustom );
    aFactory->SetTLLocked(true);
    aFactory->CreateEnum(aRegisterMap->GetRegisterByAddress(EXPOSUREMODE_ADDR));

    aFactory->SetName("Shutter");
    aFactory->SetDescription("Adjust the shutter time. unit is us");
    aFactory->SetCategory("AcquisitionControl\\Vc3dCameraControl");
    aFactory->SetRepresentation(PvGenRepresentationLinear);
    aFactory->SetNameSpace( PvGenNameSpaceCustom );
    aFactory->SetTLLocked(true);
    aFactory->CreateInteger(aRegisterMap->GetRegisterByAddress(SHUTTER_ADDR), 0, 5000, 1);


    aFactory->SetName("Gain");
    aFactory->SetDescription("Gain");
    aFactory->SetCategory("AcquisitionControl\\Vc3dCameraControl");
    aFactory->SetRepresentation(PvGenRepresentationLinear);
    aFactory->SetTLLocked(true);
    aFactory->SetNameSpace( PvGenNameSpaceCustom );
    aFactory->CreateInteger(aRegisterMap->GetRegisterByAddress(GAIN_ADDR), 0, 480, 1);

 
    // Add callbacks for some image format registers we need for user sets persistence as
    // we are double-selecting the registers with RegionSelector and ComponentSelector
    aRegisterMap->GetRegisterByAddress( GEV_REG_VGEVD_WIDTH( mSourceIndex ) )->AddEventSink( this );
    aRegisterMap->GetRegisterByAddress( GEV_REG_VGEVD_HEIGHT( mSourceIndex ) )->AddEventSink( this );
    aRegisterMap->GetRegisterByAddress( GEV_REG_VGEVD_PIXELFORMAT( mSourceIndex ) )->AddEventSink( this );
    aRegisterMap->GetRegisterByAddress( GEV_REG_VGEVD_OFFSETX( mSourceIndex ) )->AddEventSink( this );
    aRegisterMap->GetRegisterByAddress( GEV_REG_VGEVD_OFFSETY( mSourceIndex ) )->AddEventSink( this );
}


///
/// \brief Pre-read register handler for this source.
///
/// Here we make sure the register is holding the value to read.
///

PvResult Source3d::PreRead( IPvRegister *aRegister )
{
    uint32_t lValue = 0;
    float lFValue = 0.0f;

    switch ( aRegister->GetAddress() )
    {
    case REGIONSELECTOR_ADDR:
        // Move current selector value to register
        lValue = mRegionSelector;
        return aRegister->Write( lValue );

    case REGIONMODE_ADDR:
        lValue = mRegions[ mRegionSelector ].GetMode();
        return aRegister->Write( lValue );

    case SCAN3DPATTERNSELECTOR_ADDR:
        // Move current selector value to register
        lValue = mPatternSelector;
        return aRegister->Write( lValue );

    case SCAN3DEXTRACTIONSELECTOR_ADDR:
        // Move current selector value to register
        lValue = mExtractionSelector;
        return aRegister->Write( mExtractionSelector );

    case SCAN3DCOORDINATESELECTOR_ADDR:
        // Move current selector value to register
        lValue = mCoordinateSelector;
        return aRegister->Write( lValue );

    case SCAN3DCOORDINATETRANSFORMSELECTOR_ADDR:
        // Move current selector value to register
        lValue = mCoordinateTransformSelector;
        return aRegister->Write( lValue );

    case EXPOSURETIMESELECTOR_ADDR:
        // Move current selector value to register
        lValue = mExposureTimeSelector;
        return aRegister->Write( lValue );

    case COMPONENTSELECTOR_ADDR:
        // Move current selector value to register
        lValue = mComponentSelector;
        return aRegister->Write( lValue );

    case SCAN3DPATTERNCOUNT_ADDR:
        // Selected by Scan3dPatternSelector
        lValue = mPattern[ mPatternSelector ].GetCount();
        return aRegister->Write( lValue );

    case SCAN3DPATTERNPERIOD_ADDR:
        // Selected by Scan3dPatternSelector
        lValue = mPattern[ mPatternSelector ].GetPeriod();
        return aRegister->Write( lValue );

    case SCAN3DEXTRACTIONSOURCE_ADDR:
        // Selected by Scan3dExtractionSelector
        lValue = mExtractions[ mExtractionSelector ].GetSource();
        return aRegister->Write( lValue );

    case SCAN3DEXTRACTIONMETHOD_ADDR:
        // Selected by Scan3dExtractionSelector
        lValue = mExtractions[ mExtractionSelector ].GetMethod();
        return aRegister->Write( lValue );

    case SCAN3DCOORDINATESYSTEM_ADDR:
        // Selected by Scan3dExtractionSelector
        lValue = mExtractions[ mExtractionSelector ].GetCoordinateSystem();
        return aRegister->Write( lValue );

    case SCAN3DOUTPUTMODE_ADDR:
        // Selected by Scan3dExtractionSelector
        lValue = mExtractions[ mExtractionSelector ].GetOutputMode();
        return aRegister->Write( lValue );

    case SCAN3DDISTANCEUNIT_ADDR:
        // Selected by Scan3dExtractionSelector
        lValue = mExtractions[ mExtractionSelector ].GetDistanceUnit();
        return aRegister->Write( lValue );

    case SCAN3DEXTRACTIONPATTERNCONTRAST_ADDR:
        // Selected by Scan3dExtractionSelector and Scan3dPatternSelector
        lValue = mExtractions[ mExtractionSelector ].GetPatternContrast( mPatternSelector );
        return aRegister->Write( lValue );

    case SCAN3DCOORDINATESCALE_ADDR:
        // Selected by Scan3dExtractionSelector and Scan3dCoordinateSelector
        lFValue = mExtractions[ mExtractionSelector ].Coordinate( mCoordinateSelector ).GetScale();
        return aRegister->WriteFloat( lFValue );

    case SCAN3DCOORDINATEOFFSET_ADDR:
        // Selected by Scan3dExtractionSelector and Scan3dCoordinateSelector
        lFValue = mExtractions[ mExtractionSelector ].Coordinate( mCoordinateSelector ).GetOffset();
        return aRegister->WriteFloat( lFValue );

    case SCAN3DTRANSFORMVALUE_ADDR:
        // Selected by Scan3dExtractionSelector and Scan3dCoordinateTransformSelector
        lFValue = mExtractions[ mExtractionSelector ].Transform().Get( mCoordinateTransformSelector );
        return aRegister->WriteFloat( lFValue );

    case SCAN3DCOORDINATESYSTEMREFERENCE_ADDR:
        // Selected by Scan3dExtractionSelector
        lValue = mExtractions[ mExtractionSelector ].GetCoordinateSystemReference();
        return aRegister->Write( lValue );

    case COMPONENTENABLE_ADDR:
        // Selected by RegionSelector and ComponentSelector
        lValue = mRegions[ mRegionSelector ].GetComponentEnable( mComponentSelector );
        return aRegister->Write( lValue );

    case EXPOSURETIME_ADDR:
        // Selected by ExposureTimeSelector (same enum entries as PatternSelector)
        lFValue = mPattern[ mExposureTimeSelector ].GetExposureTime();
        return aRegister->WriteFloat( lFValue );

     //Initialize the registers for VC_Nano3D_V camera
    case SHUTTER_ADDR:
       return aRegister->Write( 1000 );
    case GAIN_ADDR:
        return aRegister->Write( 100 );



    default:
        break;
    }

    return PvResult::Code::OK;
}


///
/// \brief Pre-write register handler for this source
///
/// This is where we would fail and invalid write operations. Int and float min/max
/// are enforced by the GenApi layer. Same about enums only allowing values from fixed
/// sets.
///
/// To be extremely thorough, a second layer of validation could be performed here.
///

PvResult Source3d::PreWrite( IPvRegister *aRegister )
{
    PVUNREFPARAM( aRegister );
    return PvResult::Code::OK;
}


///
/// \brief Post-write register handler for this source.
///
/// This is where we react to a value written to a register. Here we assume the value
/// has been validated and written.
///
/// This callback does not have a return value, it cannot fail. Failing a write operation
/// must happen in the PreWrite callback.
///

void Source3d::PostWrite( IPvRegister *aRegister )
{
    uint32_t lValue = 0;
    aRegister->Read( lValue );

    float lFValue = 0.0f;

    switch ( aRegister->GetAddress() )
    {
    case REGIONSELECTOR_ADDR:
        // Copy new selector value from register
        mRegionSelector = static_cast<RegionEnum>( lValue );
        std::cout << "RegionSelector = " << RegionToString( mRegionSelector ) << " (" << lValue << ")" << std::endl;
        return;

    case REGIONMODE_ADDR:
        mRegions[ mRegionSelector ].SetMode( lValue != 0 );
        std::cout << "RegionMode[" << RegionToString( mRegionSelector ) << "] = " <<
            ( ( lValue != 0 ) ? "true" : "false" ) << " (" << lValue << ")" << std::endl;
        return;

    case SCAN3DPATTERNSELECTOR_ADDR:
        // Copy new selector value from register
        mPatternSelector = static_cast<PatternEnum>( lValue );
        std::cout << "PatternSelector = " << PatternToString( mPatternSelector ) << " (" << lValue << ")" << std::endl;
        return;

    case SCAN3DEXTRACTIONSELECTOR_ADDR:
        // Copy new selector value from register
        mExtractionSelector = static_cast<Scan3dExtractionEnum>( lValue );
        std::cout << "ExtractionSelector = " << Scan3dExtractionToString( mExtractionSelector ) << " (" << lValue << ")" << std::endl;
        return;

    case SCAN3DCOORDINATESELECTOR_ADDR:
        // Copy new selector value from register
        mCoordinateSelector = static_cast<CoordinateEnum>( lValue );
        std::cout << "CoordinateSelector = " << CoordinateToString( mCoordinateSelector ) << " (" << lValue << ")" << std::endl;
        return;

    case SCAN3DCOORDINATETRANSFORMSELECTOR_ADDR:
        // Copy new selector value from register
        mCoordinateTransformSelector = static_cast<TransformEnum>( lValue );
        std::cout << "CoordinateTransformSelector = " << TransformToString( mCoordinateTransformSelector ) << " (" << lValue << ")" << std::endl;
        return;

    case EXPOSURETIMESELECTOR_ADDR:
        // Copy new selector value from register
        mExposureTimeSelector = static_cast<PatternEnum>( lValue );
        std::cout << "ExposureTimeSelector = " << PatternToString( mExposureTimeSelector ) << " (" << lValue << ")" << std::endl;
        return;

    case COMPONENTSELECTOR_ADDR:
        // Copy new selector value from register
        mComponentSelector = static_cast<ComponentEnum>( lValue );
        std::cout << "ComponentSelector = " << ComponentToString( mComponentSelector ) << " (" << lValue << ")" << std::endl;
        return;

    case SCAN3DPATTERNCOUNT_ADDR:
        // Selected by Scan3dPatternSelector
        mPattern[ mPatternSelector ].SetCount( lValue );
        std::cout << "Scan3dPatternCount[" << PatternToString( mPatternSelector ) << "] = " << lValue << std::endl;
        return;

    case SCAN3DPATTERNPERIOD_ADDR:
        // Selected by Scan3dPatternSelector
        mPattern[ mPatternSelector ].SetPeriod( lValue );
        std::cout << "Scan3dPatternPeriod[" << PatternToString( mPatternSelector ) << "] = " << lValue << std::endl;
        return;

    case SCAN3DEXTRACTIONSOURCE_ADDR:
        // Selected by Scan3dExtractionSelector
        mExtractions[ mExtractionSelector ].SetSource( static_cast<RegionEnum>( lValue ) );
        std::cout << "Scan3dExtractionSource[" << Scan3dExtractionToString( mExtractionSelector ) << "] = " <<
            RegionToString( mExtractions[ mExtractionSelector ].GetSource() ) << " (" << lValue << ")" << std::endl;
        return;

    case SCAN3DEXTRACTIONMETHOD_ADDR:
        // Selected by Scan3dExtractionSelector
        mExtractions[ mExtractionSelector ].SetMethod( static_cast<MethodEnum>( lValue ) );
        std::cout << "Scan3dExtractionMethod[" << Scan3dExtractionToString( mExtractionSelector ) << "] = " <<
            MethodToString( mExtractions[ mExtractionSelector ].GetMethod() ) << " (" << lValue << ")" << std::endl;
        return;

    case SCAN3DCOORDINATESYSTEM_ADDR:
        // Selected by Scan3dExtractionSelector
        mExtractions[ mExtractionSelector ].SetCoordinateSystem( static_cast<CoordinateSystemEnum>( lValue ) );
        std::cout << "Scan3dCoordinateSystem[" << Scan3dExtractionToString( mExtractionSelector ) << "] = " <<
            CoordinateSystemToString( mExtractions[ mExtractionSelector ].GetCoordinateSystem() ) << " (" << lValue << ")" << std::endl;
        return;

    case SCAN3DOUTPUTMODE_ADDR:
        // Selected by Scan3dExtractionSelector
        mExtractions[ mExtractionSelector ].SetOutputMode( static_cast<OutputModeEnum>( lValue ) );
        std::cout << "Scan3dOutputMode[" << Scan3dExtractionToString( mExtractionSelector ) << "] = " <<
            OutputModeToString( mExtractions[ mExtractionSelector ].GetOutputMode() ) << " (" << lValue << ")" << std::endl;
        return;

    case SCAN3DDISTANCEUNIT_ADDR:
        // Selected by Scan3dExtractionSelector
        mExtractions[ mExtractionSelector ].SetDistanceUnit( static_cast<DistanceUnitEnum>( lValue ) );
        std::cout << "Scan3dDistanceUnit[" << Scan3dExtractionToString( mExtractionSelector ) << "] = " <<
            DistanceUnitToString( mExtractions[ mExtractionSelector ].GetDistanceUnit() ) << " (" << lValue << ")" << std::endl;
        return;

    case SCAN3DEXTRACTIONPATTERNCONTRAST_ADDR:
        // Selected by Scan3dExtractionSelector and Scan3dPatternSelector
        mExtractions[ mExtractionSelector ].SetPatternContrast( mPatternSelector, lValue );
        std::cout << "Scan3dExtractionPatternContrast[" << Scan3dExtractionToString( mExtractionSelector ) << "]" <<
            "[" << PatternToString( mPatternSelector ) << "] = " << lValue << std::endl;
        return;

    case SCAN3DCOORDINATESCALE_ADDR:
        // Selected by Scan3dExtractionSelector and Scan3dCoordinateSelector
        aRegister->ReadFloat( lFValue );
        mExtractions[ mExtractionSelector ].Coordinate( mCoordinateSelector ).SetScale( lFValue );
        std::cout << "Scan3dCoordinateScale[" << Scan3dExtractionToString( mExtractionSelector ) << "]" <<
            "[" << CoordinateToString( mCoordinateSelector ) << "] = " << lFValue << std::endl;
        return;

    case SCAN3DCOORDINATEOFFSET_ADDR:
        // Selected by Scan3dExtractionSelector and Scan3dCoordinateSelector
        aRegister->ReadFloat( lFValue );
        mExtractions[ mExtractionSelector ].Coordinate( mCoordinateSelector ).SetOffset( lFValue );
        std::cout << "Scan3dCoordinateOffset[" << Scan3dExtractionToString( mExtractionSelector ) << "]" <<
            "[" << CoordinateToString( mCoordinateSelector ) << "] = " << lFValue << std::endl;
        return;

    case SCAN3DTRANSFORMVALUE_ADDR:
        // Selected by Scan3dExtractionSelector and Scan3dCoordinateTransformSelector
        aRegister->ReadFloat( lFValue );
        mExtractions[ mExtractionSelector ].Transform().Set( mCoordinateTransformSelector, lFValue );
        std::cout << "Scan3dTransformValue[" << Scan3dExtractionToString( mExtractionSelector ) << "]" <<
            "[" << TransformToString( mCoordinateTransformSelector ) << "] = " << lFValue << std::endl;
        return;

    case SCAN3DCOORDINATESYSTEMREFERENCE_ADDR:
        // Selected by Scan3dExtractionSelector
        mExtractions[ mExtractionSelector ].SetCoordinateSystemReference( static_cast<CoordinateSystemReferenceEnum>( lValue ) );
        std::cout << "Scan3dCoordinateSystemReference[" << Scan3dExtractionToString( mExtractionSelector ) << "] = " <<
            CoordinateSystemReferenceToString( mExtractions[ mExtractionSelector ].GetCoordinateSystemReference() ) << " (" << lValue << ")" << std::endl;
        return;

    case COMPONENTENABLE_ADDR:
        // Selected by RegionSelector and ComponentSelector
        mRegions[ mRegionSelector ].SetComponentEnable( mComponentSelector, ( lValue != 0 ) );
        std::cout << "ComponentEnable[" << RegionToString( mRegionSelector ) << "]" <<
            "[" << ComponentToString( mComponentSelector ) << "] = " << lValue << std::endl;
        return;

    case EXPOSURETIME_ADDR:
        // Selected by ExposureTimeSelector (same enum entries as PatternSelector)
        aRegister->ReadFloat( lFValue );
        mPattern[ mExposureTimeSelector ].SetExposureTime( lFValue );
        std::cout << "ExposureTime[" << PatternToString( mExposureTimeSelector ) << "] = " << lFValue << std::endl;
        return;

     //Get register value for VC_Nano3D_V camera
    case SHUTTER_ADDR:
       set_Shutter_time (lValue);
       return;
    case GAIN_ADDR:
         set_Gain (lValue);
         return;
    case LASERMODE_ADDR:
         if (lValue==2) lValue = 1;
         // when 1: turn on the laser, also enable chunk and  ChunkMode active 
         // when 0: turn off the laser and also disable the chunk and chunk mode active
         if (lValue == 1)
         {
            mLaserOn=true;
            SetChunkModeActive(true);
            SetChunkEnable(CHUNKID, true);
         }else{
            SetChunkEnable(CHUNKID, false);
            SetChunkEnable(CHUNKID, true);
            mLaserOn=false;
         }
         set_Laser_Mode(lValue);
         return;


    default:
        break;
    }
}


///
/// \brief Custom save handler, manages device-side selected feature saving.
///
/// User set persistence is performed at the register level. Some selected features
/// are selected on addresses (each instance has a unique register) and do not require special
/// persistence handling.
///
/// Some features only use one register which maps to multiple objects or variables 
/// depending on the state of its selector(s). Those features require special persistence
/// handling to be properly mapped to user sets.
///
/// To persist device-side selected registers the code needs to go through all possible
/// selector values and call IPvRegisterStore::Persist with the register, the store
/// and the a suffix to used to make the selected register instance unique. If a register
/// is affected by more than one selector, all possible combinations must be walked-through.
///
/// See PersistPattern, PersistExtraction, PersistExposureTime for single-selector examples.
/// All other Persist methods involve more than one selector.
///

PvResult Source3d::Persist( IPvRegister *aRegister, IPvRegisterStore *aStore )
{
    if ( ( aRegister->GetAddress() == GEV_REG_VGEVD_WIDTH( mSourceIndex ) ) ||
         ( aRegister->GetAddress() == GEV_REG_VGEVD_HEIGHT( mSourceIndex ) ) ||
         ( aRegister->GetAddress() == GEV_REG_VGEVD_OFFSETX( mSourceIndex ) ) ||
         ( aRegister->GetAddress() == GEV_REG_VGEVD_OFFSETY( mSourceIndex ) ) )
    {
        // Selected by RegionSelector and ComponentSelector
        return PersistRegion( aRegister, aStore );
    }

    if ( aRegister->GetAddress() == GEV_REG_VGEVD_PIXELFORMAT( mSourceIndex ) )
    {
        // Selected by RegionSelector and ComponentSelector
        return PersistRegionComponent( aRegister, aStore );
    }

    switch ( aRegister->GetAddress() )
    {
    case SCAN3DPATTERNCOUNT_ADDR:
    case SCAN3DPATTERNPERIOD_ADDR:
        // Selected by Scan3dPatternSelector
        return PersistPattern( aRegister, aStore );

    case SCAN3DEXTRACTIONSOURCE_ADDR:
    case SCAN3DEXTRACTIONMETHOD_ADDR:
    case SCAN3DCOORDINATESYSTEM_ADDR:
    case SCAN3DOUTPUTMODE_ADDR:
    case SCAN3DDISTANCEUNIT_ADDR:
    case SCAN3DCOORDINATESYSTEMREFERENCE_ADDR:
        // Selected by Scan3dExtractionSelector
        return PersistExtraction( aRegister, aStore );

    case SCAN3DEXTRACTIONPATTERNCONTRAST_ADDR:
        // Selected by Scan3dExtractionSelector and Scan3dPatternSelector
        return PersistExtractionPattern( aRegister, aStore );

    case SCAN3DCOORDINATESCALE_ADDR:
    case SCAN3DCOORDINATEOFFSET_ADDR:
        // Selected by Scan3dExtractionSelector and Scan3dCoordinateSelector
        return PersistExtractionCoordinate( aRegister, aStore );

    case SCAN3DTRANSFORMVALUE_ADDR:
        // Selected by Scan3dExtractionSelector and Scan3dCoordinateTransformSelector
        return PersistExtractionCoordinateTransform( aRegister, aStore );

    case REGIONMODE_ADDR:
        // Selected by RegionSelector
        return PersistRegion( aRegister, aStore );

    case COMPONENTENABLE_ADDR:
        // Selected by RegionSelector and ComponentSelector
        return PersistRegionComponent( aRegister, aStore );

    case EXPOSURETIME_ADDR:
        // Selected by ExposureTimeSelector (same enum entries as PatternSelector)
        return PersistExposureTime( aRegister, aStore );

    default:
        break;
    }

    return PvResult::Code::NOT_IMPLEMENTED;
}


///
/// \brief Persists a register selected by Scan3dPatternSelector
///

PvResult Source3d::PersistPattern( IPvRegister *aRegister, IPvRegisterStore *aStore )
{
    PatternEnum lOld = mPatternSelector;
    for ( int i = PatternGraycode; i <= PatternSine; i++ )
    {
        // Select, persist register (save or load depending on context)
        mPatternSelector = static_cast<PatternEnum>( i );
        aStore->Persist( aRegister, PatternToString( static_cast<PatternEnum>( i ) ).c_str() );
    }

    mPatternSelector = lOld;
    return PvResult::Code::OK;
}


///
/// \brief Persists a register selected by Scan3dExtractionSelector
///

PvResult Source3d::PersistExtraction( IPvRegister *aRegister, IPvRegisterStore *aStore )
{
    Scan3dExtractionEnum lOld = mExtractionSelector;
    for ( int i = Scan3dExtraction0; i <= Scan3dExtraction1; i++ )
    {
        // Select, persist register (save or load depending on context)
        mExtractionSelector = static_cast<Scan3dExtractionEnum>( i );
        aStore->Persist( aRegister, Scan3dExtractionToString( static_cast<Scan3dExtractionEnum>( i ) ).c_str() );
    }

    mExtractionSelector = lOld;
    return PvResult::Code::OK;
}


///
/// \brief Persists a register selected by Scan3dExtractionSelector and Scan3dPatternSelector
///

PvResult Source3d::PersistExtractionPattern( IPvRegister *aRegister, IPvRegisterStore *aStore )
{
    Scan3dExtractionEnum lOld1 = mExtractionSelector;
    PatternEnum lOld2 = mPatternSelector;

    for ( int i = Scan3dExtraction0; i <= Scan3dExtraction1; i++ )
    {
        // Select extraction
        mExtractionSelector = static_cast<Scan3dExtractionEnum>( i );
        for ( int j = PatternGraycode; j <= PatternSine; j++ )
        {
            // Select pattern
            mPatternSelector = static_cast<PatternEnum>( j );

            // Create persistence suffix
            std::string lSuffix = Scan3dExtractionToString( static_cast<Scan3dExtractionEnum>( i ) );
            lSuffix += PatternToString( static_cast<PatternEnum>( j ) );

            // Persist double-selected register (save or load depending on context)
            aStore->Persist( aRegister, lSuffix.c_str() );
        }
    }

    mPatternSelector = lOld2;
    mExtractionSelector = lOld1;

    return PvResult::Code::OK;
}


///
/// \brief Persists a register selected by Scan3dExtractionSelector and Scan3dCoordinateSelector
///

PvResult Source3d::PersistExtractionCoordinate( IPvRegister *aRegister, IPvRegisterStore *aStore )
{
    Scan3dExtractionEnum lOld1 = mExtractionSelector;
    CoordinateEnum lOld2 = mCoordinateSelector;

    for ( int i = Scan3dExtraction0; i <= Scan3dExtraction1; i++ )
    {
        // Select extraction
        mExtractionSelector = static_cast<Scan3dExtractionEnum>( i );
        for ( int j = CoordinateA; j <= CoordinateC; j++ )
        {
            // Select coordinate
            mCoordinateSelector = static_cast<CoordinateEnum>( j );

            // Create persistence suffix
            std::string lSuffix = Scan3dExtractionToString( static_cast<Scan3dExtractionEnum>( i ) );
            lSuffix += CoordinateToString( static_cast<CoordinateEnum>( j ) );

            // Persist double-selected register (save or load depending on context)
            aStore->Persist( aRegister, lSuffix.c_str() );
        }
    }

    mCoordinateSelector = lOld2;
    mExtractionSelector = lOld1;

    return PvResult::Code::OK;
}


///
/// \brief Persists a register selected by Scan3dExtractionSelector and Scan3dCoordinateTransformSelector
///

PvResult Source3d::PersistExtractionCoordinateTransform( IPvRegister *aRegister, IPvRegisterStore *aStore )
{
    Scan3dExtractionEnum lOld1 = mExtractionSelector;
    TransformEnum lOld2 = mCoordinateTransformSelector;

    for ( int i = Scan3dExtraction0; i <= Scan3dExtraction1; i++ )
    {
        // Select extraction
        mExtractionSelector = static_cast<Scan3dExtractionEnum>( i );
        for ( int j = TransformRotationX; j <= TransformTranslationZ; j++ )
        {
            // Select coordinate transform
            mCoordinateTransformSelector = static_cast<TransformEnum>( j );

            // Create persistence suffix
            std::string lSuffix = Scan3dExtractionToString( static_cast<Scan3dExtractionEnum>( i ) );
            lSuffix += TransformToString( static_cast<TransformEnum>( j ) );

            // Persist double-selected register (save or load depending on context)
            aStore->Persist( aRegister, lSuffix.c_str() );
        }
    }

    mCoordinateTransformSelector = lOld2;
    mExtractionSelector = lOld1;

    return PvResult::Code::OK;
}


///
/// \brief Persists a register selected by RegionSelector
///

PvResult Source3d::PersistRegion( IPvRegister *aRegister, IPvRegisterStore *aStore )
{
    RegionEnum lOld1 = mRegionSelector;

    for ( int i = Region0; i <= RegionScan3dExtraction1; i++ )
    {
        // Select region
        mRegionSelector = static_cast<RegionEnum>( i );

        // Create persistence suffix
        std::string lSuffix = RegionToString( static_cast<RegionEnum>( i ) );

        // Persist selected register (save or load depending on context)
        aStore->Persist( aRegister, lSuffix.c_str() );
    }

    mRegionSelector = lOld1;

    return PvResult::Code::OK;
}


///
/// \brief Persists a register selected by RegionSelector and ComponentSelector
///

PvResult Source3d::PersistRegionComponent( IPvRegister *aRegister, IPvRegisterStore *aStore )
{
    RegionEnum lOld1 = mRegionSelector;
    ComponentEnum lOld2 = mComponentSelector;

    for ( int i = Region0; i <= RegionScan3dExtraction1; i++ )
    {
        // Select region
        mRegionSelector = static_cast<RegionEnum>( i );
        for ( int j = ComponentIntensity; j <= ComponentScatter; j++ )
        {
            // Only persist component if the region supports it
            if ( !mRegions[ i ].HasComponent( static_cast<ComponentEnum>( j ) ) )
            {
                continue;
            }

            // Select component
            mComponentSelector = static_cast<ComponentEnum>( j );

            // Create persistence suffix
            std::string lSuffix = RegionToString( static_cast<RegionEnum>( i ) );
            lSuffix += ComponentToString( static_cast<ComponentEnum>( j ) );

            // Persist double-selected register (save or load depending on context)
            aStore->Persist( aRegister, lSuffix.c_str() );
        }
    }

    mComponentSelector = lOld2;
    mRegionSelector = lOld1;

    return PvResult::Code::OK;
}


///
/// \brief Persists a register selected by Scan3dExtractionSelector and ExposureTimeSelector
///

PvResult Source3d::PersistExposureTime( IPvRegister *aRegister, IPvRegisterStore *aStore )
{
    PatternEnum lOld = mExposureTimeSelector;
    for ( int i = PatternGraycode; i <= PatternSine; i++ )
    {
        // Select, persist register (save or load depending on context)
        mExposureTimeSelector = static_cast<PatternEnum>( i );
        aStore->Persist( aRegister, PatternToString( static_cast<PatternEnum>( i ) ).c_str() );
    }

    mExposureTimeSelector = lOld;
    return PvResult::Code::OK;
}


///
/// \brief Returns the required chunk size for the current configuration
///

uint32_t Source3d::GetRequiredChunkSize() const
{
    return ( mChunkModeActive && mChunkSampleEnabled ) ? CHUNKSIZE : 0;
}


///
/// \brief Prepares an image buffer for acquisition with the current configuration.
///
/// Used when multi-part is not allowed, defaults to region 0.
///

void Source3d::SetupIfNeeded( PvBuffer *aBuffer )
{
    if ( aBuffer->GetPayloadType() != PvPayloadTypeImage )
    {
        aBuffer->Reset( PvPayloadTypeImage );
    }

    uint32_t lRequiredChunkSize = ( mChunkModeActive && mChunkSampleEnabled ) ? CHUNKSIZE : 0;
    PvImage *lImage = aBuffer->GetImage();
    if ( ( lImage->GetWidth() != mRegions[ 0 ].GetWidth() ) ||
         ( lImage->GetHeight() != mRegions[ 0 ].GetHeight() ) ||
         ( lImage->GetPixelType() != mRegions[ 0 ].GetPixelType( ComponentIntensity ) ) ||
         ( lImage->GetMaximumChunkLength() != lRequiredChunkSize ) )
    {
        lImage->Alloc( GetWidth(), GetHeight(), GetPixelType(), 0, 0, lRequiredChunkSize );
    }
}


///
/// \brief Prepares a multi-part buffer for acquisition with the current configuration
///

void Source3d::SetupMultiPartIfNeeded( PvBuffer *aBuffer )
{
    // First make sure the buffer is multipart
    if ( aBuffer->GetPayloadType() != PvPayloadTypeMultiPart )
    {
        aBuffer->Reset( PvPayloadTypeMultiPart );
    }

    // Get multi-part representation of the buffer
    IPvMultiPartContainer *lMPC = aBuffer->GetMultiPartContainer();

    // Check if parts match regions as enabled/configured
    uint32_t lPartIndex = 0;
    bool lNeedSetup = false;
    for ( uint32_t i = 0; i < REGIONCOUNT; i++ )
    {
        if ( !mRegions[ i ].GetMode() )
        {
            // Region not enabled
            continue;
        }

        uint32_t lComponentCount = mRegions[ i ].GetComponentCount();
        for ( uint32_t j = 0; j < lComponentCount; j++ )
        {
            if ( mRegions[ i ].GetComponentEnable( j ) )
            {
                // Get multi-part section, check if region matches part
                IPvMultiPartSection *lMPS = lMPC->GetPart( lPartIndex );
                if ( ( lMPS == NULL ) ||
                     !mRegions[ i ].Matches( j, lMPS ) )
                {
                    // Part missing, need setup
                    lNeedSetup = true;
                    break;
                }

                lPartIndex++;
            }
        }
    }

    // If we need chunks, they have to be in the last part
    if ( mChunkModeActive && mChunkSampleEnabled )
    {
        IPvMultiPartSection *lLastSection = lMPC->GetPart( lMPC->GetPartCount() - 1 );
        if ( ( lLastSection == NULL ) ||
             ( lLastSection->GetDataType() != PvMultiPartChunkData ) )
        {
            lNeedSetup = true;
        }

        lPartIndex++;
    }

    // This checks covers additional parts from a previous configuration
    if ( lPartIndex != lMPC->GetPartCount() )
    {
        lNeedSetup = true;
    }

    // Is the buffer is already setup to our needs?
    if ( !lNeedSetup )
    {
        return;
    }

    // Setup multi-part buffer to match our regions setup
    lMPC->Reset();
    for ( uint32_t i = 0; i < REGIONCOUNT; i++ )
    {
        if ( !mRegions[ i ].GetMode() )
        {
            // Region not enabled
            continue;
        }

        mRegions[ i ].AddPartsTo( lMPC );
    }

    // Need chunks?
    if ( mChunkModeActive && mChunkSampleEnabled )
    {
#ifdef  SCAN3D_FEATURE_ENABLE
        lMPC->AddChunkPart( CHUNKSIZE, CHUNKLAYOUTID );
#else
        lMPC->AddChunkPart( mDetectedLaserPoints*sizeof(float)*sizeof(float) + sizeof(mDetectedLaserPoints), CHUNKLAYOUTID );
#endif
    }

    // With all parts defined, allocated buffer
    lMPC->AllocAllParts();
}


///
/// \brief Add chunk data to a buffer
///

void Source3d::AddChunkSample( IPvChunkData *aChunkData )
{
    // Add frame count to chunk data
#ifdef  SCAN3D_FEATURE_ENABLE
   // Add frame count to chunk data
    uint8_t lData[ 36 ] = { 0 };
    // Add frame count to chunk data
    memcpy( lData, &mFrameCount, sizeof( mFrameCount ) );
    // Add current time string to chunk data
    Time2UInt8( lData + 4, sizeof( lData ) - 4 );
    // Add chunk data to buffer
    aChunkData->ResetChunks();
    aChunkData->SetChunkLayoutID( CHUNKLAYOUTID );
    aChunkData->AddChunk( CHUNKID, lData, sizeof( lData ) );    
#else
    int i;
    aChunkData->ResetChunks();
    aChunkData->SetChunkLayoutID( CHUNKLAYOUTID );
    aChunkData->AddChunk( CHUNKID, (uint8_t *) &mDetectedLaserPoints, sizeof( mDetectedLaserPoints ) );
    aChunkData->AddChunk( CHUNKID + 1, (uint8_t *)mpLaserPosX_MM,  mDetectedLaserPoints*sizeof(float) );
    aChunkData->AddChunk( CHUNKID + 2, (uint8_t *)mpLaserPosZ_MM,  mDetectedLaserPoints*sizeof(float) );
#ifdef DEBUG
    printf("-------- detected laser points[%d]----\n",mDetectedLaserPoints);
    for (i = 0; i < mDetectedLaserPoints; i++)
    {
       printf("--[%d]---X_MM[%f] Z_MM[%f]\n",i, mpLaserPosX_MM[i], mpLaserPosZ_MM[i]);
    }
#endif
#endif
}


