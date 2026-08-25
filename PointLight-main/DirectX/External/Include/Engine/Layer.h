#pragma once
#include "Entity.h"

#include "GameObject.h"

class Layer :
    public Entity
{
private:
    vector<Ptr<GameObject>>     m_vecParents; // Layer 소속되어있는 GameObject 중에서도 최상위 부모 오브젝트만 따로 관리
    vector<Ptr<GameObject>>     m_vecObjects; // Layer 소속되어있는 모든 오브젝트
    int                         m_LayerIdx;


public:
    void AddObject(GameObject* _Object, bool _MoveWithChild);
    const vector<Ptr<GameObject>>& GetParentObjects() { return m_vecParents; }
    const vector<Ptr<GameObject>>& GetObjects() { return m_vecObjects; }

    void RegisterObject(GameObject* _Object) { m_vecObjects.push_back(_Object); }
    void Deregister() { m_vecObjects.clear(); }

    void RegisterObjectAsParent(GameObject* _Object) { m_vecParents.push_back(_Object); }

    void DeregisterAsParent(GameObject* _Target);

public:
    void Begin();
    void Tick();
    void FinalTick();
    

public:
    CLONE(Layer);

public:
    Layer(int _Idx);
    Layer(const Layer& _Origin);
    virtual ~Layer();

    friend class ALevel;
};

