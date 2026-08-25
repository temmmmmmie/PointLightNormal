#include "pch.h"
#include "EditorMgr.h"

#include <Engine/RenderMgr.h>
#include "EditorUI.h"
#include "CEditorCamMoveScript.h"

void EditorMgr::CreateEditorGameObject()
{
	// =============
	// Editor Camera
	// =============
	Ptr<GameObjectEx> pEditorCam = new GameObjectEx;
	pEditorCam->SetName(L"Editor Camera");
	pEditorCam->AddComponent(new CTransform);
	pEditorCam->AddComponent(new CCamera);
	pEditorCam->AddComponent(new CEditorCamMoveScript);

	Vec2 vResol = Device::GetInst()->GetRenderResolution();

	pEditorCam->Transform()->SetRelativePos(Vec3(0.f, 0.f, -500.f));
	pEditorCam->Camera()->LayerCheckAll();
	pEditorCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pEditorCam->Camera()->SetOrthoScale(1.f);
	pEditorCam->Camera()->SetWidth(vResol.x);
	pEditorCam->Camera()->SetAspectRatio(vResol.x / vResol.y);
	pEditorCam->Camera()->SetFar(100000.f);

	m_vecEditorObj.push_back(pEditorCam);

	// Editor 카메라 등록
	RenderMgr::GetInst()->RegisterEditorCamera(pEditorCam->Camera());

}