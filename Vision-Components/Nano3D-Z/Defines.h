// *****************************************************************************
//
// Copyright (c) 2018, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include <stdlib.h>
#include <iostream>
#include <string>
#include <stdint.h>


#define BUFFERCOUNT ( 16 )
#define DEFAULT_FPS ( 30 )

#define WIDTH_MIN ( 64 )
#define WIDTH_MAX ( 1408 )
#define WIDTH_DEFAULT ( 1408 )
#define WIDTH_INC ( 4 )

#define HEIGHT_MIN ( 4 )
#define HEIGHT_MAX ( 1080 )
#define HEIGHT_DEFAULT ( 1080 )
#define HEIGHT_INC ( 1 )

#define SAMPLEINTEGER_ADDR ( 0x10000000 )
#define SAMPLEFLOAT_ADDR ( 0x10000020 )
#define SAMPLESTRING_ADDR ( 0x10000030 )
#define SAMPLEBOOLEAN_ADDR ( 0x10000040 )
#define SAMPLECOMMAND_ADDR ( 0x10000050 )
#define SAMPLEENUM_ADDR ( 0x10000060 )

#define EVENTID ( 0x9006 )
#define EVENTDATAID ( 0x9005 )
#define CHUNKID ( 0x4001 )
#define CHUNKLAYOUTID ( 0x12345678 )
#define CHUNKSIZE ( 64 )

#define DESCRIPTION ( "This is where you put the description of the custom feature." )
#define TOOLTIP ( "This is where you put the tooltip of the custom feature." )

#define SAMPLECATEGORY ( "SampleCategory" )
#define SAMPLEINTEGERNAME ( "SampleInteger" )
#define SAMPLEFLOATNAME ( "SampleFloat" )
#define SAMPLESTRINGNAME ( "SampleString" )
#define SAMPLEBOOLEANNAME ( "SampleBoolean" )
#define SAMPLECOMMANDNAME ( "SampleCommand" )
#define SAMPLEENUMNAME ( "SampleEnum" )

#define EVENTCATEGORY ( "EventControl\\EventSample" )
#define EVENTCOUNTNAME ( "EventSampleCount" )
#define EVENTTIMENAME ( "EventSampleTime" )

#define CHUNKCATEGORY ( "ChunkDataControl" )
#define CHUNKCOUNTNAME ( "ChunkSampleCount" )
#define CHUNKTIMENAME ( "ChunkSampleTime" )

#define REGIONCOUNT ( 4 )
#define EXTRACTIONCOUNT ( 2 )
#define PATTERNCOUNT ( 2 )
#define COORDINATECOUNT ( 3 )
#define COMPONENTCOUNT ( 8 )

// The register addresses of source registers are indedex to ensure unique addresses
// if a source is instantiated more than once
#define BASE_ADDR 0x20000000
#define REGIONSELECTOR_ADDR ( BASE_ADDR + 0x0000 )
#define REGIONMODE_ADDR ( BASE_ADDR + 0x0004 )
#define SCAN3DPATTERNSELECTOR_ADDR ( BASE_ADDR + 0x0008 )
#define SCAN3DPATTERNCOUNT_ADDR ( BASE_ADDR + 0x000C )
#define SCAN3DPATTERNPERIOD_ADDR ( BASE_ADDR + 0x0010 )
#define SCAN3DEXTRACTIONSELECTOR_ADDR ( BASE_ADDR + 0x0014 )
#define SCAN3DEXTRACTIONSOURCE_ADDR ( BASE_ADDR + 0x0018 )
#define SCAN3DEXTRACTIONMETHOD_ADDR ( BASE_ADDR + 0x001C )
#define SCAN3DCOORDINATESYSTEM_ADDR ( BASE_ADDR + 0x0020 )
#define SCAN3DOUTPUTMODE_ADDR ( BASE_ADDR + 0x0024 )
#define SCAN3DDISTANCEUNIT_ADDR ( BASE_ADDR + 0x0028 )
#define SCAN3DEXTRACTIONPATTERNCONTRAST_ADDR ( BASE_ADDR + 0x002C )
#define SCAN3DCOORDINATESELECTOR_ADDR ( BASE_ADDR + 0x0030 )
#define SCAN3DCOORDINATESCALE_ADDR ( BASE_ADDR + 0x0034 )
#define SCAN3DCOORDINATEOFFSET_ADDR ( BASE_ADDR + 0x003C )
#define SCAN3DCOORDINATETRANSFORMSELECTOR_ADDR ( BASE_ADDR + 0x0040 )
#define SCAN3DTRANSFORMVALUE_ADDR ( BASE_ADDR + 0x0044 )
#define SCAN3DCOORDINATESYSTEMREFERENCE_ADDR ( BASE_ADDR + 0x0048 )
#define COMPONENTSELECTOR_ADDR ( BASE_ADDR + 0x004C )
#define COMPONENTENABLE_ADDR ( BASE_ADDR + 0x0050 )
#define EXPOSURETIMESELECTOR_ADDR ( BASE_ADDR + 0x0054 )
#define EXPOSURETIME_ADDR ( BASE_ADDR + 0x0058 )

// Addresses of image control attributes registers (defined by PvSoftDeviceGEV)
// 'a' is the streaming channel
#define GEV_REG_VGEVD_WIDTH( a ) ( 0x3FFE0010 + ( 0x80 * a ) )
#define GEV_REG_VGEVD_HEIGHT( a ) ( 0x3FFE0014 + ( 0x80 * a ) )
#define GEV_REG_VGEVD_PIXELFORMAT( a ) ( 0x3FFE0018 + ( 0x80 * a) )
#define GEV_REG_VGEVD_OFFSETX( a ) ( 0x3FFE0020 + ( 0x80 * a ) )
#define GEV_REG_VGEVD_OFFSETY( a ) ( 0x3FFE0024 + ( 0x80 * a ) )

//
// Addresses of the camera registers
//
#define CAMERA_BASE  0x10050000
#define LASERMODE_ADDR    ( CAMERA_BASE + 0x0000 )
#define EXPOSUREMODE_ADDR ( CAMERA_BASE + 0x0004) 
#define SHUTTER_ADDR      ( CAMERA_BASE + 0x0008) 
#define GAIN_ADDR         ( CAMERA_BASE + 0x000C) 
