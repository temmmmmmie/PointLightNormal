#pragma once
#include "EditorUI.h"
#include <Engine/GameObject.h>
#include <Engine/Asset.h>

class Inspector :
    public EditorUI
{
private:
    Ptr<GameObject>         m_TargetObject;
    Ptr<Asset>              m_TargetAsset;

    class ComponentUI*      m_arrComUI[(UINT)COMPONENT_TYPE::END];
    vector<class ScriptUI*> m_vecScriptUI;

    class AssetUI*          m_arrAssetUI[(UINT)ASSET_TYPE::END];

public:
    virtual void Tick_UI() override;

public:
    void SetTargetObject(Ptr<GameObject> _TargetObject);
    Ptr<GameObject> GetTargetObject() { return m_TargetObject; }

    void SetTargetAsset(Ptr<Asset> _TargetAsset);

public:
    Inspector();
    virtual ~Inspector();
};

