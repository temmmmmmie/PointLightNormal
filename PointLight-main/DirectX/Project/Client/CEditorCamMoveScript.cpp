#include "pch.h"
#include "CEditorCamMoveScript.h"

#include <Engine/CTransform.h>
#include <Engine/CCamera.h>

CEditorCamMoveScript::CEditorCamMoveScript()
	: CScript(-1)
	, m_Speed(500.f)
{

}

CEditorCamMoveScript::~CEditorCamMoveScript()
{

}

void CEditorCamMoveScript::Tick()
{
	// Orthograhpic
	if (PROJ_TYPE::ORTHOGRAPHIC == Camera()->GetProjType())
	{
		MoveCamera2D();
	}

	// Perspective
	else
	{
		MoveCamera3D();
	}
}



void CEditorCamMoveScript::MoveCamera2D()
{
	Vec3 vPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::W))
		vPos.y += m_Speed * ENGINE_DT;
	if (KEY_PRESSED(KEY::S))
		vPos.y -= m_Speed * ENGINE_DT;
	if (KEY_PRESSED(KEY::A))
		vPos.x -= m_Speed * ENGINE_DT;
	if (KEY_PRESSED(KEY::D))
		vPos.x += m_Speed * ENGINE_DT;

	Transform()->SetRelativePos(vPos);
}

void CEditorCamMoveScript::MoveCamera3D()
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vFront = Transform()->GetLocalDir(DIR::FRONT);
	Vec3 vRight = Transform()->GetLocalDir(DIR::RIGHT);

	if (KEY_PRESSED(KEY::W))
		vPos += vFront * m_Speed * ENGINE_DT;
	if (KEY_PRESSED(KEY::S))
		vPos += -vFront * m_Speed * ENGINE_DT;
	if (KEY_PRESSED(KEY::A))
		vPos += -vRight * m_Speed * ENGINE_DT;
	if (KEY_PRESSED(KEY::D))
		vPos += vRight * m_Speed * ENGINE_DT;

	Transform()->SetRelativePos(vPos);

	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec3 vRot = Transform()->GetRelativeRot();
		Vec2 vDragDir = KeyMgr::GetInst()->GetMouseDragDir();

		vRot.y += vDragDir.x * XM_PI * 2.f * ENGINE_DT;
		vRot.x += vDragDir.y * XM_PI * 2.f * ENGINE_DT;

		Transform()->SetRelativeRot(vRot);
	}	
}

void CEditorCamMoveScript::SaveToLevelFile(FILE* _File)
{
}

void CEditorCamMoveScript::LoadFromLevelFile(FILE* _File)
{
}