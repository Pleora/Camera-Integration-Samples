// *****************************************************************************
//
// Copyright (c) 2019, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include "Enums.h"

#include <PvMultiPartContainer.h>

#include <map>
#include <vector>


struct Component
{
    Component()
        : PixelType( PvPixelUndefined )
        , Enabled( true )
    {
    }

    PvPixelType PixelType;
    bool Enabled;
};

typedef std::map<ComponentEnum, Component> ComponentMap;
typedef std::vector<ComponentEnum> ComponentVector;


class Region
{
public:

    Region();
    virtual ~Region();

    void AddComponent( ComponentEnum aComponent, PvPixelType aDefaultPixelType );
    void SetIDs( uint32_t aRegionID, uint32_t aSourceID );

    uint32_t GetComponentCount() const { return static_cast<uint32_t>( mComponentValues.size() ); }
    bool GetComponentEnable( uint32_t aIndex) { return mComponents[ mComponentValues[ aIndex ] ].Enabled; }
    bool GetComponentEnable( ComponentEnum aComponent );
    void SetComponentEnable( ComponentEnum aComponent, bool aValue );
    bool HasComponent( ComponentEnum aValue ) { return mComponents.find( aValue ) != mComponents.end(); }

    bool GetMode() const { return mMode; }
    void SetMode( bool aValue ) { mMode = aValue; }

    uint32_t GetWidth() const { return mWidth; }
    void SetWidth( uint32_t aValue ) { mWidth = aValue; }

    uint32_t GetHeight() const { return mHeight; }
    void SetHeight( uint32_t aValue ) { mHeight = aValue; }

    uint32_t GetOffsetX() const { return mOffsetX; }
    void SetOffsetX( uint32_t aValue ) { mOffsetX = aValue; }

    uint32_t GetOffsetY() const { return mOffsetY; }
    void SetOffsetY( uint32_t aValue ) { mOffsetY = aValue; }

    PvPixelType GetPixelType( ComponentEnum aComponent ) const;
    void SetPixelType( ComponentEnum aComponent, PvPixelType aValue );

    bool Matches( uint32_t aComponentIndex, IPvMultiPartSection *aMPS );
    void AddPartsTo( IPvMultiPartContainer *aMPC );
    void Fill( uint32_t aComponentIndex, IPvMultiPartSection *aMPS );
    void Fill( IPvImage *aImage );
    uint32_t GetPayloadSize() const;

private:

    bool mMode;
    PvMultiPartDataType mDataType;
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mOffsetX;
    uint32_t mOffsetY;
    ComponentMap mComponents;
    ComponentVector mComponentValues;

    uint32_t mSourceID;
    uint32_t mRegionID;
};

