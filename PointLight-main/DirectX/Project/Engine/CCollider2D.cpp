#include "pch.h"
#include "CCollider2D.h"

#include "CTransform.h"

CCollider2D::CCollider2D()
	: Component(COMPONENT_TYPE::COLLIDER2D)
	, m_Scale(Vec3(1.f, 1.f, 1.f))
	, m_IndependentScale(false)
	, m_OverlapCount(0)
{
}

CCollider2D::CCollider2D(const CCollider2D& _Origin)
	: Component(_Origin)
	, m_Offset(_Origin.m_Offset)
	, m_Scale(_Origin.m_Scale)
	, m_IndependentScale(_Origin.m_IndependentScale)
	, m_OverlapCount(0)
{
}

CCollider2D::~CCollider2D()
{
}


void CCollider2D::FinalTick()
{
	/*Vec3 vPos = Transform()->GetPos();
	Vec3 vScale = Transform()->GetScale();
	Vec3 vRotation = Transform()->GetRotation();

	Vec3 vFinalPos = vPos + m_Offset;
	Vec3 vFinalScale = m_Scale * vScale;

	DrawDebugRect(vFinalPos, vFinalScale, vRotation, Vec4(0.f, 1.f, 0.f, 1.f), true, 0.f);*/

	Matrix Scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	Matrix matTrans = XMMatrixTranslation(m_Offset.x, m_Offset.y, m_Offset.z);
	Matrix matObjWorld = Transform()->GetmatWorld();

	if (false == m_IndependentScale)
	{
		// 크 자 이 x 부(GameObject)
		m_matWorld = Scale * matTrans * matObjWorld;
	}
	else
	{
		// 크 자 이 x 부(크' 자' 이')
		Vec3 ObjScale = Transform()->GetRelativeScale();
		Matrix matObjScale = XMMatrixScaling(ObjScale.x, ObjScale.y, ObjScale.z);
		Matrix matObjScaleInv = XMMatrixInverse(nullptr, matObjScale);

		// 크 자 이 x (크'-1) x 부(크' 자' 이')
		// 크 자 이 x 부(자' 이')
		m_matWorld = Scale * matTrans * matObjScaleInv * matObjWorld;
	}

	if (0 < m_OverlapCount)
	{
		DrawDebugRect(m_matWorld, Vec4(1.f, 0.f, 0.f, 1.f), false, 0.f);
	}
	else if(0 == m_OverlapCount)
	{
		DrawDebugRect(m_matWorld, Vec4(0.f, 1.f, 0.f, 1.f), false, 0.f);
	}
	else {
		assert(nullptr);
	}
}

void CCollider2D::BeginOverlap(CCollider2D* _Other)
{
	++m_OverlapCount;

	for (size_t i = 0; i < m_vecDelegate.size(); ++i)
	{
		if (m_vecDelegate[i].OverlapInst && m_vecDelegate[i].BeginOverlap)
		{
			(m_vecDelegate[i].OverlapInst->*m_vecDelegate[i].BeginOverlap)(this, _Other->GetOwner(), _Other);
		}
	}
}

void CCollider2D::Overlap(CCollider2D* _Other)
{
	for (size_t i = 0; i < m_vecDelegate.size(); ++i)
	{
		if (m_vecDelegate[i].OverlapInst && m_vecDelegate[i].Overlap)
		{
			(m_vecDelegate[i].OverlapInst->*m_vecDelegate[i].Overlap)(this, _Other->GetOwner(), _Other);
		}
	}
}

void CCollider2D::EndOverlap(CCollider2D* _Other)
{
	--m_OverlapCount;

	for (size_t i = 0; i < m_vecDelegate.size(); ++i)
	{
		if (m_vecDelegate[i].OverlapInst && m_vecDelegate[i].EndOverlap)
		{
			(m_vecDelegate[i].OverlapInst->*m_vecDelegate[i].EndOverlap)(this, _Other->GetOwner(), _Other);
		}
	}
}

void CCollider2D::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_Offset, sizeof(Vec3), 1, _File);
	fwrite(&m_Scale, sizeof(Vec3), 1, _File);
	fwrite(&m_IndependentScale, sizeof(bool), 1, _File);
}

void CCollider2D::LoadFromLevelFile(FILE* _File)
{
	fread(&m_Offset, sizeof(Vec3), 1, _File);
	fread(&m_Scale, sizeof(Vec3), 1, _File);
	fread(&m_IndependentScale, sizeof(bool), 1, _File);
}