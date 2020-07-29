// *****************************************************************************
//
// Copyright (c) 2019, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include "Enums.h"


class Scan3dTransform
{
public:

    Scan3dTransform();
    virtual ~Scan3dTransform();

    // Indexed access
    float Get( TransformEnum aTransform ) const;
    void Set( TransformEnum aTransform, float aValue );

    float GetRotationX() const { return mRotationX; }
    void SetRotationX( float aValue ) { mRotationX = aValue; }

    float GetRotationY() const { return mRotationY; }
    void SetRotationY( float aValue ) { mRotationY = aValue; }

    float GetRotationZ() const { return mRotationZ; }
    void SetRotationZ( float aValue ) { mRotationZ = aValue; }

    float GetTranslationX() const { return mTranslationX; }
    void SetTranslationX( float aValue ) { mTranslationX = aValue; }

    float GetTranslationY() const { return mTranslationY; }
    void SetTranslationY( float aValue ) { mTranslationY = aValue; }

    float GetTranslationZ() const { return mTranslationZ; }
    void SetTranslationZ( float aValue ) { mTranslationZ = aValue; }

private:

    float mRotationX;
    float mRotationY;
    float mRotationZ;

    float mTranslationX;
    float mTranslationY;
    float mTranslationZ;

};

