#include "pch.h"
#include "RenderMgr.h"

#include "AssetMgr.h"

void RenderMgr::Init()
{
	// MRT 생성
	CreateMRT();

	// SwapChain MRT 의 0번 RenderTargetTex 가 SwapChain 타겟이다.
	Ptr<ATexture> RTTex = m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->GetRT(0);

	// PostProcess 용도, RenderTarget 을 복사받을 텍스쳐    
	m_RTCopyTex = AssetMgr::GetInst()->CreateTexture(L"PostProcessTex"
		, RTTex->GetWidth(), RTTex->GetHeight()
		, RTTex->GetFormat(), D3D11_BIND_SHADER_RESOURCE);
	m_RTCopyTex->Binding(15);

	// Debug 렌더링 용도 게임 오브젝트
	m_DebugObj = new GameObject;
	m_DebugObj->AddComponent(new CTransform);
	m_DebugObj->AddComponent(new CMeshRender);	


	// ColorTarget 과 광원MRT 타겟들을 병합해서 SwapChain 에 출력시키는 재질
	Ptr<AGraphicShader> pMergeShader = new AGraphicShader;
	pMergeShader->SetName(L"MergeShader");
	pMergeShader->CreateVertexShader(L"HLSL\\merge.fx", "VS_Merge");
	pMergeShader->CreatePixelShader(L"HLSL\\merge.fx", "PS_Merge");
	pMergeShader->SetRSType(RS_TYPE::CULL_NONE);
	pMergeShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pMergeShader->SetBSType(BS_TYPE::DEFAULT);

	m_MergeMtrl = new AMaterial;
	m_MergeMtrl->SetShader(pMergeShader);
	m_MergeMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_DEBUG);
	m_MergeMtrl->SetTexture(TEX_0, FIND(ATexture, L"ColorTargetTex"));
	m_MergeMtrl->SetTexture(TEX_1, FIND(ATexture, L"DiffuseTargetTex"));
	m_MergeMtrl->SetTexture(TEX_2, FIND(ATexture, L"SpecularTargetTex"));
	m_MergeMtrl->SetTexture(TEX_3, FIND(ATexture, L"EmissiveTargetTex"));


	// Selected 타겟을 화면에 출력시키는 재질 생성
	Ptr<AGraphicShader> pSelectedShader = new AGraphicShader;
	pSelectedShader->SetName(L"SelectedTargetShader");
	pSelectedShader->CreateVertexShader(L"HLSL\\debug.fx", "VS_Selected");
	pSelectedShader->CreatePixelShader(L"HLSL\\debug.fx", "PS_Selected");
	pSelectedShader->SetRSType(RS_TYPE::CULL_NONE);
	pSelectedShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pSelectedShader->SetBSType(BS_TYPE::DEFAULT);
	
	m_SelectedMtrl = new AMaterial;
	m_SelectedMtrl->SetShader(pSelectedShader);
	m_SelectedMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_DEBUG);
}

void RenderMgr::CreateMRT()
{
	Vec2 vResol = Device::GetInst()->GetRenderResolution();

	// =============
	// SwapChain MRT
	// =============
	{
		Ptr<ATexture> RTTex = AssetMgr::GetInst()->Find<ATexture>(L"RenderTargetTex");
		Ptr<ATexture> DSTex = AssetMgr::GetInst()->Find<ATexture>(L"DepthStencilTex");

		Vec4 ClearColor = Vec4(0.f, 0.f, 0.f, 0.f);

		m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN] = new MRT;
		m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->Create(&RTTex, 1, DSTex);
		m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->SetClearColor(&ClearColor);

	}

	// ============
	// Deferred MRT
	// ============
	{
		Ptr<ATexture> arrTex[5] =
		{
			AssetMgr::GetInst()->CreateTexture(L"ColorTargetTex", (UINT)vResol.x, (UINT)vResol.y
				, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			AssetMgr::GetInst()->CreateTexture(L"NormalTargetTex", (UINT)vResol.x, (UINT)vResol.y
				, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			AssetMgr::GetInst()->CreateTexture(L"PositionTargetTex", (UINT)vResol.x, (UINT)vResol.y
				, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			AssetMgr::GetInst()->CreateTexture(L"EmissiveTargetTex", (UINT)vResol.x, (UINT)vResol.y
				, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			AssetMgr::GetInst()->CreateTexture(L"DataTargetTex", (UINT)vResol.x, (UINT)vResol.y
				, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		};

		Vec4 ClearColor[5] =
		{
			Vec4(0.f, 0.f, 0.f, 0.f),
			Vec4(0.f, 0.f, 0.f, 0.f),
			Vec4(0.f, 0.f, 0.f, 0.f),
			Vec4(0.f, 0.f, 0.f, 0.f),
			Vec4(0.f, 0.f, 0.f, 0.f),
		};

		Ptr<ATexture> DSTex = AssetMgr::GetInst()->Find<ATexture>(L"DepthStencilTex");
		
		m_arrMRT[(UINT)MRT_TYPE::DEFERRED] = new MRT;
		m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->Create(arrTex, 5, DSTex);
		m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->SetClearColor(ClearColor);
	}


	// =========
	// Light MRT
	// =========
	{
		Ptr<ATexture> arrTex[2] =
		{
			AssetMgr::GetInst()->CreateTexture(L"DiffuseTargetTex", (UINT)vResol.x, (UINT)vResol.y
				, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			AssetMgr::GetInst()->CreateTexture(L"SpecularTargetTex", (UINT)vResol.x, (UINT)vResol.y
				, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		};

		Vec4 ClearColor[2] =
		{
			Vec4(0.f, 0.f, 0.f, 0.f),
			Vec4(0.f, 0.f, 0.f, 0.f),
		};

		Ptr<ATexture> DSTex = AssetMgr::GetInst()->Find<ATexture>(L"DepthStencilTex");

		m_arrMRT[(UINT)MRT_TYPE::LIGHT] = new MRT;
		m_arrMRT[(UINT)MRT_TYPE::LIGHT]->Create(arrTex, 2, nullptr);
		m_arrMRT[(UINT)MRT_TYPE::LIGHT]->SetClearColor(ClearColor);


		// Light 단계에서 사용할 쉐이더와 재질
		Ptr<AGraphicShader> pDirLightShader = new AGraphicShader;
		pDirLightShader->SetName(L"DirLightShader");
		pDirLightShader->CreateVertexShader(L"HLSL\\light.fx", "VS_DirLight");
		pDirLightShader->CreatePixelShader(L"HLSL\\light.fx", "PS_DirLight");
		pDirLightShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
		pDirLightShader->SetBSType(BS_TYPE::ONE_ONE); // 빛이 타겟에 누적되어야 하기 때문에
		pDirLightShader->SetRSType(RS_TYPE::CULL_NONE);

		Ptr<AMaterial> pDirLightMtrl = new AMaterial;
		pDirLightMtrl->SetShader(pDirLightShader);
		pDirLightMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_LIGHT);
		pDirLightMtrl->SetTexture(TEX_0, FIND(ATexture, L"PositionTargetTex"));
		pDirLightMtrl->SetTexture(TEX_1, FIND(ATexture, L"NormalTargetTex"));

		AssetMgr::GetInst()->AddAsset(L"DirLightMtrl", pDirLightMtrl.Get());


		// Light 단계에서 사용할 쉐이더와 재질
		Ptr<AGraphicShader> pPointLightShader = new AGraphicShader;
		pPointLightShader->SetName(L"PointLightShader");
		pPointLightShader->CreateVertexShader(L"HLSL\\light.fx", "VS_PointLight");
		pPointLightShader->CreatePixelShader(L"HLSL\\light.fx", "PS_PointLight");
		pPointLightShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
		pPointLightShader->SetBSType(BS_TYPE::ONE_ONE); // 빛이 타겟에 누적되어야 하기 때문에
		pPointLightShader->SetRSType(RS_TYPE::CULL_FRONT); // 카메라가 광원 범위 볼륨메쉬 안으로 진입한 상황에도 문제가 없기 위해서

		Ptr<AMaterial> pPointLightMtrl = new AMaterial;
		pPointLightMtrl->SetShader(pPointLightShader);
		pPointLightMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_LIGHT);
		pPointLightMtrl->SetTexture(TEX_0, FIND(ATexture, L"PositionTargetTex"));
		pPointLightMtrl->SetTexture(TEX_1, FIND(ATexture, L"NormalTargetTex"));

		AssetMgr::GetInst()->AddAsset(L"PointLightMtrl", pPointLightMtrl.Get());
	}

	// =========
	// Decal MRT
	// =========
	{
		Ptr<ATexture> arrTex[2] =
		{
			FIND(ATexture, L"ColorTargetTex"),
			FIND(ATexture, L"EmissiveTargetTex"),
		};

		Vec4 ClearColor[2] =
		{
			Vec4(0.f, 0.f, 0.f, 0.f),
			Vec4(0.f, 0.f, 0.f, 0.f),
		};
		
		m_arrMRT[(UINT)MRT_TYPE::DECAL] = new MRT;
		m_arrMRT[(UINT)MRT_TYPE::DECAL]->Create(arrTex, 2, nullptr);
		m_arrMRT[(UINT)MRT_TYPE::DECAL]->SetClearColor(ClearColor);
	}
}