#pragma once
#include "Asset.h"

#include "GameObject.h"



class APrefab :
    public Asset
{
private:
    Ptr<GameObject> m_ProtoObj;

public:
    GameObject* Instantiate();
    void SetGameObject(Ptr<GameObject> _Object);

public:
    virtual int Save(const wstring& _FilePath) override;
    virtual int Load(const wstring& _FilePath) override;

    CLONE_DISABLE(AMesh);
public:
    APrefab(bool _EngineAsset = false);
    virtual ~APrefab();
};

