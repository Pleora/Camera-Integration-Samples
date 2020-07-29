// *****************************************************************************
//
// Copyright (c) 2019, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "Defines.h"
#include "Scan3dTransform.h"


///
/// \brief Constructor
///

Scan3dTransform::Scan3dTransform()
    : mRotationX( 0.0f )
    , mRotationY( 0.0f )
    , mRotationZ( 0.0f )
    , mTranslationX( 0.0f )
    , mTranslationY( 0.0f )
    , mTranslationZ( 0.0f )
{

}


///
/// \brief Destructor
///

Scan3dTransform::~Scan3dTransform()
{

}


///
/// \brief Gets a transform attribute identified by a TransformEnum
///

float Scan3dTransform::Get( TransformEnum aTransform ) const
{
    switch ( aTransform )
    {
    case TransformRotationX:
        return mRotationX;

    case TransformRotationY:
        return mRotationY;

    case TransformRotationZ:
        return mRotationZ;

    case TransformTranslationX:
        return mTranslationX;

    case TransformTranslationY:
        return mTranslationY;

    case TransformTranslationZ:
        return mTranslationZ;

    default:
        break;
    }

    return 0.0;
}


///
/// \brief Sets a transform attribute identified by a TransformEnum
///

void Scan3dTransform::Set( TransformEnum aTransform, float aValue )
{
    switch ( aTransform )
    {
    case TransformRotationX:
        mRotationX = aValue;
        break;

    case TransformRotationY:
        mRotationY = aValue;
        break;

    case TransformRotationZ:
        mRotationZ = aValue;
        break;

    case TransformTranslationX:
        mTranslationX = aValue;
        break;

    case TransformTranslationY:
        mTranslationY = aValue;
        break;

    case TransformTranslationZ:
        mTranslationZ = aValue;
        break;

    default:
        break;
    }
}

