#include "pch.h"
#include "CameraUI.h"


CameraUI::CameraUI()
	: ComponentUI("CameraUI", COMPONENT_TYPE::CAMERA)
{
}

CameraUI::~CameraUI()
{
}

void CameraUI::Tick_UI()
{
	ComponentUI::Tick_UI();

    CCamera* pCam = GetTargetObject()->Camera();

    // 카메라 우선순위( 0 : MainCamera, 1 이상 : SubCamera)
    {
        ImGui::Text("Priority");
        ImGui::SameLine(120);
        int Priority = pCam->GetPriority();
        ImGui::SetNextItemWidth(100);
        if (ImGui::InputInt("##Priority", &Priority, 0))
        {
            pCam->SetPriority(Priority);
        }
    }

    // 투영 방식
    {
        ImGui::Text("Projection");
        ImGui::SameLine(120);

        PROJ_TYPE CurType = pCam->GetProjType();

        int SelectID = (int)CurType;
        if (ImGui::RadioButton("ORTHOGRAPHIC", &SelectID, 0))
            pCam->SetProjType((PROJ_TYPE)0);

        ImGui::SameLine();

        if (ImGui::RadioButton("PERSPECTIVE", &SelectID, 1))
            pCam->SetProjType((PROJ_TYPE)1);
    }
  
    // 직교투영 배율, 가로 범위
    {
        ImGui::BeginDisabled(pCam->GetProjType() == PROJ_TYPE::PERSPECTIVE);

        ImGui::Text("Width");
        ImGui::SameLine(120);

        float Width = pCam->GetWidth();
        ImGui::SetNextItemWidth(100);
        if (ImGui::InputFloat("##Width", &Width))
        {
            pCam->SetWidth(Width);
        }

        ImGui::Text("OrthoScale");
        ImGui::SameLine(120);
        ImGui::SetNextItemWidth(100);
        float OrthoScale = pCam->GetOrthoScale();
        if (ImGui::DragFloat("##OrthoScale", &OrthoScale, 0.01f))
        {
            if (OrthoScale <= 0)
                OrthoScale = 0.1f;

            pCam->SetOrthoScale(OrthoScale);
        }

        ImGui::EndDisabled();
    }

    // 시야 각(FOV, Fileld Of View)
    {
        ImGui::BeginDisabled(pCam->GetProjType() == PROJ_TYPE::ORTHOGRAPHIC);

        ImGui::Text("FOV");

        ImGui::SameLine(120);

        float FOV = pCam->GetFOV();
        FOV = (FOV / XM_PI) * 180.f;

        ImGui::SetNextItemWidth(100);
        if (ImGui::DragFloat("##FOV", &FOV, 0.01f))
        {
            FOV = (FOV / 180.f) * XM_PI;
            pCam->SetFOV(FOV);
        }

        ImGui::EndDisabled();
    }


    // 종횡비
    {
        ImGui::Text("AspectRatio");
        ImGui::SameLine(120);
        float AspectRatio = pCam->GetAspectRatio();
        ImGui::SetNextItemWidth(100);
        if (ImGui::InputFloat("##AspectRatio", &AspectRatio))
        {
            pCam->SetAspectRatio(AspectRatio);
        }
    }

    // 최대시야 거리
    {
        ImGui::Text("MaxDistance");
        ImGui::SameLine(120);
        float Far = pCam->GetFar();
        ImGui::SetNextItemWidth(100);
        if (ImGui::InputFloat("##Far", &Far))
        {
            pCam->SetFar(Far);
        }
    }

    
    //UINT                m_LayerCheck;   // 렌더링할 레이어 필터링하기 위한 비트값
}