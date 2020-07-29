// *****************************************************************************
//
// Copyright (c) 2019, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "Defines.h"
#include "Scan3dExtraction.h"


///
/// \brief Constructor
///

Scan3dExtraction::Scan3dExtraction()
    : mSource( Region0 )
    , mMethod( MethodDefault )
    , mCoordinateSystem( CoordinateSystemCartesian )
    , mOutputMode( OutputModeUncalibratedC )
    , mDistanceUnit( DistanceUnitMillimeter )
    , mCoordinateSystemReference( CoordinateSystemReferenceAnchored )
{
    for ( int i = 0; i < PATTERNCOUNT; i++ )
    {
        mPatternContrast[ i ] = 0;
    }
}


///
/// \brief Destructor
///

Scan3dExtraction::~Scan3dExtraction()
{

}

