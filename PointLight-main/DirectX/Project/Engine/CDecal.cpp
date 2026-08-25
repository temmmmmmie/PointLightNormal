#include "pch.h"
#include "CDecal.h"

CDecal::CDecal()
	: CRenderComponent(COMPONENT_TYPE::DECAL)
{
	SetMesh(FIND(AMesh, L"CubeMesh"));

	CreateMaterial();	
}

CDecal::~CDecal()
{
}


void CDecal::FinalTick()
{	
	DrawDebugCube(Transform()->GetmatWorld(), Vec4(0.f, 1.f, 0.f, 1.f), true, 0.f);
}

void CDecal::Render()
{
	Transform()->Binding();

	GetMaterial(0)->SetScalar(INT_0, (int)m_Mode);

	// 특정 오브젝트만 데칼을 넣고 싶을 경우
	if (nullptr != m_Object)
		GetMaterial(0)->SetScalar(FLOAT_0, (float)m_Object->GetID());
	else
		GetMaterial(0)->SetScalar(FLOAT_0, 0.f);

	GetMaterial(0)->SetTexture(TEX_2, m_DecalTex);
	GetMaterial(0)->Binding();

	GetMesh()->Render(0);
}

void CDecal::SetDecalTex(Ptr<ATexture> _Tex)
{
	m_DecalTex = _Tex;
}

void CDecal::SetSpecifiedObject(Ptr<GameObject> _Object)
{
	m_Object = _Object;
}

void CDecal::CreateMaterial()
{
	wstring MtrlName = L"DecalMtrl";
	wstring ShaderName = L"DecalShader";

	Ptr<AMaterial> pMtrl = AssetMgr::GetInst()->Find<AMaterial>(MtrlName);

	if (nullptr == pMtrl)
	{
		Ptr<AGraphicShader> pShader = AssetMgr::GetInst()->Find<AGraphicShader>(ShaderName);

		if (nullptr == pShader)
		{
			pShader = new AGraphicShader;
			pShader->SetName(ShaderName);
			pShader->CreateVertexShader(L"HLSL\\decal.fx", "VS_Decal");
			pShader->CreatePixelShader(L"HLSL\\decal.fx", "PS_Decal");
			pShader->SetRSType(RS_TYPE::CULL_FRONT);
			pShader->SetBSType(BS_TYPE::DECAL);
			pShader->SetDSType(DS_TYPE::LESS_EQUAL);
			AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());
		}

		pMtrl = new AMaterial;
		pMtrl->SetName(MtrlName);
		pMtrl->SetShader(pShader);
		pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_DECAL);

		pMtrl->SetTexture(TEX_0, FIND(ATexture, L"PositionTargetTex"));
		pMtrl->SetTexture(TEX_1, FIND(ATexture, L"DataTargetTex"));

		AssetMgr::GetInst()->AddAsset(pMtrl->GetName(), pMtrl.Get());
	}

	SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(MtrlName), 0);
}

void CDecal::SaveToLevelFile(FILE* _File)
{
}

void CDecal::LoadFromLevelFile(FILE* _File)
{
}