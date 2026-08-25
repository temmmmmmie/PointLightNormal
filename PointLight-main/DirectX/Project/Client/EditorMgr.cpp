#include "pch.h"
#include "EditorMgr.h"

#include <Engine/Engine.h>
#include <Engine/Device.h>
#include <Engine/PathMgr.h>
#include <Engine/RenderMgr.h>
#include <Engine/KeyMgr.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"


EditorMgr::EditorMgr()
    : m_ShowDemo(false)
{
}

EditorMgr::~EditorMgr()
{
    SetCurrentDirectory(PathMgr::GetInst()->GetBinPath().c_str());
   
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void EditorMgr::Init()
{
	// 에디터용 오브젝트 제작
	CreateEditorGameObject();

	// ImGui 파트 초기화
	InitImGui();
}

void EditorMgr::Progress()
{
    EditorObjectProgress();

    ImGuiProgress();
}

void EditorMgr::EditorObjectProgress()
{
    for (size_t i = 0; i < m_vecEditorObj.size(); ++i)
    {
        m_vecEditorObj[i]->Tick();
    }

    for (size_t i = 0; i < m_vecEditorObj.size(); ++i)
    {
        m_vecEditorObj[i]->FinalTick();
    }
}

void EditorMgr::ImGuiProgress()
{
    // =====================
    // ImGui FrameWork Start
    // =====================
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // ==================
    // ImGui Update(Tick)
    // ==================
    if (KEY_TAP(KEY::_9))
        m_ShowDemo = !m_ShowDemo;
    if (m_ShowDemo)
        ImGui::ShowDemoWindow(&m_ShowDemo);

    // 모든 EditorUI 업데이트
    for (const auto& pair : m_mapUI)
    {
        if(pair.second->IsActive())
            pair.second->Tick();
    }

    // ===============
    // ImGui Rendering
    // ===============
    ImGui::Render();  
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

// Vector2 에 구현되어있는 ImVec2 전환 컨버전 오퍼레이터 실제 구현
Vector2::operator ImVec2() const
{
    return ImVec2(x, y);    
}