#include "pch.h"
#include "CMissileScript.h"

#include <Engine/GameObject.h>
#include <Engine/CTransform.h>

CMissileScript::CMissileScript()
	: CScript((UINT)SCRIPT_TYPE::MISSILESCRIPT)
	, m_Speed(200.f)
{
}

CMissileScript::~CMissileScript()
{
}

void CMissileScript::Begin()
{
	if (GetOwner()->Collider2D())
	{
		GetOwner()->Collider2D()->AddDelegate(this, (OVERLAP)&CMissileScript::BeginOverlap , nullptr , nullptr);
	}
}

void CMissileScript::Tick()
{
	Vec3 vPos = Transform()->GetRelativePos();

	vPos.y += DT * m_Speed;

	Transform()->SetRelativePos(vPos);
}

void CMissileScript::SaveToLevelFile(FILE* _File)
{
}

void CMissileScript::LoadFromLevelFile(FILE* _File)
{
}

void CMissileScript::BeginOverlap(CCollider2D* _OwnCollider, GameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	DestroyGameObject(GetOwner());
}
