#pragma once
#include "EditorUI.h"

#include <Engine/Asset.h>

class AssetUI :
    public EditorUI
{
private:
    const ASSET_TYPE    m_Type;
    Ptr<Asset>          m_TargetAsset;

public:
    void SetTargetAsset(Ptr<Asset> _Asset) { m_TargetAsset = _Asset; }

    template<typename T>
    Ptr<T> GetAsset() { return (T*)m_TargetAsset.Get(); }

protected:
    void OutputAssetName();

public:
    AssetUI(ASSET_TYPE _Type);
    virtual ~AssetUI();
};

