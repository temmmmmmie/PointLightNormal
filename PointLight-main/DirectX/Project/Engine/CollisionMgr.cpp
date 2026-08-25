#include "pch.h"
#include "CollisionMgr.h"


#include "AssetMgr.h"
#include "AMesh.h"

#include "LevelMgr.h"
#include "Level.h"
#include "Layer.h"
#include "GameObject.h"

CollisionMgr::CollisionMgr()
	: m_Matrix{}
{

}

CollisionMgr::~CollisionMgr()
{

}

void CollisionMgr::Progress()
{
	for (UINT Row = 0; Row < MAX_LAYER; ++Row)
	{
		for (UINT Col = Row; Col < MAX_LAYER; ++Col)
		{
			if (m_Matrix[Row] & (1 << Col))
			{
				CollisionBtwLayer(Row, Col);
			}
		}
	}
}

void CollisionMgr::CollisionBtwLayer(UINT _Left, UINT _Right)
{
	Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurrentLevel();

	Ptr<Layer> pLeftLayer = pCurLevel->GetLayer(_Left);
	Ptr<Layer> pRightLayer = pCurLevel->GetLayer(_Right);

	const vector<Ptr<GameObject>>& vecLeft = pLeftLayer->GetObjects();
	const vector<Ptr<GameObject>>& vecRight = pRightLayer->GetObjects();

	for (size_t i = 0; i < vecLeft.size(); ++i)
	{
		if (nullptr == vecLeft[i]->Collider2D())
			continue;

		for (size_t j = 0; j < vecRight.size(); ++j)
		{
			if (nullptr == vecRight[j]->Collider2D())
				continue;

			COLLIDER_ID ColID = {};
			ColID.LEFT = vecLeft[i]->Collider2D()->GetID();
			ColID.Right = vecRight[j]->Collider2D()->GetID();

			map<LONGLONG, bool>::iterator iter = m_mapColInfo.find(ColID.ID);
			if (iter == m_mapColInfo.end())
			{
				m_mapColInfo.insert(make_pair(ColID.ID, false));
				iter = m_mapColInfo.find(ColID.ID);
			}
			

			bool IsDead = vecLeft[i]->IsDead() || vecRight[j]->IsDead();


			if (IsOverlap(vecLeft[i]->Collider2D(), vecRight[j]->Collider2D()))
			{
				if (iter->second)
				{
					if (IsDead)
					{
						vecLeft[i]->Collider2D()->EndOverlap(vecRight[j]->Collider2D());
						vecRight[j]->Collider2D()->EndOverlap(vecLeft[i]->Collider2D());
					}
					else
					{
						vecLeft[i]->Collider2D()->Overlap(vecRight[j]->Collider2D());
						vecRight[j]->Collider2D()->Overlap(vecLeft[i]->Collider2D());
					}
				}
				else
				{
					if (!IsDead)
					{
						vecLeft[i]->Collider2D()->BeginOverlap(vecRight[j]->Collider2D());
						vecRight[j]->Collider2D()->BeginOverlap(vecLeft[i]->Collider2D());
					}
				}

				iter->second = true;
			}
			else
			{
				if (iter->second)
				{
					vecLeft[i]->Collider2D()->EndOverlap(vecRight[j]->Collider2D());
					vecRight[j]->Collider2D()->EndOverlap(vecLeft[i]->Collider2D());
				}

				iter->second = false;
			}						
		}
	}
}

bool CollisionMgr::IsOverlap(CCollider2D* _LeftCol, CCollider2D* _RightCol)
{
	// 두 충돌체의 모서리(표면) 방향벡터를 가져온다.
	const Matrix& matLeftWorld = _LeftCol->GetWorldMat();
	const Matrix& matRightWorld = _RightCol->GetWorldMat();

	// 사각형 충돌체의 기본형태 도형을 가져온다.
	Ptr<AMesh> pRectMesh = AssetMgr::GetInst()->Find<AMesh>(L"RectMesh");
	Vtx* pVtx = pRectMesh->GetVtxSysMem();
		
	// 사각형 도형을 각 충돌체 꼭지점 위치로 보내서, 각 충돌체의 표면 방향벡터 4개를 구한다.
	Vec3 vProj[4] = {};
	vProj[0] = XMVector3TransformCoord(pVtx[1].vPos, matLeftWorld) - XMVector3TransformCoord(pVtx[0].vPos, matLeftWorld);
	vProj[1] = XMVector3TransformCoord(pVtx[2].vPos, matLeftWorld) - XMVector3TransformCoord(pVtx[1].vPos, matLeftWorld);
	vProj[2] = XMVector3TransformCoord(pVtx[1].vPos, matRightWorld) - XMVector3TransformCoord(pVtx[0].vPos, matRightWorld);
	vProj[3] = XMVector3TransformCoord(pVtx[2].vPos, matRightWorld) - XMVector3TransformCoord(pVtx[1].vPos, matRightWorld);
	 
	vProj[0].z = 0.f;
	vProj[1].z = 0.f;
	vProj[2].z = 0.f;
	vProj[3].z = 0.f;

	// 두 충돌체 중심을 잇는 방향벡터를 구한다.
	Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matLeftWorld) - XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matRightWorld);
	vCenter.z = 0;

	for (int i = 0; i < 4; ++i)
	{
		// 투영 축
		Vec3 vProjAxis = vProj[i];
		vProjAxis.Normalize();

		// 투영축으로 나머지 표면벡터를 투영시킨 거리 전체합 절반
		float AxisSum = 0.f;
		for (int j = 0; j < 4; ++j)
		{				
			AxisSum += fabs(vProjAxis.Dot(vProj[j]));
		}
		AxisSum /= 2.f;

		// 중심을 이은 Center 벡터를 투영축으로 투영시킨 거리
		float fCenter = fabs(vProjAxis.Dot(vCenter));

		// 중심거리가 면적 절반합보다 크면, 투 충돌체가 벌어져있다.
		if (AxisSum < fCenter)
			return false;
	}

	return true;
}






void CollisionMgr::CollisionCheck(UINT _LeftLayer, UINT _RightLayer)
{
	UINT Row = _LeftLayer;
	UINT Col = _RightLayer;

	if (Col < Row)
	{
		Row = _RightLayer;
		Col = _LeftLayer;
	}

	m_Matrix[Row] ^= (1 << Col);
}

void CollisionMgr::CollisionCheckClear()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_Matrix[i] = 0;
	}
}
