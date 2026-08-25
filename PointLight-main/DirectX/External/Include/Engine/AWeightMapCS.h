#pragma once

#include "StructuredBuffer.h"
#include "AComputeShader.h"
#include "ATexture.h"

class AWeightMapCS :
    public AComputeShader
{
private:
    Ptr<StructuredBuffer>   m_WeightMap;
    Ptr<ATexture>           m_BrushTex;
    Ptr<StructuredBuffer>   m_RaycastOut;
    Vec2                    m_BrushScale;
    int                     m_WeightIdx;

    int                     m_WeightMapWidth;
    int                     m_WeightMapHeight;

public:
    void SetBrushPos(Ptr<StructuredBuffer> _Buffer) { m_RaycastOut = _Buffer; }
    void SetBrushScale(Vec2 _Scale) { m_BrushScale = _Scale; }
    void SetWeightIdx(int _Idx) { m_WeightIdx = _Idx; }
    void SetWeightMapWidthHeight(UINT _Width, UINT _Height) { m_WeightMapWidth = (int)_Width; m_WeightMapHeight = (int)_Height; }
    void SetWeightMap(Ptr<StructuredBuffer> _WeightMap) { m_WeightMap = _WeightMap; }
    void SetBrushTex(Ptr<ATexture> _BrushTex) { m_BrushTex = _BrushTex; }

public:
    virtual int Binding() override;
    virtual void CalcGroupCount() override;
    virtual void Clear() override;

public:
    AWeightMapCS();
    virtual ~AWeightMapCS();
};

