#pragma once
#include "AComputeShader.h"

#include "ATexture.h"

class ASetColorCS :
    public AComputeShader
{
private:
    Ptr<ATexture>   m_TargetTex;
    Vec4            m_Color;

public:
    void SetTargetTexture(Ptr<ATexture> _Tex) { m_TargetTex = _Tex; }
    void SetColor(Vec4 _Color) { m_Color = _Color; }

private:
    virtual int Binding() override;
    virtual void CalcGroupCount() override;
    virtual void Clear() override;



public:
    ASetColorCS();
    virtual ~ASetColorCS();
};

