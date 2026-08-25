#include "pch.h"
#include "CMonsterScript.h"

#include <Engine/GameObject.h>

CMonsterScript::CMonsterScript()
	: CScript((UINT)SCRIPT_TYPE::MONSTERSCRIPT)
	, m_Speed(200.f)
{
}

CMonsterScript::~CMonsterScript()
{
}

void CMonsterScript::Begin()
{
	if (GetOwner()->Collider2D())
	{
		GetOwner()->Collider2D()->AddDelegate(this
			, (OVERLAP)&CMonsterScript::BeginOverlap
			, nullptr
			, nullptr);
	}
}

void CMonsterScript::Tick()
{
	if (KEY_TAP(KEY::P))
	{
		Ptr<AMaterial> pMtrl = GetRenderComponent()->GetDynamicMaterial(0);
		pMtrl->SetScalar(INT_0, 1);
	}

	if (KEY_TAP(KEY::O))
	{
		Ptr<AMaterial> pMtrl = GetRenderComponent()->GetMaterial(0);
		pMtrl->SetScalar(INT_0, 0);

		GetRenderComponent()->GetSharedMaterial(0);
	}
}

void CMonsterScript::SaveToLevelFile(FILE* _File)
{
}

void CMonsterScript::LoadFromLevelFile(FILE* _File)
{
}

void CMonsterScript::BeginOverlap(CCollider2D* _OwnCollider, GameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}