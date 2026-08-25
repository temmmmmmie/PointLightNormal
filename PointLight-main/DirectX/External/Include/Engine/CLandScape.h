#pragma once
#include "CRenderComponent.h"
#include "StructuredBuffer.h"

#include "ARaycastCS.h"
#include "AHeightMapCS.h"

struct tRaycastOut
{
    Vec2    Location;
    UINT    Distance;
    int     Success;
};


class CLandScape :
    public CRenderComponent
{
private:
    UINT                    m_FaceX;
    UINT                    m_FaceZ;

    Vec2                    m_BrushScale;
    Ptr<ATexture>           m_BrushTex;

    // Raycasting
    Ptr<ARaycastCS>         m_RaycastCS;
    Ptr<StructuredBuffer>   m_RaycastOut;   // 마우스 픽킹 위치
    tRaycastOut             m_Out;          // 마우스 픽킹 위치

    // HeightMap
    Ptr<ATexture>           m_HeightMap;
    Ptr<AHeightMapCS>       m_HeightMapCS;  // 높이맵 수정용 컴퓨트 쉐이더


public:
    void SetFace(UINT _FaceX, UINT _FaceZ);
    void SetHeightMap(Ptr<ATexture> _HeightMap) { m_HeightMap = _HeightMap; }
    void CreateHeightMap(UINT _Width, UINT _Height);

public:
    virtual void FinalTick() override;
    virtual void Render() override;

private:
    void CreateMesh();
    void CreateMaterial();  
    void CreateComputeShader();    
    int Raycasting();

    CLONE(CLandScape);
public:
    CLandScape();
    virtual ~CLandScape();
};

