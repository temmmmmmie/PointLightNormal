#include "pch.h"
#include "CSkyBox.h"

CSkyBox::CSkyBox()
	: CRenderComponent(COMPONENT_TYPE::SKYBOX)
	, m_Type(SKYBOX_TYPE::SPHERE)
{	
	SetSkyBoxType(m_Type);

	CreateMaterial();

	SetFrustumCulling(false);
}

CSkyBox::~CSkyBox()
{
}


void CSkyBox::SetSkyBoxType(SKYBOX_TYPE _Type)
{
	m_Type = _Type;

	if (m_Type == SKYBOX_TYPE::SPHERE)
	{
		SetMesh(FIND(AMesh, L"SphereMesh"));
	}

	else
	{
		SetMesh(FIND(AMesh, L"CubeMesh"));
	}
}

void CSkyBox::SetSkyBoxTexture(Ptr<ATexture> _Tex)
{
	wstring MtrlName = L"SkyBoxMtrl";
	wstring ShaderName = L"SkyBoxShader";
	SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(MtrlName), 0);

	if (_Tex->IsCubeMap())	
		m_SkyCubeTex = _Tex;
	else
		m_SkyTex = _Tex;	
}

void CSkyBox::FinalTick()
{
	int a = 0;
}

void CSkyBox::Render()
{
	Transform()->Binding();

	if (m_Type == SKYBOX_TYPE::SPHERE)
	{
		GetMaterial(0)->SetTexture(TEX_0, m_SkyTex);
	}
	else
	{
		GetMaterial(0)->SetTexture(TEXCUBE_0, m_SkyCubeTex);
	}

	GetMaterial(0)->SetScalar(INT_0, (int)m_Type);
	
	GetMaterial(0)->Binding();

	GetMesh()->Render(0);
}

void CSkyBox::CreateMaterial()
{
	wstring MtrlName = L"SkyBoxMtrl";
	wstring ShaderName = L"SkyBoxShader";

	Ptr<AMaterial> pMtrl = AssetMgr::GetInst()->Find<AMaterial>(MtrlName);

	if (nullptr == pMtrl)
	{
		Ptr<AGraphicShader> pShader = AssetMgr::GetInst()->Find<AGraphicShader>(ShaderName);

		if (nullptr == pShader)
		{
			pShader = new AGraphicShader;
			pShader->SetName(ShaderName);
			pShader->CreateVertexShader(L"HLSL\\skybox.fx", "VS_SkyBox");
			pShader->CreatePixelShader(L"HLSL\\skybox.fx", "PS_SkyBox");
			pShader->SetRSType(RS_TYPE::CULL_FRONT);
			pShader->SetBSType(BS_TYPE::DEFAULT);
			pShader->SetDSType(DS_TYPE::LESS_EQUAL);
			AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());
		}

		pMtrl = new AMaterial;
		pMtrl->SetName(MtrlName);
		pMtrl->SetShader(pShader);
		pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_OPAQUE);
		AssetMgr::GetInst()->AddAsset(pMtrl->GetName(), pMtrl.Get());
	}

	SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(MtrlName), 0);
}


void CSkyBox::SaveToLevelFile(FILE* _File)
{
}

void CSkyBox::LoadFromLevelFile(FILE* _File)
{
}
