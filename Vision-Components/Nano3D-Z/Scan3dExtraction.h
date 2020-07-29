// *****************************************************************************
//
// Copyright (c) 2019, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include "Enums.h"
#include "Scan3dCoordinate.h"
#include "Scan3dTransform.h"

#include <vector>

#include <stdint.h>


class Scan3dExtraction
{
public:

    Scan3dExtraction();
    virtual ~Scan3dExtraction();

    RegionEnum GetSource() const { return mSource; }
    void SetSource( RegionEnum aValue ) { mSource = aValue; }

    MethodEnum GetMethod() const { return mMethod; }
    void SetMethod( MethodEnum aValue ) { mMethod = aValue; }

    CoordinateSystemEnum GetCoordinateSystem() const { return mCoordinateSystem; }
    void SetCoordinateSystem( CoordinateSystemEnum aValue ) { mCoordinateSystem = aValue; }

    OutputModeEnum GetOutputMode() const { return mOutputMode; }
    void SetOutputMode( OutputModeEnum aValue ) { mOutputMode = aValue; }

    DistanceUnitEnum GetDistanceUnit() const { return mDistanceUnit; }
    void SetDistanceUnit( DistanceUnitEnum aValue ) { mDistanceUnit = aValue; }

    uint32_t GetPatternContrast( PatternEnum aPattern ) { return mPatternContrast[ aPattern ]; }
    void SetPatternContrast( PatternEnum aPattern, uint32_t aValue ) { mPatternContrast[ aPattern ] = aValue; }

    Scan3dCoordinate &Coordinate( CoordinateEnum aCoordinate ) { return mCoordinate[ aCoordinate ]; }
    Scan3dTransform &Transform() { return mTransform; }

    CoordinateSystemReferenceEnum GetCoordinateSystemReference() const { return mCoordinateSystemReference; }
    void SetCoordinateSystemReference( CoordinateSystemReferenceEnum aValue ) { mCoordinateSystemReference = aValue; }

private:

    RegionEnum mSource;
    MethodEnum mMethod;
    CoordinateSystemEnum mCoordinateSystem;
    OutputModeEnum mOutputMode;
    DistanceUnitEnum mDistanceUnit;
    uint32_t mPatternContrast[ PATTERNCOUNT ];
    Scan3dCoordinate mCoordinate[ COORDINATECOUNT ];
    Scan3dTransform mTransform;
    CoordinateSystemReferenceEnum mCoordinateSystemReference;
};


typedef std::vector<Scan3dExtraction> Scan3dExtractionVector;

