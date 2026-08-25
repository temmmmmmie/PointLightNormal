#include "pch.h"
#include "AMaterial.h"

#include "PathMgr.h"

#include "Device.h"
#include "ConstBuffer.h"


AMaterial::AMaterial(bool _EngineAsset)
	: Asset(ASSET_TYPE::MATERIAL, _EngineAsset)
	, m_Domain(RENDER_DOMAIN::NONE)
	, m_Const{}
{
	m_Const.DiffCoef = Vec4(1.f, 1.f, 1.f, 1.f);
	m_Const.SpecCoef = Vec4(0.2f, 0.2f, 0.2f, 1.f);
}

AMaterial::~AMaterial()
{
}

void AMaterial::Binding()
{
	m_Shader->Binding();

	for (UINT i = 0; i < TEX_END; ++i)
	{
		if (nullptr == m_Tex[i])
		{
			ATexture::Clear(i);
			m_Const.bTex[i] = 0;
		}
		else
		{
			m_Tex[i]->Binding(i);
			m_Const.bTex[i] = 1;
		}
	}

	static Ptr<ConstBuffer> pCB = Device::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pCB->SetData(&m_Const);
	pCB->Binding();
}

int AMaterial::Save(const wstring& _FilePath)
{
	// 저장 경로 체크
	if (!CheckFilePath(_FilePath))
		return E_FAIL;

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _FilePath.c_str(), L"wb");

	// 재질의 렌더링 도메인 시점은 언제였는지
	fwrite(&m_Domain, sizeof(RENDER_DOMAIN), 1, pFile);

	// 쉐이더에 전달할 상수 데이터가 어떤 값이었는지
	fwrite(&m_Const, sizeof(tMtrlConst), 1, pFile);

	// 어떤 쉐이더를 사용 하는지
	SaveAssetRef(m_Shader.Get(), pFile);

	// 쉐이더(t레지스터에 바인딩할) 텍스쳐들이 무엇이었는지
	for (UINT i = 0; i < (UINT)TEX_PARAM::TEX_END; ++i)
	{
		SaveAssetRef(m_Tex[i].Get(), pFile);
	}

	fclose(pFile);

	// 상대경로 저장
	SaveRelativePath(_FilePath);

	return S_OK;
}

int AMaterial::Load(const wstring& _FilePath)
{
	// 로드 경로 체크
	if (!CheckFilePath(_FilePath))
		return E_FAIL;

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _FilePath.c_str(), L"rb");

	// 재질의 렌더링 도메인 시점은 언제였는지
	fread(&m_Domain, sizeof(RENDER_DOMAIN), 1, pFile);

	// 쉐이더에 전달할 상수 데이터가 어떤 값이었는지
	fread(&m_Const, sizeof(tMtrlConst), 1, pFile);

	// 어떤 쉐이더를 사용 하는지
	m_Shader = LoadAssetRef<AGraphicShader>(pFile);

	// 쉐이더(t레지스터에 바인딩할) 텍스쳐들이 무엇이었는지
	for (UINT i = 0; i < (UINT)TEX_PARAM::TEX_END; ++i)
	{
		m_Tex[i] = LoadAssetRef<ATexture>(pFile);
	}

	fclose(pFile);

	return S_OK;
}