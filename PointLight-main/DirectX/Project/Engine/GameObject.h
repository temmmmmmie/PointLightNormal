#pragma once
#include "Entity.h"

#include "components.h"

#define GET_COMPONENT(Name, Type)  C##Name* Name() { return (C##Name*)GetComponent(COMPONENT_TYPE::Type); }

// Transform 컴포넌트는 기본적으로 가지고 있어야한다.
class GameObject :
    public Entity
{
private:
    Ptr<Component>          m_arrCom[(UINT)COMPONENT_TYPE::END];
    Ptr<CRenderComponent>   m_RenderCom;

    // 하나의 게임오브젝트가 여러개의 Script 를 가질 수도 있기 때문
    vector<Ptr<CScript>>    m_vecScript;

    // 다른 GameObject 를 자식으로 관리
    GameObject*             m_Parent;
    vector<Ptr<GameObject>> m_vecChild;

    int                     m_LayerIdx;
    bool                    m_IsDead;
public:
    void AddComponent(Component* _Component);
    Component* GetComponent(COMPONENT_TYPE _Type) { return m_arrCom[(UINT)_Type].Get(); }
    GET(Ptr<CRenderComponent>, RenderCom);

    void AddChild(Ptr<GameObject> _Child, int _NextLayer = -1);
    const vector<Ptr<GameObject>>& GetChild() { return m_vecChild; }

    Ptr<GameObject> GetParent() { return m_Parent; }

    const vector<Ptr<CScript>>& GetScripts() { return m_vecScript; }

    template<typename T>
    Ptr<T> GetScript();


    GET_COMPONENT(Transform, TRANSFORM);
    GET_COMPONENT(Camera, CAMERA);
    GET_COMPONENT(Collider2D, COLLIDER2D);
    GET_COMPONENT(Light2D, LIGHT2D);
    GET_COMPONENT(Light3D, LIGHT3D);
    GET_COMPONENT(Animator3D, ANIMATOR3D);
    GET_COMPONENT(MeshRender, MESHRENDER);
    GET_COMPONENT(SpriteRender, SPRITE_RENDER);
    GET_COMPONENT(FlipbookRender, FLIPBOOK_RENDER);
    GET_COMPONENT(TileRender, TILE_RENDER);
    GET_COMPONENT(ParticleRender, PARTICLE_RENDER);
    GET_COMPONENT(SkyBox, SKYBOX);
    GET_COMPONENT(Decal, DECAL);
    GET_COMPONENT(LandScape, LANDSCAPE);

    bool IsDead() { return m_IsDead; }
    int GetLayerIdx() { return m_LayerIdx; }
    bool IsAncestor(GameObject* _Other);


private:
    void RegisterToLayer();
    void DeregisterAsParent();

public:
    void DisconnectWithParent();
    void RegisterToLayerAsParent();
    
public:
    void Begin();       // 레벨이 시작될 때
    void Tick();        // 오브젝트가 할 일
    void FinalTick();   // 오브젝트의 컴포넌트가 할 일
    void Render();      // 오브젝트를 화면에 렌더링
    void Render_ShadowMap(Ptr<AMaterial> _ShadowMapMtrl);

    CLONE(GameObject);
public:
    GameObject();
    GameObject(const GameObject& _Origin);
    virtual ~GameObject();

    friend class Layer;
    friend class TaskMgr;
};

template<typename T>
inline Ptr<T> GameObject::GetScript()
{
    for (size_t i = 0; i < m_vecScript.size(); ++i)
    {
        if (dynamic_cast<T*>(m_vecScript[i].Get()))
            return (T*)m_vecScript[i].Get();
    }

    return nullptr;
}
