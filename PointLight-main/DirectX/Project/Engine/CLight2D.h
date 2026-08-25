#pragma once
#include "Component.h"


class CLight2D :
    public Component
{
private:
    tLightInfo    m_Info;

public:
    void SetLightColor(Vec3 _Color) { m_Info.LightColor = _Color; }
    void SetAmbient(Vec3 _Ambient) { m_Info.Ambient = _Ambient; }
    void SetLightType(LIGHT_TYPE _Type) { m_Info.Type = _Type; }
    void SetAngle(float _Angle) { m_Info.Angle = _Angle; }
    void SetRadius(float _Radius) { m_Info.Radius = _Radius; }

    LIGHT_TYPE GetLightType() { return m_Info.Type; }
    Vec3 GetLightColor() { return m_Info.LightColor; }
    Vec3 GetAmbient() { return m_Info.Ambient; }
    float GetAngle() { return m_Info.Angle; }
    float GetRadius() { return m_Info.Radius; }

    const tLightInfo GetLight2DInfo() { return m_Info; }

public:
    virtual void FinalTick() override;
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(CLight2D);
public:
    CLight2D();
    virtual ~CLight2D();
};

