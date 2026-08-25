#include "pch.h"
#include "APrefab.h"

#include "Engine.h"


APrefab::APrefab(bool _EngineAsset)
	: Asset(ASSET_TYPE::PREFAB, _EngineAsset)
{
}

APrefab::~APrefab()
{
}

GameObject* APrefab::Instantiate()
{	
	return m_ProtoObj->Clone();
}

void APrefab::SetGameObject(Ptr<GameObject> _Object)
{
	assert(-1 == _Object->GetLayerIdx());

	m_ProtoObj = _Object;
}

int APrefab::Save(const wstring& _FilePath)
{
	// 저장 경로 체크
	if (!CheckFilePath(_FilePath))
		return E_FAIL;

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _FilePath.c_str(), L"wb");

	Engine::GetInst()->m_SaveGameObjectFunc(m_ProtoObj.Get(), pFile);

	fclose(pFile);

	return S_OK;
}

int APrefab::Load(const wstring& _FilePath)
{
	// 로드 경로 체크
	if (!CheckFilePath(_FilePath))
		return E_FAIL;

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _FilePath.c_str(), L"rb");

	m_ProtoObj = Engine::GetInst()->m_LoadGameObjectFunc(pFile);

	fclose(pFile);

	return S_OK;
}