#include "pch.h"
#include "CLight2D.h"

#include "RenderMgr.h"
#include "CTransform.h"

CLight2D::CLight2D()
	: Component(COMPONENT_TYPE::LIGHT2D)
	, m_Info{}
{
}

CLight2D::~CLight2D()
{
}

void CLight2D::FinalTick()
{
	// 광원 위치정보 갱신
	m_Info.WorldPos = Transform()->GetWorldPos();

	// 광원의 방향정보 갱신
	m_Info.Direction = Transform()->GetWorldDir(DIR::RIGHT);

	// 광원 위치에 DebugRendering 으로 광원의 위치를 표시해줌
	DrawDebugRect(Transform()->GetWorldPos(), Vec3(10.f, 10.f, 1.f), Vec3(0.f, 0.f, 0.f), Vec4(1.f, 1.f, 0.f, 1.f), false);

	// 광원 등록
	RenderMgr::GetInst()->RegisterLight2D(this);
}

void CLight2D::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_Info, sizeof(tLightInfo), 1, _File);
}

void CLight2D::LoadFromLevelFile(FILE* _File)
{
	fread(&m_Info, sizeof(tLightInfo), 1, _File);
}
