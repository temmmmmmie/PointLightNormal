#pragma once

#include "Asset.h"

#include "assets.h"
#include "PathMgr.h"

class AssetMgr
	: public singleton<AssetMgr>
{
	SINGLE(AssetMgr)
private:
	map<wstring, Ptr<Asset>>	m_mapAsset[(UINT)ASSET_TYPE::END];
	bool						m_Changed;

public:
	void Init();

	void AddAsset(const wstring& _Key, Ptr<Asset> _Asset);

	Ptr<Asset> Find(ASSET_TYPE _Type, const wstring& _Key);

	template<typename T>
	Ptr<T> Find(const wstring& _Key);

	template<typename T>
	Ptr<T> Load(const wstring& _Key, const wstring& _RelativePath);

	Ptr<ATexture> CreateTexture(const wstring& _Key, UINT _Width, UINT _Height, DXGI_FORMAT _Format, UINT _BindFlag, D3D11_USAGE _Usage = D3D11_USAGE_DEFAULT);

	void GetAssetNames(ASSET_TYPE _Type, vector<wstring>& _vecName);

	bool IsChanged() { bool Changed = m_Changed; m_Changed = false; return Changed; }

	Ptr<AMeshData> LoadFBX(const wstring& _strPath);

private:
	void CreateEngineMesh();
	void CreateEngineGraphicShader();
	void CreateEngineMaterial();
	void CreateEngineSprite();
	void CreateEngineFlipbook();
};

template<typename T>
ASSET_TYPE GetAssetType()
{
	if constexpr (std::is_same_v<T, AMesh>)
		return ASSET_TYPE::MESH;
	else if constexpr (std::is_same_v<T, ATexture>)
		return ASSET_TYPE::TEXTURE;
	else if constexpr (std::is_same_v<T, ASprite>)
		return ASSET_TYPE::SPRITE;
	else if constexpr (std::is_same_v<T, AFlipbook>)
		return ASSET_TYPE::FLIPBOOK;
	else if constexpr (std::is_same_v<T, AMaterial>)
		return ASSET_TYPE::MATERIAL;
	else if constexpr (std::is_same_v<T, AGraphicShader>)
		return ASSET_TYPE::GRAPHICSHADER;
	else if constexpr (std::is_same_v<T, ASound>)
		return ASSET_TYPE::SOUND;
	else if constexpr (std::is_same_v<T, APrefab>)
		return ASSET_TYPE::PREFAB;
	else if constexpr (std::is_same_v<T, AMeshData>)
		return ASSET_TYPE::MESHDATA;
	else 
		return ASSET_TYPE::END;
}



// 함수 템플렛에서 T 타입을 구별해서, 적절한 Enum 으로 전환하는 부분이 필요
template<typename T>
Ptr<T> AssetMgr::Find(const wstring& _Key)
{	
	ASSET_TYPE Type = GetAssetType<T>();

	assert(Type != ASSET_TYPE::END);

	map<wstring, Ptr<Asset>>::iterator iter = m_mapAsset[(UINT)Type].find(_Key);

	if (iter == m_mapAsset[(UINT)Type].end())
		return nullptr;

	return (T*)iter->second.Get();
}

template<typename T>
Ptr<T> AssetMgr::Load(const wstring& _Key, const wstring& _RelativePath)
{
	ASSET_TYPE Type = GetAssetType<T>();

	assert(Type != ASSET_TYPE::END);

	Ptr<Asset> pAsset = Find<T>(_Key).Get();
	if (nullptr != pAsset)
	{
		return (T*)pAsset.Get();
	}

	pAsset = new T;
	wstring FilePath = CONTENT_PATH;
	FilePath += _RelativePath;

	if (FAILED(pAsset->Load(FilePath)))
	{
		return nullptr;
	}

	// 로딩된 Asset 이, 자신의 Key, Path 를 알게 한다.
	pAsset->SetKey(_Key);
	pAsset->SetRelativePath(_RelativePath);
	m_mapAsset[(UINT)Type].insert(make_pair(_Key, pAsset));

	m_Changed = true;

	return (T*)pAsset.Get();
}

#define FIND(Type, Key) AssetMgr::GetInst()->Find<Type>(Key)
#define LOAD(Type, Key) AssetMgr::GetInst()->Load<Type>(Key, Key)