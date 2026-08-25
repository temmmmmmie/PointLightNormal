#pragma once

#include "AComputeShader.h"
#include "ATexture.h"

class ARaycastCS :
    public AComputeShader
{
private:
    UINT                    m_FaceX;
    UINT                    m_FaceZ;
    tRay                    m_Ray;

    Ptr<ATexture>           m_HeightMap;
    Ptr<StructuredBuffer>   m_OutBuffer;

public:
    void SetHeightMap(Ptr<ATexture> _HeightMap) { m_HeightMap = _HeightMap; }
    void SetRayInfo(const tRay& _ray) { m_Ray = _ray; }
    void SetFace(UINT _FaceX, UINT _FaceZ) { m_FaceX = _FaceX, m_FaceZ = _FaceZ; }
    void SetOutBuffer(Ptr<StructuredBuffer> _Buffer) { m_OutBuffer = _Buffer; }


public:
    virtual int Binding() override;
    virtual void CalcGroupCount() override;
    virtual void Clear() override;

public:
    ARaycastCS();
    virtual ~ARaycastCS();
};

