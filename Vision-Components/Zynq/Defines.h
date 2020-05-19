// *****************************************************************************RCODEREADERNUMADDR
//
// Copyright (c) 2018, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include <stdlib.h>
#include <iostream>


#define BUFFERCOUNT ( 16 )
#define DEFAULT_FPS ( 30 )

#define WIDTH_MIN ( 64 )
#define WIDTH_MAX ( 1920 )
#define WIDTH_DEFAULT ( 960 )
#define WIDTH_INC ( 4 )

#define HEIGHT_MIN ( 4 )
#define HEIGHT_MAX ( 1080 )
#define HEIGHT_DEFAULT (720 )
#define HEIGHT_INC ( 1 )

// Custom parameters defines
#define FEATURECATEGORY "FeatureCategory"

#define BARCODEREADERADDR ( 0x10000000 )
#define BARCODEREADERNAME "BarcodeReader"
#define BARCODEREADERDESCRIPTION "BarcodeReader feature enable/disable."
#define BARCODEREADERTOOLTIP "Boolean."

//
// Chunk defines
//

#define CHUNKID ( 0x4001 )
#define CHUNKLAYOUTID ( 0x12345678 )
#define CHUNKSIZE ( 64 )

#define CHUNKCATEGORY "ChunkDataControl"

#define CHUNKCOUNTNAME "ChunkSampleCount"
#define CHUNKCOUNTDESCRIPTION "Counter keeping track of images with chunks generated."
#define CHUNKCOUNTTOOLTIP "Chunk count."

#define CHUNKTIMENAME "ChunkSampleTime"
#define CHUNKTIMEDESCRIPTION "String representation of the time when the chunk data was generated."
#define CHUNKTIMETOOLTIP "Chunk time."


//
// Event defines
//

#define EVENTID ( 0x9006 )
#define EVENTDATAID ( 0x9005 )

#define EVENTCATEGORY "EventControl\\EventSample"

#define EVENTCOUNTNAME "EventSampleCount"
#define EVENTCOUNTDESCRIPTION "Counter keeping track of events generated."
#define EVENTCOUNTTOOLTIP "Event count."

#define EVENTTIMENAME "EventSampleTime"
#define EVENTTIMEDESCRIPTION "String representation of the time when the event was generated."
#define EVENTTIMETOOLTIP "Event time."


