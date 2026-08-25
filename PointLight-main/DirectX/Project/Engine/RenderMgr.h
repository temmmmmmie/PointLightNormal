#pragma once
#include "singleton.h"

#include "GameObject.h"
#include "StructuredBuffer.h"

#include "MRT.h"

enum class RENDER_MODE
{
    PLAY,
    EDITOR,
};

class RenderMgr :
    public singleton<RenderMgr>
{
    SINGLE(RenderMgr);
private:
    vector<Ptr<CCamera>>    m_vecCam;           // Level 안에 있는 카메라
    Ptr<CCamera>            m_EditorCam;        // Editor 용 카메라

    vector<Ptr<CLight2D>>   m_vecLight2D;       // Level 안에 있는 2D 광원
    Ptr<StructuredBuffer>   m_Light2DBuffer;    // 광원 정보를 담아서 Register 에 바인딩

    vector<Ptr<CLight3D>>   m_vecLight3D;       // Level 안에 있는 3D 광원
    Ptr<StructuredBuffer>   m_Light3DBuffer;    // 광원 정보를 담아서 Register 에 바인딩

    Ptr<GameObject>         m_DebugObj;
    list<tDebugShapeInfo>   m_DebugShapeList;
       
    Ptr<ATexture>           m_RTCopyTex;

    RENDER_MODE             m_RenderMode;
    void(RenderMgr::*       m_RenderFunc)(void);

    Ptr<MRT>                m_arrMRT[(UINT)MRT_TYPE::END];
    Ptr<ATexture>           m_SelectedTarget;

    Ptr<AMaterial>          m_MergeMtrl;
    Ptr<AMaterial>          m_SelectedMtrl;
    

public:
    bool RegisterCamera(Ptr<CCamera> _Cam, int _Priority);
    void RegisterEditorCamera(Ptr<CCamera> _Cam) { m_EditorCam = _Cam; }

    void RegisterLight2D(Ptr<CLight2D> _Light) { m_vecLight2D.push_back(_Light); }
    int RegisterLight3D(Ptr<CLight3D> _Light) { m_vecLight3D.push_back(_Light); return m_vecLight3D.size() - 1; }

    Ptr<CCamera> GetPOVCamera();

    void AddDebugShapeInfo(const tDebugShapeInfo& _Info) { m_DebugShapeList.push_back(_Info); }
    void ChangeRenderMode(RENDER_MODE _RenderMode);
    void CopyTarget();

    Ptr<MRT> GetMRT(MRT_TYPE _Type) { return m_arrMRT[(UINT)_Type]; }
    void GetRenderTargets(vector<Ptr<ATexture>>& _vecTargets);
    void SetSelectedTarget(Ptr<ATexture> _Target) { m_SelectedTarget = _Target; }
    Ptr<ATexture> GetSelectedTarget() { return m_SelectedTarget; }

public:
    void Init();
    void Progress();
     
private:
    void Binding();
    void MRTClear();

    void Render_Start();
    void Render_Play();
    void Render_Editor();
    void Render_Lighting();
    void Render_Merge();
    void Render_SelectedTarget();
    void Render_Debug();
    void Clear();

    void CreateMRT();

    friend class CCamera;
};

