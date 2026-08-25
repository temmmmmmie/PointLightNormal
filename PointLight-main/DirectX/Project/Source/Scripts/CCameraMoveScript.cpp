#include "pch.h"
#include "CCameraMoveScript.h"

#include <Engine/CTransform.h>
#include <Engine/CCamera.h>

CCameraMoveScript::CCameraMoveScript()
	: CScript((UINT)SCRIPT_TYPE::CAMERAMOVESCRIPT)
	, m_Speed(500.f)
{

}

CCameraMoveScript::~CCameraMoveScript()
{

}

void CCameraMoveScript::Tick()
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

void CCameraMoveScript::SaveToLevelFile(FILE* _File)
{
}

void CCameraMoveScript::LoadFromLevelFile(FILE* _File)
{
}


void CCameraMoveScript::MoveCamera2D()
{
	Vec3 vPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::W))
		vPos.y += m_Speed * DT;
	if (KEY_PRESSED(KEY::S))
		vPos.y -= m_Speed * DT;
	if (KEY_PRESSED(KEY::A))
		vPos.x -= m_Speed * DT;
	if (KEY_PRESSED(KEY::D))
		vPos.x += m_Speed * DT;

	Transform()->SetRelativePos(vPos);
}

void CCameraMoveScript::MoveCamera3D()
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vFront = Transform()->GetLocalDir(DIR::FRONT);
	Vec3 vRight = Transform()->GetLocalDir(DIR::RIGHT);

	if (KEY_PRESSED(KEY::W))
		vPos += vFront * m_Speed * DT;
	if (KEY_PRESSED(KEY::S))
		vPos += -vFront * m_Speed * DT;
	if (KEY_PRESSED(KEY::A))
		vPos += -vRight * m_Speed * DT;
	if (KEY_PRESSED(KEY::D))
		vPos += vRight * m_Speed * DT;

	Transform()->SetRelativePos(vPos);



	Vec3 vRot = Transform()->GetRelativeRot();

	if (KEY_PRESSED(KEY::Q))
		vRot.y -= XM_PI * DT;
	if (KEY_PRESSED(KEY::E))
		vRot.y += XM_PI * DT;

	Transform()->SetRelativeRot(vRot);
}
