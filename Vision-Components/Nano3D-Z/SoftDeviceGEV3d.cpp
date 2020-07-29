// *****************************************************************************
//
// Copyright (c) 2018, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "Defines.h"

#include <PvSampleUtils.h>
#include <PvSoftDeviceGEV.h>
#include <PvBuffer.h>
#include <PvFPSStabilizer.h>
#include <PvSampleTransmitterConfig.h>

#include "Utilities.h"
#include "Source3d.h"
#include "MyEventSink.h"
#include "MyRegisterEventSink.h"
#ifdef SCAN3D_FEATURE_ENABLE
#include "SourceLeft.h"
#include "SourceRight.h"
#endif
#include <fstream>


PV_INIT_SIGNAL_HANDLER();

#define USERSET_COUNT ( 2 )


int main( int aCount, const char **aArgs )
{
    PVUNREFPARAM( aCount );
    PVUNREFPARAM( aArgs );

    // Select interface if MAC was not provided
    PvString lInterface = ( aCount > 1 ) ? PvString( aArgs[1] ) : PvSelectInterface();
    if ( lInterface.GetLength() == 0 )
    {
        std::cout << "No interface selected, terminating" << std::endl;
        return -1;
    }


   // Instantiate interface implementations
#ifdef SCAN3D_FEATURE_ENABLE
    SourceLeft lSourceLeft( 0 );
    SourceRight lSourceRight( 1 );
    Source3d lSource3d( 2 );
#else
    Source3d lSource3d( 0 );
#endif
    // Instantiate interface implementations
    MyRegisterEventSink lRegisterEventSink;
    MyEventSink lEventSink( &lRegisterEventSink );

    // Instantiate the device itself
    PvSoftDeviceGEV lDevice;

    // Set device identify
    IPvSoftDeviceGEVInfo *lInfo = lDevice.GetInfo();
    lInfo->SetModelName( "SoftDeviceGEV3d_VC-3D-Nano" );
    lInfo->SetGenICamXMLVersion( 1, 2, 3 );
    lInfo->SetGenICamXMLGUIDs( "BA07A10F-969E-4900-9B11-EE914F7A5D7F", "F87E8639-DE12-404E-A079-BBFF5FC9D82AA" );

    // Add stream, register event sink

#ifdef SCAN3D_FEATURE_ENABLE
    lDevice.AddStream( &lSourceLeft );
    lDevice.AddStream( &lSourceRight );
    lDevice.AddStream( &lSource3d );
#else
    lDevice.AddStream( &lSource3d );
#endif
    lDevice.RegisterEventSink( &lEventSink );

    // Configure user-set count
    lDevice.SetUserSetCount( USERSET_COUNT );

    // Start device
    const std::string lModelName( lInfo->GetModelName().GetAscii() );
    PvResult lResult = lDevice.Start( lInterface );
    if ( !lResult.IsOK() )
    {
        std::cout << "Error starting " << lModelName << std::endl;
        if ( lResult.GetCode() == PvResult::Code::GENICAM_XML_ERROR )
        {
            std::cout << "The error is possibly in the dynamically generated GenICam XML file:" << std::endl;
            std::cout << lResult.GetDescription().GetAscii() << std::endl;

            PvString lXML;
            lDevice.GetGenICamXMLFile( lXML );
            std::cout << lXML.GetAscii() << std::endl;
        }

        return -1;
    }

    std::cout << lModelName << " started" << std::endl;
    
    // Loop until keyboard hit
    PvFlushKeyboard();
    while ( !PvKbHit() )
    {
        FireTestEvents( lDevice.GetMessagingChannel() );
        PvSleepMs( 100 );
    }

    // Stop device
    lDevice.Stop();
    std::cout << lModelName << " stopped" << std::endl;

    return 0;
}


