#pragma once
#include "Entity.h"

enum FACE_TYPE
{
    FT_NEAR,
    FT_FAR,    
    FT_TOP,
    FT_BOT,
    FT_LEFT,
    FT_RIGHT,

    FT_END,
};

class CCamera;

class Frustum :
    public Entity
{
private:
    CCamera*    m_Owner;
    Vec3        m_ProjPos[8];
    Vec4        m_WorldFace[FT_END];

public:
    bool FrustumPointCheck(Vec3 _Point);
    bool FrustumSphereCheck(Vec3 _Point, float _Radius);

public:
    void FinalTick();

    CLONE(Frustum);
public:
    Frustum(CCamera* _Owner);
    virtual ~Frustum();
};

