#include "pch.h"
#include "LevelMgr.h"

#include "device.h"
#include "AssetMgr.h"
#include "CollisionMgr.h"

#include "RenderMgr.h"

LevelMgr::LevelMgr()
{}

LevelMgr::~LevelMgr()
{}

Ptr<GameObject> LevelMgr::FindObjectByName(const wstring& _Name)
{
	if (nullptr == m_CurLevel)
		return nullptr;

	return m_CurLevel->FindObjectByName(_Name);
}

void LevelMgr::Init()
{
}

void LevelMgr::Progress()
{
	if (nullptr == m_CurLevel)
		return;

	// 레벨 업데이트
	m_CurLevel->Deregister();
	
	if (LEVEL_STATE::PLAY == m_CurLevel->GetState())
	{
		m_CurLevel->Tick();
	}
	
	m_CurLevel->FinalTick();

	// 충돌 검사 진행
	CollisionMgr::GetInst()->Progress();

	// 렌더링
	RenderMgr::GetInst()->Progress();
}
