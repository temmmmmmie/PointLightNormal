#pragma once
#include "Entity.h"


#define GET_OTHER_COMPONENT(Type)  class C##Type* Type();

class GameObject;

class Component :
    public Entity
{
private:
    const COMPONENT_TYPE    m_Type;
    GameObject*             m_Owner;

public:
    GET(COMPONENT_TYPE, Type);
    GET(GameObject*, Owner);

    GET_OTHER_COMPONENT(Transform);
    GET_OTHER_COMPONENT(Light2D);
    GET_OTHER_COMPONENT(Light3D);
    GET_OTHER_COMPONENT(Animator3D);
    GET_OTHER_COMPONENT(MeshRender);
    GET_OTHER_COMPONENT(SpriteRender);
    GET_OTHER_COMPONENT(FlipbookRender);
    GET_OTHER_COMPONENT(TileRender);
    GET_OTHER_COMPONENT(ParticleRender);
    GET_OTHER_COMPONENT(Camera);
    GET_OTHER_COMPONENT(Collider2D);
    GET_OTHER_COMPONENT(SkyBox);
    GET_OTHER_COMPONENT(Decal);
    GET_OTHER_COMPONENT(LandScape);


private:
    SET(GameObject*, Owner);

public:
    virtual void Begin() {}
    virtual void FinalTick() = 0;
    virtual void SaveToLevelFile(FILE* _File) = 0;
    virtual void LoadFromLevelFile(FILE* _File) = 0;

    Component& operator =(const Component& _Other);

    virtual Component* Clone() = 0;
public:
    Component(COMPONENT_TYPE _Type);
    Component(const Component& _Origin);
    virtual ~Component();

    friend class GameObject;
};

