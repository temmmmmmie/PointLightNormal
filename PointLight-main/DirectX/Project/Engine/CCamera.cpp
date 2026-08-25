#include "pch.h"
#include "CCamera.h"

#include "Device.h"
#include "RenderMgr.h"
#include "LevelMgr.h"
#include "Level.h"
#include "GameObject.h"
#include "CTransform.h"

CCamera::CCamera()
	: Component(COMPONENT_TYPE::CAMERA)
	, m_Frustum(this)
	, m_ProjType(PROJ_TYPE::ORTHOGRAPHIC)
	, m_OrthoScale(1.f)
	, m_AspectRatio(1.f)
	, m_Width(100)
	, m_Far(10000.f)
	, m_FOV(XM_PI / 2.f)
	, m_Priority(-1)
	, m_LayerCheck(0)
	, m_Ray{}
{
	Vec2 vRenderResol = Device::GetInst()->GetRenderResolution();
	m_Width = vRenderResol.x;
	m_AspectRatio = vRenderResol.x / vRenderResol.y;	
}

CCamera::CCamera(const CCamera& _Origin)
	: Component(_Origin)
	, m_Frustum(this)
	, m_ProjType(_Origin.m_ProjType)
	, m_OrthoScale(_Origin.m_OrthoScale)
	, m_Width(_Origin.m_Width)
	, m_AspectRatio(_Origin.m_AspectRatio)
	, m_FOV(_Origin.m_FOV)
	, m_Far(_Origin.m_Far)
	, m_Priority(_Origin.m_Priority)
	, m_LayerCheck(_Origin.m_LayerCheck)
	, m_Ray{}
{
}

CCamera::~CCamera()
{
}

void CCamera::Begin()
{
	if (-1 != m_Priority)
		RenderMgr::GetInst()->RegisterCamera(this, m_Priority);
}

void CCamera::FinalTick()
{
	// View 행렬 계산
	// 이동 x 회전(공전)
	// ViewSpace - 카메라가 기준이 되는 좌표계
	// 1. 카메라가 원점에 존재한다.
	// 2. 카메라가 바라보는 방향이 Z축이다.
	Vec3 WorldPos = Transform()->GetRelativePos();
	Matrix matTrans = XMMatrixTranslation(-WorldPos.x, -WorldPos.y, -WorldPos.z);

	Vec3 vR = Transform()->GetWorldDir(DIR::RIGHT);
	Vec3 vU = Transform()->GetWorldDir(DIR::UP);
	Vec3 vF = Transform()->GetWorldDir(DIR::FRONT);

	// 1행 vR
	// 2행 vU
	// 3행 vF
	// 으로 구성된 행렬의 역행렬이 View 회전행렬이다.

	// vR.x, vR.y, vR.z				  (1 0 0)
	// vU.x, vU.y, vU.z	 x	(vRot)  = (0 1 0)
	// vF.x, vF.y, vF.z				  (0 0 1)

	// 직교 행렬
	// 행렬 구성하고 있는 각 요소가, 행 단위로, 서로가 수직상태인 행렬
	// 직교행렬의 역행렬은 전치행렬이다.

	// vRot
	// vR.x, vU.x, vF.x
	// vR.y, vU.y, vF.y
	// vR.z, vU.z, vF.z
	Matrix matViewRot = XMMatrixIdentity();

	matViewRot._11 = vR.x;	matViewRot._12 = vU.x;	matViewRot._13 = vF.x;
	matViewRot._21 = vR.y;	matViewRot._22 = vU.y;	matViewRot._23 = vF.y;
	matViewRot._31 = vR.z;	matViewRot._32 = vU.z;	matViewRot._33 = vF.z;

	m_matView = matTrans * matViewRot;
	m_matViewInv = XMMatrixInverse(nullptr, m_matView);


	// 투영(Projection) 행렬 계산
	// 1. 직교투영(2D 게임)
	if (PROJ_TYPE::ORTHOGRAPHIC == m_ProjType)
	{
		m_matProj = XMMatrixOrthographicLH(m_Width * m_OrthoScale, (m_Width / m_AspectRatio) * m_OrthoScale, 1.f, m_Far);
	}

	// 2. 원근투영(3D 게임)
	else
	{
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, 1.f, m_Far);
	}

	m_matProjInv = XMMatrixInverse(nullptr, m_matProj);

	m_matWorld = Transform()->GetmatWorld();
	m_matWorldInv = Transform()->GetmatWorldInv();


	// 절두체 계산
	m_Frustum.FinalTick();

	// Ray 계산
	CalcRay();
}

void CCamera::Render()
{	
	// 계산한 View, Proj 행렬 업데이트
	g_Trans.matWorld	= m_matWorld;
	g_Trans.matWorldInv = m_matWorldInv;
	g_Trans.matView		= m_matView;
	g_Trans.matViewInv	= m_matViewInv;
	g_Trans.matProj		= m_matProj;
	g_Trans.matProjInv	= m_matProjInv;

	// 현재 레벨에 있는 물체들을, RenderDomain 에 따라 분류를 한다.
	SortObject();

	// Domain 순서대로 렌더링	
	// ==================
	// Deferred Rendering
	// ==================
	// DeferredMRT 로 출력을 교체한다.
	RenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED)->OMSet();
	for (size_t i = 0; i < m_vecDeferred.size(); ++i)
	{
		m_vecDeferred[i]->Render();
	}

	// =====
	// Decal
	// =====
	RenderMgr::GetInst()->GetMRT(MRT_TYPE::DECAL)->OMSet();
	for (size_t i = 0; i < m_vecDecal.size(); ++i)
	{
		m_vecDecal[i]->Render();
	}

	// ================================================
	// Lighting
	// Deferred 단계에서 그려진 물체에, 빛을 씌워주는 작업
	// ================================================
	RenderMgr::GetInst()->Render_Lighting();

	// ==========================================================
	// Merge
	// Deferred 단계에서 그려진 컬러정보와, Lighting 단계에서 생성된
	// 광원 정보를 병합해서, SwapChain 타겟으로 출력한다.
	// ==========================================================
	RenderMgr::GetInst()->Render_Merge();

	// =================
	// Forward Rendering
	// =================
	// SwapchainMRT 로 출력을 교체
	RenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();

	for (size_t i = 0; i < m_vecOpaque.size(); ++i)
	{
		m_vecOpaque[i]->Render();
	}

	for (size_t i = 0; i < m_vecMask.size(); ++i)
	{
		m_vecMask[i]->Render();
	}

	for (size_t i = 0; i < m_vecTransparent.size(); ++i)
	{
		m_vecTransparent[i]->Render();
	}

	for (size_t i = 0; i < m_vecParticles.size(); ++i)
	{
		m_vecParticles[i]->Render();
	}

	// PostProcess
	for (size_t i = 0; i < m_vecPostProcess.size(); ++i)
	{
		RenderMgr::GetInst()->CopyTarget();

		m_vecPostProcess[i]->Render();
	}
}

void CCamera::SortObject()
{
	// 이전프레임 정보 제거
	m_vecDeferred.clear();
	m_vecDecal.clear();
	m_vecOpaque.clear();
	m_vecMask.clear();
	m_vecTransparent.clear();
	m_vecParticles.clear();
	m_vecPostProcess.clear();


	Ptr<ALevel> pLevel = LevelMgr::GetInst()->GetCurrentLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (!(m_LayerCheck & (1 << i)))
			continue;

		Ptr<Layer> pLayer = pLevel->GetLayer(i);
		const vector<Ptr<GameObject>>& vecObjects = pLayer->GetObjects();

		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			if (nullptr == vecObjects[j]->GetRenderCom()
				|| nullptr == vecObjects[j]->GetRenderCom()->GetMesh()
				|| nullptr == vecObjects[j]->GetRenderCom()->GetMaterial(0)
				|| nullptr == vecObjects[j]->GetRenderCom()->GetMaterial(0)->GetShader())
			{
				continue;
			}

			// 절두체 테스트를 실패하면 카메라 시야 밖에 있다는 뜻
			if(vecObjects[j]->GetRenderCom()->IsFrustumCulling() 
			  && !m_Frustum.FrustumSphereCheck(vecObjects[j]->Transform()->GetWorldPos(), vecObjects[j]->GetRenderCom()->GetBoundingBox()))
				continue;

			RENDER_DOMAIN domain = vecObjects[j]->GetRenderCom()->GetMaterial(0)->GetDomain();

			switch (domain)
			{
			case RENDER_DOMAIN::DOMAIN_DEFERRED:
				m_vecDeferred.push_back(vecObjects[j].Get());
				break;
			case RENDER_DOMAIN::DOMAIN_DECAL:
				m_vecDecal.push_back(vecObjects[j].Get());
				break;
			case RENDER_DOMAIN::DOMAIN_OPAQUE:
				m_vecOpaque.push_back(vecObjects[j].Get());
				break;
			case RENDER_DOMAIN::DOMAIN_MASK:
				m_vecMask.push_back(vecObjects[j].Get());
				break;
			case RENDER_DOMAIN::DOMAIN_TRANSPARENT:
				m_vecTransparent.push_back(vecObjects[j].Get());
				break;
			case RENDER_DOMAIN::DOMAIN_PARTICLE:
				m_vecParticles.push_back(vecObjects[j].Get());
				break;
			case RENDER_DOMAIN::DOMAIN_POSTPROCESS:
				m_vecPostProcess.push_back(vecObjects[j].Get());
				break;
			}
		}
	}
}

void CCamera::SortObject_ShadowMap()
{
	m_vecShadowMap.clear();

	Ptr<ALevel> pLevel = LevelMgr::GetInst()->GetCurrentLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (!(m_LayerCheck & (1 << i)))
			continue;

		Ptr<Layer> pLayer = pLevel->GetLayer(i);
		const vector<Ptr<GameObject>>& vecObjects = pLayer->GetObjects();

		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			if (nullptr == vecObjects[j]->GetRenderCom()
				|| nullptr == vecObjects[j]->GetRenderCom()->GetMesh()
				|| nullptr == vecObjects[j]->GetRenderCom()->GetMaterial(0)
				|| nullptr == vecObjects[j]->GetRenderCom()->GetMaterial(0)->GetShader())
			{
				continue;
			}

			if(RENDER_DOMAIN::DOMAIN_DEFERRED == vecObjects[j]->GetRenderCom()->GetMaterial(0)->GetDomain())
				m_vecShadowMap.push_back(vecObjects[j].Get());		
		}
	}
}

void CCamera::Render_ShadowMap(Ptr<AMaterial> _ShadowMapMtrl)
{	
	g_Trans.matWorld = m_matWorld;
	g_Trans.matWorldInv = m_matWorldInv;
	g_Trans.matView = m_matView;
	g_Trans.matViewInv = m_matViewInv;
	g_Trans.matProj = m_matProj;
	g_Trans.matProjInv = m_matProjInv;

	for (size_t i = 0; i < m_vecShadowMap.size(); ++i)
	{
		m_vecShadowMap[i]->Render_ShadowMap(_ShadowMapMtrl);
	}
}

void CCamera::CalcRay()
{
	// ViewPort 정보
	Ptr<MRT> pSwapChainMRT = RenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN);
	if (nullptr == pSwapChainMRT)
		return;

	// 메인 MRT 의 ViewPort 값을 가져온다.
	const D3D11_VIEWPORT& VP = pSwapChainMRT->GetViewPort();

	// 현재 마우스 좌표
	Vec2 vMousePos = KeyMgr::GetInst()->GetMousePos();

	// 마우스를 향하는 직선은 카메라 위치를 지난다.
	m_Ray.vStart = Transform()->GetWorldPos();

	// View 공간 상에서 카메라에서 마우스 방향을 향하는 방향벡터를 구한다.
	//  - 마우스가 있는 좌표를 -1 ~ 1 사이의 정규화된 좌표로 바꾼다.
	//  - 투영행렬의 _11, _22 에 있는 값은 Near 평면상에서 Near 값을 가로 세로 길이로 나눈값
	//  - 실제 ViewSpace 상에서의 Near 평명상에서 마우스가 있는 지점을 향하는 위치를 구하기 위해서 비율을 나누어서 
	//  - 실제 Near 평면상에서 마우스가 향하는 위치를 좌표를 구함
	m_Ray.vDir.x = (((vMousePos.x - VP.TopLeftX) * 2.f / VP.Width) - 1.f) / m_matProj._11;
	m_Ray.vDir.y = -(((vMousePos.y - VP.TopLeftY) * 2.f / VP.Height) - 1.f) / m_matProj._22;
	m_Ray.vDir.z = 1.f;

	// 방향 벡터에 ViewMatInv 를 적용, 월드상에서의 방향을 알아낸다.
	m_Ray.vDir = XMVector3TransformNormal(m_Ray.vDir, m_matViewInv);
	m_Ray.vDir.Normalize();
}


void CCamera::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_ProjType, sizeof(PROJ_TYPE), 1, _File);
	fwrite(&m_OrthoScale, sizeof(float), 1, _File);
	fwrite(&m_Width, sizeof(float), 1, _File);
	fwrite(&m_AspectRatio, sizeof(float), 1, _File);
	fwrite(&m_FOV, sizeof(float), 1, _File);
	fwrite(&m_Far, sizeof(float), 1, _File);
	fwrite(&m_Priority, sizeof(int), 1, _File);
	fwrite(&m_LayerCheck, sizeof(UINT), 1, _File);
}

void CCamera::LoadFromLevelFile(FILE* _File)
{
	fread(&m_ProjType, sizeof(PROJ_TYPE), 1, _File);
	fread(&m_OrthoScale, sizeof(float), 1, _File);
	fread(&m_Width, sizeof(float), 1, _File);
	fread(&m_AspectRatio, sizeof(float), 1, _File);
	fread(&m_FOV, sizeof(float), 1, _File);
	fread(&m_Far, sizeof(float), 1, _File);
	fread(&m_Priority, sizeof(int), 1, _File);
	fread(&m_LayerCheck, sizeof(UINT), 1, _File);
}
