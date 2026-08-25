#pragma once
#include "Asset.h"

#include "Layer.h"

class ALevel :
    public Asset
{
private:
    Ptr<Layer>      m_arrLayer[MAX_LAYER];
    LEVEL_STATE     m_CurState;

public:
    void Begin();
    void Tick();
    void FinalTick();

public:
    void AddObject(int _LayerIdx, GameObject* _Object, bool _MoveWithChild = true);
    Ptr<Layer> GetLayer(int _LayerIdx) { return m_arrLayer[_LayerIdx]; }
    LEVEL_STATE GetState() { return m_CurState; }
    Ptr<GameObject> FindObjectByName(const wstring& _Name);

    void Deregister();

    virtual int Save(const wstring& _FilePath) override { return S_OK; };
    virtual int Load(const wstring& _FilePath) override { return S_OK; };

private:
    bool ChangeLevelState(LEVEL_STATE _NextState);

public:
    CLONE(ALevel);

public:
    ALevel();    
    ALevel(const ALevel& _Origin);
    virtual ~ALevel();

    friend class LevelMgr;
    friend class TaskMgr;
};

