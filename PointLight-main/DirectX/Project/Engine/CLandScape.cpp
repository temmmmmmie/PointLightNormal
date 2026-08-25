#include "pch.h"
#include "CLandScape.h"

#include "RenderMgr.h"

CLandScape::CLandScape()
	: CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
	, m_FaceX(1)
	, m_FaceZ(1)
	, m_BrushScale(Vec2(0.2f, 0.2f))
{
	CreateMesh();
	CreateMaterial();
	CreateComputeShader();

	// 높이를 올릴때 사용할 BrushTexture
	m_BrushTex = LOAD(ATexture, L"Texture\\brush\\TX_Twirl02.png");

	// Raycasting 결과를 받는 용도의 구조화버퍼
	m_RaycastOut = new StructuredBuffer;
	m_RaycastOut->Create(sizeof(tRaycastOut), 1, SB_TYPE::SRV_UAV, true);
}

CLandScape::~CLandScape()
{
}

void CLandScape::SetFace(UINT _FaceX, UINT _FaceZ)
{
	if (m_FaceX == _FaceX && m_FaceZ == _FaceZ)
		return;

	m_FaceX = _FaceX;
	m_FaceZ = _FaceZ;

	// 메쉬를 새로 만들어서 설정	
	CreateMesh();

	// 기존에 메시에 매칭되던 재질도 재설정 해야하기 때문에, 다시 재질 설정
	CreateMaterial();
}

void CLandScape::FinalTick()
{
	if (KEY_PRESSED(KEY::LBTN))
	{
		Raycasting();

		if (nullptr != m_HeightMap)
		{
			// 높이맵 설정
			m_HeightMapCS->SetBrushPos(m_RaycastOut);
			m_HeightMapCS->SetBrushScale(m_BrushScale);
			m_HeightMapCS->SetHeightMap(m_HeightMap);
			m_HeightMapCS->SetBrushTex(m_BrushTex);
			m_HeightMapCS->Execute();
		}
	}
}

void CLandScape::Render()
{
	GetMaterial(0)->GetShader()->SetRSType(RS_TYPE::WIRE_FRAME);


	GetMaterial(0)->SetScalar(INT_0, m_FaceX);
	GetMaterial(0)->SetScalar(INT_1, m_FaceZ);
	GetMaterial(0)->SetTexture(TEX_0, m_HeightMap);
	GetMaterial(0)->Binding();

	GetMesh()->Render(0);
}

void CLandScape::CreateMesh()
{
	vector<Vtx> vecVtx;
	Vtx v;

	for (int Row = 0; Row < m_FaceZ + 1; ++Row)
	{
		for (int Col = 0; Col < m_FaceX + 1; ++Col)
		{
			v.vPos = Vec3((float)Col, 0.f, (float)Row);

			v.vTangent = Vec3(1.f, 0.f, 0.f);
			v.vNormal = Vec3(0.f, 1.f, 0.f);
			v.vBinormal = Vec3(0.f, 0.f, -1.f);

			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
			v.vUV = Vec2((float)Col, (float)(m_FaceZ - Row));

			vecVtx.push_back(v);
		}
	}

	vector<UINT> vecIdx;
	for (int Row = 0; Row < m_FaceZ; ++Row)
	{
		for (int Col = 0; Col < m_FaceX; ++Col)
		{
			// 0
			// | \
			// 2--1 
			vecIdx.push_back(Row * (m_FaceX + 1) + (m_FaceX + 1) + Col);
			vecIdx.push_back(Row * (m_FaceX + 1) + 1 + Col);
			vecIdx.push_back(Row * (m_FaceX + 1) + Col);

			// 1--2
			//  \ |
			//    0
			vecIdx.push_back(Row * (m_FaceX + 1) + 1 + Col);
			vecIdx.push_back(Row * (m_FaceX + 1) + (m_FaceX + 1) + Col);
			vecIdx.push_back(Row * (m_FaceX + 1) + (m_FaceX + 1) + 1 + Col);
		}
	}

	Ptr<AMesh> pMesh = new AMesh;
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	SetMesh(pMesh);
}

void CLandScape::CreateMaterial()
{
	wstring strMtrlKey = L"LandScapeMtrl";
	wstring strShaderKey = L"LandScapeShader";

	Ptr<AMaterial> pMtrl = AssetMgr::GetInst()->Find<AMaterial>(strMtrlKey);

	if (nullptr == pMtrl)
	{
		Ptr<AGraphicShader> pShader = AssetMgr::GetInst()->Find<AGraphicShader>(strShaderKey);

		if (nullptr == pShader)
		{
			pShader = new AGraphicShader;
			pShader->CreateVertexShader(L"HLSL\\landscape.fx", "VS_LandScape");
			pShader->CreateHullShader(L"HLSL\\landscape.fx", "HS_LandScape");
			pShader->CreateDomainShader(L"HLSL\\landscape.fx", "DS_LandScape");
			pShader->CreatePixelShader(L"HLSL\\landscape.fx", "PS_LandScape");
			pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

			pShader->SetRSType(RS_TYPE::CULL_BACK);
			pShader->SetDSType(DS_TYPE::LESS);
			pShader->SetBSType(BS_TYPE::DEFAULT);

			pShader->AddShaderParam(SHADER_PARAM::FLOAT, 0, L"TessLevel", false, 0.1f);

			AssetMgr::GetInst()->AddAsset(strShaderKey, pShader.Get());
		}

		pMtrl = new AMaterial;
		pMtrl->SetShader(pShader);
		pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_DEFERRED);
		AssetMgr::GetInst()->AddAsset(strMtrlKey, pMtrl.Get());
	}

	SetMaterial(pMtrl, 0);
}


int CLandScape::Raycasting()
{
	// 현재 시점 카메라 가져오기
	Ptr<CCamera> pCam = RenderMgr::GetInst()->GetPOVCamera();
	if (nullptr == pCam)
		return false;

	// 구조화버퍼 클리어
	m_Out = {};
	m_Out.Distance = 0xffffffff;
	m_RaycastOut->SetData(&m_Out);

	// 카메라가 시점에서 마우스를 향하는 Ray 정보를 가져옴
	tRay ray = pCam->GetRay();

	// LandScape 의 WorldInv 행렬 가져옴
	const Matrix& matWorldInv = Transform()->GetmatWorldInv();

	// 월드 기준 Ray 정보를 LandScape 의 Local 공간으로 데려감
	ray.vStart = XMVector3TransformCoord(ray.vStart, matWorldInv);
	ray.vDir = XMVector3TransformNormal(ray.vDir, matWorldInv);
	ray.vDir.Normalize();

	// Raycast 컴퓨트 쉐이더에 필요한 데이터 전달
	m_RaycastCS->SetRayInfo(ray);
	m_RaycastCS->SetFace(m_FaceX, m_FaceZ);
	m_RaycastCS->SetOutBuffer(m_RaycastOut);
	m_RaycastCS->SetHeightMap(m_HeightMap);

	// 컴퓨트쉐이더 실행
	m_RaycastCS->Execute();

	// 결과 확인
	m_RaycastOut->GetData(&m_Out);

	// 결과 확인
	if (m_Out.Success)
	{
		m_Out.Location;
		int a = 0;
	}

	return m_Out.Success;
}


void CLandScape::CreateComputeShader()
{
	m_RaycastCS = new ARaycastCS;

	m_HeightMapCS = new AHeightMapCS;
}

void CLandScape::CreateHeightMap(UINT _Width, UINT _Height)
{
	m_HeightMap = new ATexture;
	m_HeightMap->Create(_Width, _Height, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, D3D11_USAGE_DEFAULT);
}