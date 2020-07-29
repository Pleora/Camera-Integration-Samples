// *****************************************************************************
//
// Copyright (c) 2019, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "Enums.h"


///
/// \brief Converts a RegionEnum to string
///

std::string RegionToString( RegionEnum aValue )
{
    switch ( aValue )
    {
    case Region0:
        return "Region0";

    case Region1:
        return "Region1";

    case RegionScan3dExtraction0:
        return "Scan3dExtraction0";

    case RegionScan3dExtraction1:
        return "Scan3dExtraction1";

    default:
        break;
    }

    return "Unknown";
}


///
/// \brief Converts a Scan3dExtractionEnum to string
///

std::string Scan3dExtractionToString( Scan3dExtractionEnum aValue )
{
    switch ( aValue )
    {
    case Scan3dExtraction0:
        return "Scan3dExtraction0";

    case Scan3dExtraction1:
        return "Scan3dExtraction1";

    default:
        break;
    }

    return "Unknown";
}


///
/// \brief Converts a MethodEnum to string
///

std::string MethodToString( MethodEnum aValue )
{
    switch ( aValue )
    {
    case MethodDefault:
        return "Default";

    case MethodFiltered:
        return "Filtered";

    case MethodBinning3d:
        return "Binning3d";

    case MethodHighSpeed:
        return "HighSpeed";

    case MethodHighResolution:
        return "HighResolution";

    default:
        break;
    }

    return "Unknown";
}


///
/// \brief Converts a CoordinateSystemEnum to string
///

std::string CoordinateSystemToString( CoordinateSystemEnum aValue )
{
    switch ( aValue )
    {
    case CoordinateSystemCartesian:
        return "Cartesian";

    case CoordinateSystemSpherical:
        return "Spherical";

    case CoordinateSystemCylindrical:
        return "Cylindrical";

    default:
        break;
    }

    return "Unknown";
}


///
/// \brief Converts an OutputModeEnum to string
///

std::string OutputModeToString( OutputModeEnum aValue )
{
    switch ( aValue )
    {
    case OutputModeUncalibratedC:
        return "UncalibratedC";

    case OutputModeCalibratedABC_Grid:
        return "CalibratedABC_Grid";

    case OutputModeCalibratedABC_PointCloud:
        return "CalibratedABC_PointCloud";

    case OutputModeCalibratedAC:
        return "UncalibratedAC";

    case OutputModeCalibratedAC_Linescan:
        return "CalibratedAC_Linescan";

    case OutputModeCalibratedC:
        return "CalibratedC";

    case OutputModeCalibratedC_Linescan:
        return "CalibratedC_Linescan";

    case OutputModeRectifiedC:
        return "RectifiedC";

    case OutputModeRectifiedC_Linescan:
        return "RectifiedC_Linescan";

    case OutputModeDisparityC:
        return "DisparityC";

    default:
        break;
    }

    return "Unknown";
}


///
/// \brief Converts a DistanceUnitEnum to string
///

std::string DistanceUnitToString( DistanceUnitEnum aValue )
{
    switch ( aValue )
    {
    case DistanceUnitMillimeter:
        return "Millimeter";

    case DistanceUnitInch:
        return "Inch";

    default:
        break;
    }

    return "Unknown";
}


///
/// \brief Converts a CoordinateSystemReferenceEnum to string
///

std::string CoordinateSystemReferenceToString( CoordinateSystemReferenceEnum aValue )
{
    switch ( aValue )
    {
    case CoordinateSystemReferenceAnchored:
        return "Anchored";

    case CoordinateSystemReferenceTransformed:
        return "Transformed";

    default:
        break;
    }

    return "Unknown";
}


///
/// \brief Converts a PatternEnum to string
///

std::string PatternToString( PatternEnum aValue )
{
    switch ( aValue )
    {
    case PatternGraycode:
        return "Graycode";

    case PatternSine:
        return "Sine";

    default:
        break;
    }

    return "Unknown";
}


///
/// \brief Converts a CoordinateEnum to string
///

std::string CoordinateToString( CoordinateEnum aValue )
{
    switch ( aValue )
    {
    case CoordinateA:
        return "CoordinateA";

    case CoordinateB:
        return "CoordinateB";

    case CoordinateC:
        return "CoordinateC";

    default:
        break;
    }

    return "Unknown";
}


///
/// \brief Converts a TransformEnum to string
///

std::string TransformToString( TransformEnum aValue )
{
    switch ( aValue )
    {
    case TransformRotationX:
        return "RotationX";

    case TransformRotationY:
        return "RotationY";

    case TransformRotationZ:
        return "RotationZ";

    case TransformTranslationX:
        return "TranslationX";

    case TransformTranslationY:
        return "TranslationY";

    case TransformTranslationZ:
        return "TranslationZ";

    default:
        break;
    }

    return "Unknown";
}


///
/// \brief Converts a ComponentEnum to string
///

std::string ComponentToString( ComponentEnum aValue )
{
    switch ( aValue )
    {
    case ComponentIntensity:
        return "Intensity";

    case ComponentInfrared:
        return "Infrared";

    case ComponentUltraviolet:
        return "Ultraviolet";

    case ComponentRange:
        return "Range";

    case ComponentReflectance:
        return "Reflectance";

    case ComponentConfidence:
        return "Confidence";

    case ComponentDisparity:
        return "Disparity";

    case ComponentScatter:
        return "Scatter";

    default:
        break;
    }

    return "Unknown";
}

