#pragma once
#include "AComputeShader.h"

#include "ATexture.h"
#include "StructuredBuffer.h"

class AHeightMapCS :
    public AComputeShader
{
private:
    Ptr<ATexture>           m_HeightMap;
    Ptr<ATexture>           m_BrushTex;
    Vec2                    m_BrushScale;
    Ptr<StructuredBuffer>   m_RaycastOut;

public:
    void SetHeightMap(Ptr<ATexture> _Texture) { m_HeightMap = _Texture; }
    void SetBrushPos(Ptr<StructuredBuffer> _Buffer) { m_RaycastOut = _Buffer; }
    void SetBrushTex(Ptr<ATexture> _Tex) { m_BrushTex = _Tex; }
    void SetBrushScale(Vec2 _Scale) { m_BrushScale = _Scale; }

public:
    virtual int Binding() override;
    virtual void CalcGroupCount() override;
    virtual void Clear() override;

public:
    AHeightMapCS();
    virtual ~AHeightMapCS();
};