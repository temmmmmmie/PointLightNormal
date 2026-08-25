#include "pch.h"
#include "CLight3D.h"

#include "CTransform.h"
#include "RenderMgr.h"

#include "ATexture.h"
#include "AMaterial.h"
#include "MRT.h"

CLight3D::CLight3D()
	: Component(COMPONENT_TYPE::LIGHT3D)
	, m_LightIdx(-1)
{
	m_CamObject = new GameObject;
	m_CamObject->SetName(L"Light Camera");
	m_CamObject->AddComponent(new CTransform);
	m_CamObject->AddComponent(new CCamera);
}

CLight3D::CLight3D(const CLight3D& _Other)
	: Component(_Other)
	, m_Info(_Other.m_Info)
	, m_LightIdx(-1)
	, m_CamObject(nullptr)
{
	SetLightType(m_Info.Type);
	m_CamObject = _Other.m_CamObject->Clone();
}

CLight3D::~CLight3D()
{
}


void CLight3D::FinalTick()
{
	// 광원 위치정보 갱신
	m_Info.WorldPos = Transform()->GetWorldPos();

	// 광원의 방향정보 갱신
	m_Info.Direction = Transform()->GetWorldDir(DIR::FRONT);

	// 광원의 영향 범위를 Debuerendering 으로 표시해줌
	if (m_Info.Type == LIGHT_TYPE::DIRECTIONAL)
	{
		DrawDebugCube(Transform()->GetWorldPos(), Vec3(10.f, 10.f, 200.f), Transform()->GetRelativeRot(), Vec4(0.2f, 0.8f, 0.2f, 1.f), false, 0.f);
	}
	else if (m_Info.Type == LIGHT_TYPE::POINT)
	{
		//DrawDebugSphere(Transform()->GetWorldPos()
			//, m_Info.Radius, Vec4(0.2f, 0.8f, 0.2f, 1.f), true, 0.f);
	}	

	// 광원 등록
	m_LightIdx = RenderMgr::GetInst()->RegisterLight3D(this);

	// 광원 전용 카메라 업데이트
	if (nullptr != m_CamObject)
	{
		*m_CamObject->Transform() = *Transform();		
		m_CamObject->Transform()->FinalTick();
		m_CamObject->Camera()->FinalTick();
	}	
}

void CLight3D::Render()
{		
	// 광원의 시점에서 물체까지의 깊이정보를 렌더링
	// 광원카메라의 View, Proj 으로 g_Tras 를 변경하게 된다.
	Render_ShadowMap();


	// 다시 Main 카메라 시점으로 돌아와서 빛을 넣어준다.
	// LightMRT 로 변경해서 빛을 그려준다.
	RenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT)->OMSet();
		
	

	m_LightMtrl->SetScalar(INT_0, m_LightIdx);

	if (m_Info.Type == LIGHT_TYPE::DIRECTIONAL)
		m_LightMtrl->SetTexture(TEX_2, m_ShadowMapMRT->GetRT(0)); // ShadowMap 바인딩
	else
		m_LightMtrl->SetTexture(TEXCUBE_0, m_ShadowMapMRT->GetRT(0)); // ShadowMap 바인딩
		m_LightMtrl->SetScalar(VEC4_0, Transform()->GetWorldPos());
		m_LightMtrl->SetScalar(FLOAT_0, m_Info.Radius);

	m_LightMtrl->SetScalar(MAT_0, m_CamObject->Camera()->GetViewMat());
	m_LightMtrl->SetScalar(MAT_1, m_CamObject->Camera()->GetProjMat());	

	m_LightMtrl->Binding();
	m_VolumeMesh->Render(0);
}

void CLight3D::Render_ShadowMap()
{
	// 일단 DirLight 만 ShadowMap 을 제작한다.
	if (m_Info.Type == LIGHT_TYPE::DIRECTIONAL)
	{
		// Main 카메라 View, Proj 행렬정보
		tTransform temp = g_Trans;

		// 광원은 자신이 보유한 전용 카메라로, 물체들을 자신의 시점에서 그린다(ShadowMap)
		// 광원 시점에서 렌더링할 물체들을 분류한다.
		m_CamObject->Camera()->SortObject_ShadowMap();
		
		// 이전 프레임 쉐도우맵 정보를 Clear 한다.
		m_ShadowMapMRT->Clear();

		// 광원 전용 MRT 에 물체들의 깊이정보를 그린다(ShadowMap)	
		m_ShadowMapMRT->OMSet();
		m_CamObject->Camera()->Render_ShadowMap(FIND(AMaterial, L"ShadowMapMtrl"));

		// 다시 Main 카메라 시점 View, Proj 으로 원상복구
		g_Trans = temp;

		// DirLight 인 경우, View, Proj 을 다시 상수버퍼에 보내는데 의미
		// Point or Spot 인 경우 View, Proj 을 다시 상수버퍼에 보내는것 뿐만 아니라 
		// VolumeMesh 를 자신에 위치에 배치시키기 위해서 World 행렬도 세팅했어야 함
		Transform()->Binding();
	}
	else
	{
		// Main 카메라 View, Proj 행렬정보
		tTransform temp = g_Trans;

		// 광원은 자신이 보유한 전용 카메라로, 물체들을 자신의 시점에서 그린다(ShadowMap)
		// 광원 시점에서 렌더링할 물체들을 분류한다.
		m_CamObject->Camera()->SortObject_ShadowMap();

		// 이전 프레임 쉐도우맵 정보를 Clear 한다.
		for (size_t i = 0; i < 6; i++) m_ShadowCubeMapMRT[i]->Clear();
		m_ShadowMapMRT->Clear();
		Matrix mat[6];
		SetMat(mat);
		Ptr <AMaterial> Cubemat = FIND(AMaterial, L"ShadowCubeMapMtrl");
		Cubemat->SetScalar(VEC4_0, Transform()->GetWorldPos());
		Cubemat->SetScalar(FLOAT_0, m_Info.Radius);

		for (size_t i = 0; i < 6; i++)
		{
			Cubemat->SetShadowMat(mat[i]);
			// 광원 전용 MRT 에 물체들의 깊이정보를 그린다(ShadowMap)	
			m_ShadowCubeMapMRT[i]->OMSet();

			m_CamObject->Camera()->Render_ShadowMap(Cubemat);
			UINT dstSubresource = D3D11CalcSubresource(0, i, 1);

			// 개별 텍스처(pSrcTex[i])의 0번 서브리소스를 큐브맵의 i번째 서브리소스(면)로 복사
			CONTEXT->CopySubresourceRegion(
				m_ShadowMapMRT->GetRT(0)->GetTex2D().Get(), dstSubresource, 0, 0, 0,
				m_ShadowCubeMapMRT[i]->GetRT(0)->GetTex2D().Get(), 0, nullptr
			);
			m_ShadowCubeMapMRT[i]->Clear();
			

		}
		// 다시 Main 카메라 시점 View, Proj 으로 원상복구
		g_Trans = temp;

		// DirLight 인 경우, View, Proj 을 다시 상수버퍼에 보내는데 의미
		// Point or Spot 인 경우 View, Proj 을 다시 상수버퍼에 보내는것 뿐만 아니라 
		// VolumeMesh 를 자신에 위치에 배치시키기 위해서 World 행렬도 세팅했어야 함
		Transform()->Binding();

	}
}

void CLight3D::SetMat(Matrix* _mat)
{
	//뷰 변환행렬

	Vec3 vEyePt = Transform()->GetWorldPos(); //시점
	Matrix mCubeMapView[6];   //큐브 텍스처의 각 서피스용 뷰 변환행렬
	Vec3 vLookDir;
	Vec3 vUpDir;
	Vec3 vRightDir;

	// +X축 면
	vLookDir = Vec3(1.f, 0.f, 0.f);
	vUpDir = Vec3(0.f, 1.f, 0.f);
	vRightDir = vUpDir.Cross(vLookDir).Normalize();
	mCubeMapView[0] = Calmat(vRightDir, vUpDir, vLookDir);

	// -X축 면
	vLookDir = Vec3 (- 1.f, 0.f, 0.f);
	vUpDir = Vec3(0.f, 1.f, 0.f);
	vRightDir = vUpDir.Cross(vLookDir).Normalize();
	mCubeMapView[1] = Calmat(vRightDir, vUpDir, vLookDir);

	// +Y축 면
	vLookDir = Vec3(0.f, 1.f, 0.f);
	vUpDir = Vec3(0.f, 0.f, -1.f);
	vRightDir = vUpDir.Cross(vLookDir).Normalize();
	mCubeMapView[2] = Calmat(vRightDir, vUpDir, vLookDir);

	// -Y축 면
	vLookDir = Vec3(0.f, -1.f, 0.f);
	vUpDir = Vec3(0.f, 0.f, 1.f);
	vRightDir = vUpDir.Cross(vLookDir).Normalize();
	mCubeMapView[3] = Calmat(vRightDir, vUpDir, vLookDir);

	// +Z축 면
	vLookDir = Vec3(0.f, 0.f, 1.f);
	vUpDir = Vec3(0.f, 1.f, 0.f);
	vRightDir = vUpDir.Cross(vLookDir).Normalize();
	mCubeMapView[4] = Calmat(vRightDir, vUpDir, vLookDir);

	// -Z축 면
	vLookDir = Vec3(0.f, 0.f, -1.f);
	vUpDir = Vec3(0.f, 1.f, 0.f);
	vRightDir = vUpDir.Cross(vLookDir).Normalize();
	mCubeMapView[5] = Calmat(vRightDir, vUpDir, vLookDir);

	//프로젝션
	Matrix mProj = XMMatrixPerspectiveFovLH(XM_PI / 2.f, 1, 1.f, 1000);

	for (size_t i = 0; i < 6; i++)
	{
		_mat[i] = mCubeMapView[i] * mProj;
	}

}

Matrix CLight3D::Calmat(Vec3 vR, Vec3 vU, Vec3 vF)
{
	Vec3 WorldPos = Transform()->GetRelativePos();
	Matrix matTrans = XMMatrixTranslation(-WorldPos.x, -WorldPos.y, -WorldPos.z);
	Matrix matViewRot = XMMatrixIdentity();

	matViewRot._11 = vR.x;	matViewRot._12 = vU.x;	matViewRot._13 = vF.x;
	matViewRot._21 = vR.y;	matViewRot._22 = vU.y;	matViewRot._23 = vF.y;
	matViewRot._31 = vR.z;	matViewRot._32 = vU.z;	matViewRot._33 = vF.z;

	return matTrans * matViewRot;
}

void CLight3D::SetLightType(LIGHT_TYPE _Type)
{
	m_Info.Type = _Type;

	if (LIGHT_TYPE::DIRECTIONAL == m_Info.Type)
	{
		m_VolumeMesh = FIND(AMesh, L"RectMesh");
		m_LightMtrl = FIND(AMaterial, L"DirLightMtrl");

		// 광원이 보유한 카메라 설정
		m_CamObject->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);		
		m_CamObject->Camera()->SetFar(100000.f);		
		m_CamObject->Camera()->SetWidth(8192.f);
		m_CamObject->Camera()->SetAspectRatio(1.f);
		m_CamObject->Camera()->LayerCheckAll();
		m_CamObject->Camera()->LayerCheck(31);


		// 광원시점에서 물체들을 렌더링 해서 만들 쉐도우맵 
		Ptr<ATexture> pRT = new ATexture;
		pRT->Create(8192, 8192, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT);

		Ptr<ATexture> pDT = new ATexture;
		pDT->Create(8192, 8192, DXGI_FORMAT_D32_FLOAT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT);

		m_ShadowMapMRT = new MRT;
		m_ShadowMapMRT->Create(&pRT, 1, pDT);
	}
	else if (LIGHT_TYPE::POINT == m_Info.Type)
	{
		m_VolumeMesh = FIND(AMesh, L"SphereMesh");
		m_LightMtrl = FIND(AMaterial, L"PointLightMtrl");

		// 광원이 보유한 카메라 설정
		m_CamObject->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
		m_CamObject->Camera()->SetFar(100000.f);
		m_CamObject->Camera()->SetWidth(8192.f);
		m_CamObject->Camera()->SetAspectRatio(1.f);
		m_CamObject->Camera()->LayerCheckAll();
		m_CamObject->Camera()->LayerCheck(31);

		Ptr<ATexture> pDT = new ATexture;
		Ptr<ATexture> pRT = new ATexture;
		pDT->Create(2048, 2048, DXGI_FORMAT_D32_FLOAT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT);

		for (size_t i = 0; i < 6; i++)
		{
			pRT->Create(2048, 2048, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT);

			Ptr<MRT> pMRT = new MRT;
			pMRT->Create(&pRT, 1, pDT);

			m_ShadowCubeMapMRT[i] = pMRT;
		}

		Ptr<ATexture> pcubemapDT = new ATexture;
		Ptr<ATexture> pcubemapRT = new ATexture;
		pcubemapDT->CreateCubeMap(2048, 2048, DXGI_FORMAT_D32_FLOAT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT);
		pcubemapRT->CreateCubeMap(2048, 2048, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT);

		m_ShadowMapMRT = new MRT;
		m_ShadowMapMRT->Create(&pcubemapRT, 1, pcubemapDT);

	}
	else if (LIGHT_TYPE::SPOT == m_Info.Type)
	{
		m_VolumeMesh = FIND(AMesh, L"ConeMesh");
	}
}

void CLight3D::SetRadius(float _Radius)
{
	m_Info.Radius = _Radius;
	Transform()->SetRelativeScale(Vec3(_Radius * 2.f, _Radius * 2.f, _Radius * 2.f));
}



void CLight3D::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_Info, sizeof(tLightInfo), 1, _File);
}

void CLight3D::LoadFromLevelFile(FILE* _File)
{
	fread(&m_Info, sizeof(tLightInfo), 1, _File);
}

void CLight3D::SaveShadowMap()
{
	m_ShadowMapMRT->GetRT(0)->Save(PathMgr::GetInst()->GetContentPath() + L"Texture\\Cubemap\\Cube.dds");


}
