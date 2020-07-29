// *****************************************************************************
//
// Copyright (c) 2019, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include <string>


typedef enum
{
    RegionInvalid = -1,
    Region0 = 0,
    Region1,
    RegionScan3dExtraction0,
    RegionScan3dExtraction1

} RegionEnum;

typedef enum
{
    Scan3dExtractionInvalid = -1,
    Scan3dExtraction0 = 0,
    Scan3dExtraction1

} Scan3dExtractionEnum;

typedef enum
{
    MethodInvalid = -1,
    MethodDefault = 0,
    MethodFiltered,
    MethodBinning3d,
    MethodHighSpeed,
    MethodHighResolution

} MethodEnum;

typedef enum
{
    CoordinateSystemInvalid = -1,
    CoordinateSystemCartesian = 0,
    CoordinateSystemSpherical,
    CoordinateSystemCylindrical

} CoordinateSystemEnum;

typedef enum
{
    OutputModeInvalid = -1,
    OutputModeUncalibratedC = 0,
    OutputModeCalibratedABC_Grid,
    OutputModeCalibratedABC_PointCloud,
    OutputModeCalibratedAC,
    OutputModeCalibratedAC_Linescan,
    OutputModeCalibratedC,
    OutputModeCalibratedC_Linescan,
    OutputModeRectifiedC,
    OutputModeRectifiedC_Linescan,
    OutputModeDisparityC

} OutputModeEnum;

typedef enum
{
    DistanceUnitInvalid = -1,
    DistanceUnitMillimeter = 0,
    DistanceUnitInch

} DistanceUnitEnum;


typedef enum
{
    CoordinateSystemReferenceInvalid = -1,
    CoordinateSystemReferenceAnchored = 0,
    CoordinateSystemReferenceTransformed

} CoordinateSystemReferenceEnum;

typedef enum
{
    PatternInvalid = -1,
    PatternGraycode = 0,
    PatternSine

} PatternEnum;

typedef enum
{
    CoordinateInvalid = -1,
    CoordinateA = 0,
    CoordinateB,
    CoordinateC

} CoordinateEnum;


typedef enum
{
    TransformInvalid = -1,
    TransformRotationX = 0,
    TransformRotationY,
    TransformRotationZ,
    TransformTranslationX,
    TransformTranslationY,
    TransformTranslationZ

} TransformEnum;


typedef enum
{
    ComponentInvalid = -1,
    ComponentIntensity = 0,
    ComponentInfrared,
    ComponentUltraviolet,
    ComponentRange,
    ComponentReflectance,
    ComponentConfidence,
    ComponentDisparity,
    ComponentScatter

} ComponentEnum;


std::string Scan3dExtractionToString( Scan3dExtractionEnum aValue );
std::string RegionToString( RegionEnum aValue );
std::string MethodToString( MethodEnum aValue );
std::string CoordinateSystemToString( CoordinateSystemEnum aValue );
std::string OutputModeToString( OutputModeEnum aValue );
std::string DistanceUnitToString( DistanceUnitEnum aValue );
std::string CoordinateSystemReferenceToString( CoordinateSystemReferenceEnum aValue );
std::string PatternToString( PatternEnum aValue );
std::string CoordinateToString( CoordinateEnum aValue );
std::string TransformToString( TransformEnum aValue );
std::string ComponentToString( ComponentEnum aValue );

