#include "pch.h"
#include "RenderMgr.h"

#include "Device.h"
#include "StructuredBuffer.h"

#include "LevelMgr.h"
#include "Level.h"

RenderMgr::RenderMgr()
    : m_RenderMode(RENDER_MODE::EDITOR)
    , m_RenderFunc(nullptr)
{
    ChangeRenderMode(m_RenderMode);
    
    m_Light2DBuffer = new StructuredBuffer;
    m_Light3DBuffer = new StructuredBuffer;
}

RenderMgr::~RenderMgr()
{
}

void RenderMgr::Binding()
{
    static vector<tLightInfo> vecLightInfo;

    // Light2D 정보를 구조화버퍼에 담아서 레지스터에 바인딩
    {
        // 구조화버퍼의 크기가 모자라면 확장한다.
        if (m_Light2DBuffer->GetElementCount() < m_vecLight2D.size())
        {
            m_Light2DBuffer->Create(sizeof(tLightInfo), m_vecLight2D.size(), SB_TYPE::SRV_ONLY, true);
        }

        // RenderMgr 에 등록된 Light2D 컴포넌트가 보유한 tLightInfo 정보를 벡터에 모은다.
        vecLightInfo.clear();
        for (size_t i = 0; i < m_vecLight2D.size(); ++i)
        {
            vecLightInfo.push_back(m_vecLight2D[i]->GetLight2DInfo());
        }

        // 모아진 tLightInfo 정보를 구조화버퍼에 보낸다.
        if (false == m_vecLight2D.empty())
        {
            m_Light2DBuffer->SetData(vecLightInfo.data(), sizeof(tLightInfo) * vecLightInfo.size());

            // 광원 정보를 전달받은 구조화버퍼를 t13 레지스터에 바인딩한다.
            m_Light2DBuffer->Binding(13);
        }
    }

    // Light3D 정보를 구조화버퍼에 담아서 레지스터에 바인딩
    {
        // 구조화버퍼의 크기가 모자라면 확장한다.
        if (m_Light3DBuffer->GetElementCount() < m_vecLight3D.size())
        {
            m_Light3DBuffer->Create(sizeof(tLightInfo), m_vecLight3D.size(), SB_TYPE::SRV_ONLY, true);
        }

        // RenderMgr 에 등록된 Light3D 컴포넌트가 보유한 tLightInfo 정보를 벡터에 모은다.
        vecLightInfo.clear();
        for (size_t i = 0; i < m_vecLight3D.size(); ++i)
        {
            vecLightInfo.push_back(m_vecLight3D[i]->GetLight3DInfo());
        }

        // 모아진 tLightInfo 정보를 구조화버퍼에 보낸다.
        if (false == m_vecLight3D.empty())
        {
            m_Light3DBuffer->SetData(vecLightInfo.data(), sizeof(tLightInfo) * vecLightInfo.size());

            // 광원 정보를 전달받은 구조화버퍼를 t14 레지스터에 바인딩한다.
            m_Light3DBuffer->Binding(14);
        }
    }
  

    // Global 데이터 상수 레지스터에 전달
    g_Global.Light2DCount = m_vecLight2D.size();
    g_Global.Light3DCount = m_vecLight3D.size();

    static Ptr<ConstBuffer> pBuffer = Device::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL);
    pBuffer->SetData(&g_Global);
    pBuffer->Binding();
    pBuffer->Binding_CS();

    // Sampler 바인딩
    Device::GetInst()->BindingSampler();
}

void RenderMgr::MRTClear()
{
    // SwapChain 은 렌더타겟, 깊이타겟 클리어
    m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->Clear();

    // DeferredMRT 는 Swapchain MRT 와 동일한 깊이타겟을 공유하므로, 렌더타겟만 클리어
    m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->ClearRT();

    // LightMRT
    m_arrMRT[(UINT)MRT_TYPE::LIGHT]->ClearRT();
}

void RenderMgr::Render_Start()
{
    // 렌더링 시작전에 필요한 리소스를 레지스터에 바인딩
    Binding();

    // MRT 들을 클리어
    MRTClear();    
}

void RenderMgr::Render_Play()
{
    // 카메라로 Level 렌더링
    for (size_t i = 0; i < m_vecCam.size(); ++i)
    {
        if (nullptr == m_vecCam[i])
            continue;

        m_vecCam[i]->Render();
    }
}

void RenderMgr::Render_Editor()
{
    if (nullptr != m_EditorCam)
        m_EditorCam->Render();
}

void RenderMgr::Render_Lighting()
{
    m_arrMRT[(UINT)MRT_TYPE::LIGHT]->OMSet();

    for (size_t i = 0; i < m_vecLight3D.size(); ++i)
    {
        m_vecLight3D[i]->Render();
    }
}

void RenderMgr::Render_Merge()
{
    m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->OMSet();

    m_MergeMtrl->Binding();
    FIND(AMesh, L"RectMesh")->Render(0);
}

void RenderMgr::Render_SelectedTarget()
{
    if (nullptr == m_SelectedTarget)
        return;

    m_SelectedMtrl->SetTexture(TEX_0, m_SelectedTarget);
    m_SelectedMtrl->Binding();
    FIND(AMesh, L"RectMesh")->Render(0);
}


void RenderMgr::Progress()
{
    // 전역 상수데이터 전달 및 타겟 클리어
    Render_Start();
    
    // 렌더링
    (this->*m_RenderFunc)();

    // SelectedTarget 으로 화면 출력
    Render_SelectedTarget();

    // DebugRender
    Render_Debug();

    // 리소스 클리어
    Clear();
        
#ifdef _DEBUG
    // Frame, DT 출력
    TimeMgr::GetInst()->Render();
#endif
}


void RenderMgr::Render_Debug()
{
    if (m_DebugShapeList.empty())
        return;

    list<tDebugShapeInfo>::iterator iter = m_DebugShapeList.begin();
    for (; iter != m_DebugShapeList.end(); )
    {       
        // 쉐이프 타입에 맞는 메시 설정
        switch (iter->ShapeType)
        {
        case DEBUG_SHAPE::RECT:
            m_DebugObj->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh_LineStrip"));
            m_DebugObj->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"DebugShapeMtrl"), 0);
            m_DebugObj->MeshRender()->GetMaterial(0)->GetShader()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
            m_DebugObj->MeshRender()->GetMaterial(0)->GetShader()->SetRSType(RS_TYPE::CULL_NONE);
            m_DebugObj->MeshRender()->GetMaterial(0)->GetShader()->SetBSType(BS_TYPE::DEFAULT);
            break;
        case DEBUG_SHAPE::CIRCLE:
            m_DebugObj->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"CircleMesh_LineStrip"));
            m_DebugObj->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"DebugShapeMtrl"), 0);
            m_DebugObj->MeshRender()->GetMaterial(0)->GetShader()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
            m_DebugObj->MeshRender()->GetMaterial(0)->GetShader()->SetRSType(RS_TYPE::CULL_NONE);
            m_DebugObj->MeshRender()->GetMaterial(0)->GetShader()->SetBSType(BS_TYPE::DEFAULT);
            break;
        case DEBUG_SHAPE::LINE:
            m_DebugObj->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"PointMesh"));
            m_DebugObj->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"DebugShapeMtrl"), 0);
            m_DebugObj->MeshRender()->GetMaterial(0)->GetShader()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
            m_DebugObj->MeshRender()->GetMaterial(0)->GetShader()->SetRSType(RS_TYPE::CULL_NONE);
            m_DebugObj->MeshRender()->GetMaterial(0)->GetShader()->SetBSType(BS_TYPE::DEFAULT);
            break;
        case DEBUG_SHAPE::CUBE:
            m_DebugObj->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"CubeMesh_LineStrip"));
            m_DebugObj->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"DebugShapeMtrl"), 0);
            m_DebugObj->MeshRender()->GetMaterial(0)->GetShader()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
            m_DebugObj->MeshRender()->GetMaterial(0)->GetShader()->SetRSType(RS_TYPE::CULL_NONE);
            m_DebugObj->MeshRender()->GetMaterial(0)->GetShader()->SetBSType(BS_TYPE::DEFAULT);
            break;
        case DEBUG_SHAPE::SPHERE:           
            m_DebugObj->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"SphereMesh"));
            m_DebugObj->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"DebugShapeMtrl"), 0);
            m_DebugObj->MeshRender()->GetMaterial(0)->GetShader()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_DebugObj->MeshRender()->GetMaterial(0)->GetShader()->SetBSType(BS_TYPE::ALPHABLEND);
            m_DebugObj->MeshRender()->GetMaterial(0)->GetShader()->SetRSType(RS_TYPE::CULL_FRONT);
            break;
        }

        // 위치 크기 회전정보 세팅
        if (iter->matWorld != XMMatrixIdentity())
        {
            m_DebugObj->Transform()->SetmatWorld(iter->matWorld);
        }

        else
        {
            m_DebugObj->Transform()->SetRelativePos(iter->WorldPos);
            m_DebugObj->Transform()->SetRelativeScale(iter->Scale);            
            m_DebugObj->Transform()->SetRelativeRot(iter->Rotation);
            m_DebugObj->Transform()->FinalTick();            
        }
        
        // 쉐이더 깊이판정 옵션 설정
        if (iter->DepthTest)
            m_DebugObj->MeshRender()->GetMaterial(0)->GetShader()->SetDSType(DS_TYPE::LESS);
        else
            m_DebugObj->MeshRender()->GetMaterial(0)->GetShader()->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);


        m_DebugObj->MeshRender()->GetMaterial(0)->SetScalar(INT_0, (int)iter->ShapeType);
        m_DebugObj->MeshRender()->GetMaterial(0)->SetScalar(VEC4_0, iter->Color);
        m_DebugObj->Render();

        iter->Age += ENGINE_DT;
        if (iter->Life < iter->Age)
        {
            iter = m_DebugShapeList.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void RenderMgr::Clear()
{
    // 등록받았던 광원을 해제한다.
    m_vecLight2D.clear();
    m_vecLight3D.clear();

    // 광원의 정보를 담아서 t13, t14 에 바인딩했던 구조화버퍼를 클리어한다.
    m_Light2DBuffer->Clear();
    m_Light3DBuffer->Clear();
}


bool RenderMgr::RegisterCamera(Ptr<CCamera> _Cam, int _Priority)
{
    if (_Priority < 0)
        return false;

    if (m_vecCam.size() < _Priority + 1)
    {
        m_vecCam.resize(_Priority + 1);
    }

    m_vecCam[_Priority] = _Cam;

    return true;
}

Ptr<CCamera> RenderMgr::GetPOVCamera()
{
    Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurrentLevel();

    if (nullptr == pCurLevel || LEVEL_STATE::PLAY != pCurLevel->GetState())
    {
        return m_EditorCam;
    }
    else
    {
        if (m_vecCam.empty())
            return nullptr;

        return m_vecCam[0];
    }
}

void RenderMgr::ChangeRenderMode(RENDER_MODE _RenderMode)
{
    m_RenderMode = _RenderMode;

    if (RENDER_MODE::PLAY == m_RenderMode)
        m_RenderFunc = &RenderMgr::Render_Play;
    else
        m_RenderFunc = &RenderMgr::Render_Editor;
}

void RenderMgr::CopyTarget()
{
    // SwapChain MRT 의 0번 RenderTargetTex 가 SwapChain 타겟이다.
    Ptr<ATexture> RTTex = m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->GetRT(0);
    CONTEXT->CopyResource(m_RTCopyTex->GetTex2D().Get(), RTTex->GetTex2D().Get());
}

void RenderMgr::GetRenderTargets(vector<Ptr<ATexture>>& _vecTargets)
{
    for (int i = 0; i < (UINT)MRT_TYPE::END; ++i)
    {
        if (nullptr == m_arrMRT[i] 
            || i == (UINT)MRT_TYPE::SWAPCHAIN
            || i == (UINT)MRT_TYPE::DECAL)
            continue;

        for (int j = 0; j < m_arrMRT[i]->GetRTCount(); ++j)
        {
            _vecTargets.push_back(m_arrMRT[i]->GetRT(j));
        }
    }
}