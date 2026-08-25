#include "pch.h"
#include "Frustum.h"

#include "CCamera.h"

Frustum::Frustum(CCamera* _Owner)
	: m_Owner(_Owner)
{	
	//   4 -- 5  Far
	//  /|   /|
	// 0 -- 1 6
	// |    |/
	// 3 -- 2  Near

	m_ProjPos[0] = Vec3(-1.f, 1.f, 0.f);
	m_ProjPos[1] = Vec3(1.f, 1.f, 0.f);
	m_ProjPos[2] = Vec3(1.f, -1.f, 0.f);
	m_ProjPos[3] = Vec3(-1.f, -1.f, 0.f);

	m_ProjPos[4] = Vec3(-1.f, 1.f, 1.f);
	m_ProjPos[5] = Vec3(1.f, 1.f,  1.f);
	m_ProjPos[6] = Vec3(1.f, -1.f, 1.f);
	m_ProjPos[7] = Vec3(-1.f, -1.f,1.f);
}

Frustum::~Frustum()
{
}

bool Frustum::FrustumPointCheck(Vec3 _Point)
{
	for (int i = 0; i < FT_END; ++i)
	{
		// (A, B, C) Dot (_Point)
		// Ax + By + Cz == D
		// N dot P > D
		if (_Point.Dot(m_WorldFace[i]) + m_WorldFace[i].w > 0)
			return false;
	}

	return true;
}

bool Frustum::FrustumSphereCheck(Vec3 _Point, float _Radius)
{
	for (int i = 0; i < FT_END; ++i)
	{
		// (A, B, C) Dot (_Point)
		// Ax + By + Cz == D
		// N dot P > D
		if (_Point.Dot(m_WorldFace[i]) + m_WorldFace[i].w > _Radius)
			return false;
	}

	return true;
}

void Frustum::FinalTick()
{	
	// NDC 좌표계 기준 최대 범위 위치를 역행렬을 곱해서 월드까지 가져옴
	// 월드상에서 카메라가 볼수있는 시야 최대 위치 꼭지점을 구함

	Vec3 vWorldPos[8] = {};

	Matrix matInv = m_Owner->GetProjInvMat() * m_Owner->GetViewInvMat();
	for (int i = 0; i < 8; ++i)
	{
		vWorldPos[i] = XMVector3TransformCoord(m_ProjPos[i], matInv);
	}

	//     Far
	//   4 -- 5  
	//  /|   /|
	// 0 -- 1 6
	// |    |/
	// 3 -- 2 
	//  Near
	m_WorldFace[FT_NEAR] = XMPlaneFromPoints(vWorldPos[0], vWorldPos[1], vWorldPos[2]);
	m_WorldFace[FT_FAR] = XMPlaneFromPoints(vWorldPos[5], vWorldPos[4], vWorldPos[7]);

	m_WorldFace[FT_LEFT] = XMPlaneFromPoints(vWorldPos[4], vWorldPos[0], vWorldPos[7]);
	m_WorldFace[FT_RIGHT] = XMPlaneFromPoints(vWorldPos[1], vWorldPos[5], vWorldPos[6]);

	m_WorldFace[FT_TOP] = XMPlaneFromPoints(vWorldPos[0], vWorldPos[4], vWorldPos[5]);
	m_WorldFace[FT_BOT] = XMPlaneFromPoints(vWorldPos[3], vWorldPos[6], vWorldPos[7]);
}