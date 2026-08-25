#pragma once
#include "Component.h"

#include "MRT.h"

class CLight3D :
    public Component
{
private:
    tLightInfo      m_Info;
    int             m_LightIdx;

    Ptr<AMesh>      m_VolumeMesh;
    Ptr<AMaterial>  m_LightMtrl;

    // 광원이 사용할 카메라
    Ptr<GameObject> m_CamObject;
    Ptr<MRT>        m_ShadowMapMRT;
    Ptr<MRT>        m_ShadowCubeMapMRT[6];

public:
    LIGHT_TYPE GetLightType() { return m_Info.Type; }
    Vec3 GetLightColor() { return m_Info.LightColor; }
    Vec3 GetAmbient() { return m_Info.Ambient; }
    float GetAngle() { return m_Info.Angle; }
    float GetRadius() { return m_Info.Radius; }

    void SetLightColor(Vec3 _Color) { m_Info.LightColor = _Color; }
    void SetAmbient(Vec3 _Ambient) { m_Info.Ambient = _Ambient; }
    void SetLightType(LIGHT_TYPE _Type);
    void SetAngle(float _Angle) { m_Info.Angle = _Angle; }
    void SetRadius(float _Radius);

    const tLightInfo GetLight3DInfo() { return m_Info; }

private:
    void Render_ShadowMap();
    void SetMat(Matrix* _mat);
    Matrix Calmat(Vec3 vR, Vec3 vU, Vec3 vF);

public:
    virtual void FinalTick() override;
    void Render();
    virtual void SaveToLevelFile(FILE* _File);
    virtual void LoadFromLevelFile(FILE* _File);
    void SaveShadowMap();

public:
    CLONE(CLight3D);
    CLight3D();
    CLight3D(const CLight3D& _Other);
    virtual ~CLight3D();
};

