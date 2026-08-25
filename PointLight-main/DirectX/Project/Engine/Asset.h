#pragma once
#include "Entity.h"


class Asset :
    public Entity
{
private:
    const ASSET_TYPE    m_Type;
    wstring             m_Key;
    wstring             m_RelativePath;
    bool                m_EngineAsset;

protected:
    SET(wstring, Key)
    SET(wstring, RelativePath)

public:
    GET_REF(wstring, Key);
    GET_REF(wstring, RelativePath);
    GET(ASSET_TYPE, Type);

    bool IsEngineAsset() { return m_EngineAsset; }

protected:
    bool CheckFilePath(const wstring& _FilePath);
    void SaveRelativePath(const wstring& _FilePath);

public:
    virtual int Save(const wstring& _FilePath) = 0;

private:
    virtual int Load(const wstring& _FilePath) = 0;

public:
    virtual Asset* Clone() = 0;
public:
    Asset(ASSET_TYPE _Type, bool _EngineAsset = false);
    Asset(const Asset& _Origin);
    virtual ~Asset();

    friend class AssetMgr;
    friend class FBXLoader;
};

