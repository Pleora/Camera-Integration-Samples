// *****************************************************************************
//
// Copyright (c) 2019, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once


class Scan3dCoordinate
{
public:

    Scan3dCoordinate();
    virtual ~Scan3dCoordinate();

    float GetScale() const { return mScale; }
    void SetScale( float aValue ) { mScale = aValue; }

    float GetOffset() const { return mOffset; }
    void SetOffset( float aValue ) { mOffset = aValue; }

private:

    float mScale;
    float mOffset;

};

