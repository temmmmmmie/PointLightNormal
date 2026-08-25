#pragma once

// 경로 관리자
class PathMgr
	: public singleton<PathMgr>
{
	SINGLE(PathMgr);
private:
	wstring			m_BinPath;
	wstring			m_ContentPath;

public:
	void Init();
	
	const wstring& GetBinPath() { return m_BinPath; }
	const wstring& GetContentPath() { return m_ContentPath; }
	wstring GetRelativePath(const wstring& _FilePath);
};

#define CONTENT_PATH PathMgr::GetInst()->GetContentPath()
