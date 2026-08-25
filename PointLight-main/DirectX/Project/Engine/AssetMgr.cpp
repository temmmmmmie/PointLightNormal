#include "pch.h"
#include "AssetMgr.h"
#include "assets.h"

AssetMgr::AssetMgr()
	: m_Changed(false)
{

}

AssetMgr::~AssetMgr()
{

}

void AssetMgr::AddAsset(const wstring& _Key, Ptr<Asset> _Asset)
{
	ASSET_TYPE Type = _Asset->GetType();

	// 이미 동일한 키로 등록된 에셋이 있으면 문제
	assert(m_mapAsset[(UINT)Type].find(_Key) == m_mapAsset[(UINT)Type].end());

	_Asset->SetKey(_Key);
	m_mapAsset[(UINT)Type].insert(make_pair(_Key, _Asset));

	m_Changed = true;
}

Ptr<Asset> AssetMgr::Find(ASSET_TYPE _Type, const wstring& _Key)
{
	map<wstring, Ptr<Asset>>::iterator iter = m_mapAsset[(UINT)_Type].find(_Key);

	if (m_mapAsset[(UINT)_Type].end() == iter)
	{
		return nullptr;
	}

	return iter->second;
}


Ptr<ATexture> AssetMgr::CreateTexture(const wstring& _Key, UINT _Width, UINT _Height, DXGI_FORMAT _Format, UINT _BindFlag, D3D11_USAGE _Usage)
{
	Ptr<ATexture> pTex = Find<ATexture>(_Key);

	if (nullptr != pTex)
		return pTex;

	pTex = new ATexture;
	pTex->Create(_Width, _Height, _Format, _BindFlag, _Usage);
	pTex->SetKey(_Key);
	m_mapAsset[(UINT)ASSET_TYPE::TEXTURE].insert(make_pair(_Key, pTex.Get()));

	return pTex;
}

void AssetMgr::GetAssetNames(ASSET_TYPE _Type, vector<wstring>& _vecName)
{
	for (const auto& Pair : m_mapAsset[(UINT)_Type])
	{
		_vecName.push_back(Pair.first);
	}
}