// *****************************************************************************
//
// Copyright (c) 2019, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include <stdint.h>

class Scan3dPattern
{
public:

    Scan3dPattern();
    virtual ~Scan3dPattern();

    uint32_t GetCount() const { return mCount; }
    void SetCount( uint32_t aValue ) { mCount = aValue; }

    uint32_t GetPeriod() const { return mPeriod; }
    void SetPeriod( uint32_t aValue ) { mPeriod = aValue; }

    float GetExposureTime() const { return mExposureTime; }
    void SetExposureTime( float aValue ) { mExposureTime = aValue; }

private:
    
    uint32_t mCount;
    uint32_t mPeriod;
    float mExposureTime;

};

