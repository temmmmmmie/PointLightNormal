#include "pch.h"
#include "CPlayerScript.h"

#include <Engine/RenderMgr.h>
#include <Engine/TaskMgr.h>
#include <Engine/GameObject.h>

#include "CMissileScript.h"

CPlayerScript::CPlayerScript()
	: CScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT)
	, m_Speed(100)
	, m_Pow(0.f)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, &m_Speed, L"Player Speed");

	AddScriptParam(SCRIPT_PARAM::PREFAB, &m_MissilePref, L"Missile Prefab");
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::Begin()
{
	if (GetOwner()->Collider2D())
	{
		GetOwner()->Collider2D()->AddDelegate(this
			, (OVERLAP)&CPlayerScript::BeginOverlap
			, (OVERLAP)&CPlayerScript::Overlap
			, (OVERLAP)&CPlayerScript::EndOverlap);
	}
}

void CPlayerScript::Tick()
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vScale = Transform()->GetRelativeScale();
	Vec3 vRot = Transform()->GetRelativeRot();

	if (KEY_PRESSED(KEY::LEFT))
		vPos.x -= DT * m_Speed;
	if (KEY_PRESSED(KEY::RIGHT))
		vPos.x += DT * m_Speed;
	if (KEY_PRESSED(KEY::UP))
		vPos.y += DT * m_Speed;
	if (KEY_PRESSED(KEY::DOWN))
		vPos.y -= DT * m_Speed;

	if (KEY_PRESSED(KEY::Z))
	{
		vRot.z += XM_PI * DT;
		vPos.z += 100 * DT;
	}

	Transform()->SetRelativePos(vPos);
	Transform()->SetRelativeScale(vScale);
	Transform()->SetRelativeRot(vRot);


	if (KEY_TAP(KEY::SPACE))
	{
		if (nullptr != m_MissilePref)
		{
			GameObject* pMissileObj = m_MissilePref->Instantiate();
			pMissileObj->Transform()->SetRelativePos(GetOwner()->Transform()->GetRelativePos());
			SpawnGameObject(3, pMissileObj);
		}		
	}

	//if (nullptr != GetRenderComponent())
	//{
	//	m_Pow += DT * 0.3f;
	//	if (m_Pow > 1.f)
	//		m_Pow = 1.f;

	//	GetRenderComponent()->GetMaterial()->SetScalar(FLOAT_0, m_Pow);
	//}
}



void CPlayerScript::BeginOverlap(CCollider2D* _OwnCollider, GameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	int a = 0;
}

void CPlayerScript::Overlap(CCollider2D* _OwnCollider, GameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CPlayerScript::EndOverlap(CCollider2D* _OwnCollider, GameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CPlayerScript::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_Speed, sizeof(float), 1, _File);
	
	SaveAssetRef(m_MissilePref.Get(), _File);
}

void CPlayerScript::LoadFromLevelFile(FILE* _File)
{
	fread(&m_Speed, sizeof(float), 1, _File);

	m_MissilePref = LoadAssetRef<APrefab>(_File);
}