#include "pch.h"
#include "Asset.h"

#include "PathMgr.h"

Asset::Asset(ASSET_TYPE _Type, bool _EngineAsset)
	: m_Type(_Type)
	, m_EngineAsset(_EngineAsset)
{
}

Asset::Asset(const Asset& _Origin)
	: Entity(_Origin)
	, m_Type(_Origin.m_Type)
	, m_EngineAsset(false)
{
}

Asset::~Asset()
{
}

bool Asset::CheckFilePath(const wstring& _FilePath)
{
	const wstring& ContentPath = PathMgr::GetInst()->GetContentPath();

	if (0 != _FilePath.find(ContentPath.c_str()))
	{
		MessageBox(nullptr, L"잘못된 경로", L"에셋 저장 실패", MB_OK);
		return false;
	}

	return true;
}

void Asset::SaveRelativePath(const wstring& _FilePath)
{	
	wstring RelativePath = _FilePath.substr(PathMgr::GetInst()->GetContentPath().length()
											, _FilePath.length() - PathMgr::GetInst()->GetContentPath().length());

	SetRelativePath(RelativePath);
}
