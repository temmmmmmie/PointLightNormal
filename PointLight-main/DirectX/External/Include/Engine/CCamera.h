#pragma once
#include "Component.h"

#include "Frustum.h"

class GameObject;

enum class PROJ_TYPE
{
    ORTHOGRAPHIC,   // 직교투영
    PERSPECTIVE,    // 원근투영
};

class CCamera :
    public Component
{
private:
    Frustum             m_Frustum;

    PROJ_TYPE           m_ProjType;
    float               m_OrthoScale;   // 직교투영 배율

    float               m_Width;        // 투영 가로길이
    float               m_AspectRatio;  // 종횡비(출력시킬 렌더타겟의 세로 대비 가로의 비율값)

    float               m_FOV;          // 시야 각(FOV, Fileld Of View)
    float               m_Far;          // 카메라가 볼 수 있는 최대 시야거리

    int                 m_Priority;     // 카메라 우선순위( 0 : MainCamera, 1 이상 : SubCamera)
    UINT                m_LayerCheck;   // 렌더링할 레이어 필터링하기 위한 비트값

    Matrix              m_matWorld; // 월드에 있는 좌표를 카메라가 기준(중심)이 되는 ViewSpace 로 이동시키는 행렬
    Matrix              m_matWorldInv; // 월드에 있는 좌표를 카메라가 기준(중심)이 되는 ViewSpace 로 이동시키는 행렬
    
    
    Matrix              m_matView; // 월드에 있는 좌표를 카메라가 기준(중심)이 되는 ViewSpace 로 이동시키는 행렬
    Matrix              m_matViewInv; // 월드에 있는 좌표를 카메라가 기준(중심)이 되는 ViewSpace 로 이동시키는 행렬

    Matrix              m_matProj; // View 공간에 있는 좌표를 2차원 NDC 좌표계로 이동시킴, z 값은 깊이값으로 사용된다.
    Matrix              m_matProjInv; // View 공간에 있는 좌표를 2차원 NDC 좌표계로 이동시킴, z 값은 깊이값으로 사용된다.

    tRay                m_Ray;

    vector<GameObject*> m_vecDeferred;
    vector<GameObject*> m_vecDecal;
    vector<GameObject*> m_vecOpaque;
    vector<GameObject*> m_vecMask;
    vector<GameObject*> m_vecTransparent;
    vector<GameObject*> m_vecParticles;
    vector<GameObject*> m_vecPostProcess;


    vector<GameObject*> m_vecShadowMap;


public:
    GET_SET(PROJ_TYPE, ProjType);
    GET_SET(float, OrthoScale);
    GET_SET(float, AspectRatio);
    GET_SET(float, Width);
    GET_SET(float, FOV);
    GET_SET(int, Priority);
    GET_SET(float, Far);

    const Matrix& GetViewMat() { return m_matView; }
    const Matrix& GetViewInvMat() { return m_matViewInv; }
    const Matrix& GetProjMat() { return m_matProj; }
    const Matrix& GetProjInvMat() { return m_matProjInv; }

    void LayerCheck(UINT _LayerIdx)
    {
        //m_LayerCheck |= (1 << _LayerIdx);
        //m_LayerCheck &= ~(1 << _LayerIdx);
        m_LayerCheck ^= (1 << _LayerIdx);
    }
    void LayerCheckAll() { m_LayerCheck = 0xffffffff; }
    void LayerCheckClear() { m_LayerCheck = 0; }

    const tRay& GetRay() { return m_Ray; }


public:
    virtual void Begin() override;
    virtual void FinalTick() override;

private:
    void Render();
    void SortObject();

    void SortObject_ShadowMap();
    void Render_ShadowMap(Ptr<AMaterial> _ShadowMapMtrl);

    void CalcRay();

    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(CCamera);
public:
    CCamera();
    CCamera(const CCamera& _Origin);
    virtual ~CCamera();

    friend class RenderMgr;
    friend class CLight3D;
};

