#include "pch.h"
#include "PathMgr.h"

#include "Engine.h"

PathMgr::PathMgr()
{

}

PathMgr::~PathMgr()
{

}

void PathMgr::Init()
{
	// 현재 경로(프로세스가 있던 폴더) 알아내기
	// VisualStudio 를 통해서 디버깅모드로 실행된 경우, Project 속성에 있는 정보를 토대로 
	// CurrentDirectory 가 결정된다.
	wchar_t szBuffer[256] = {};
	GetCurrentDirectory(256, szBuffer);

	// Bin 폴더 경로 저장
	m_BinPath = szBuffer;

	// 상위 폴더(부모폴더) 경로를 알아낸다.
	int Len = wcslen(szBuffer);

	for (int i = Len-1; 0 < i; --i)
	{
		if ('\\' == szBuffer[i])
		{
			szBuffer[i] = 0;
			break;
		}
	}

	// Content 를 붙여준다.
	wcscat_s(szBuffer, 256, L"\\Content\\");
		
	//SetWindowText(Engine::GetInst()->GetMainWinow(), szBuffer);

	m_ContentPath = szBuffer;
}


wstring PathMgr::GetRelativePath(const wstring& _FilePath)
{
	size_t FindPos = _FilePath.find(m_ContentPath);
	if (FindPos == wstring::npos)
		return L"";

	return _FilePath.substr(FindPos + m_ContentPath.length(), _FilePath.length());
}