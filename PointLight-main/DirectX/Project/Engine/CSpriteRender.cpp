#include "pch.h"
#include "CSpriteRender.h"

#include "AssetMgr.h"

CSpriteRender::CSpriteRender()
	: CRenderComponent(COMPONENT_TYPE::SPRITE_RENDER)
{
	SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));

	CreateMaterial();
}

CSpriteRender::~CSpriteRender()
{
}

void CSpriteRender::FinalTick()
{
}

void CSpriteRender::Render()
{
	if (nullptr != m_Sprite)
	{
		GetMaterial(0)->SetScalar(VEC2_0, m_Sprite->GetLeftTop());
		GetMaterial(0)->SetScalar(VEC2_1, m_Sprite->GetSlice());
		GetMaterial(0)->SetScalar(VEC2_2, m_Sprite->GetOffset());

		GetMaterial(0)->SetTexture(TEX_0, m_Sprite->GetAtlas());
	}

	GetMaterial(0)->Binding();

	GetMesh()->Render(0);
}

void CSpriteRender::SaveToLevelFile(FILE* _File)
{
	CRenderComponent::SaveToLevelFile(_File);
	SaveAssetRef(m_Sprite.Get(), _File);
}

void CSpriteRender::LoadFromLevelFile(FILE* _File)
{
	CRenderComponent::LoadFromLevelFile(_File);
	m_Sprite = LoadAssetRef<ASprite>(_File);
}

void CSpriteRender::CreateMaterial()
{
	Ptr<AMaterial> pMtrl = AssetMgr::GetInst()->Find<AMaterial>(L"SpriteMtrl");

	if (nullptr == pMtrl)
	{
		Ptr<AGraphicShader> pShader = AssetMgr::GetInst()->Find<AGraphicShader>(L"SpriteShader");

		if (nullptr == pShader)
		{
			pShader = new AGraphicShader;
			pShader->SetName(L"SpriteShader");
			pShader->CreateVertexShader(L"HLSL\\sprite.fx", "VS_Sprite");
			pShader->CreatePixelShader(L"HLSL\\sprite.fx", "PS_Sprite");
			pShader->SetRSType(RS_TYPE::CULL_NONE);
			pShader->SetBSType(BS_TYPE::ALPHABLEND);
			pShader->SetDSType(DS_TYPE::NO_WRITE);
			AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());
		}

		pMtrl = new AMaterial;
		pMtrl->SetName(L"SpriteMtrl");
		pMtrl->SetShader(pShader);
		pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_TRANSPARENT);
		AssetMgr::GetInst()->AddAsset(pMtrl->GetName(), pMtrl.Get());
	}

	SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"SpriteMtrl"), 0);
}
