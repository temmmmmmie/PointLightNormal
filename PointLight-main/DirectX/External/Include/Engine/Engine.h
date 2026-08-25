#pragma once

class Engine
	: public singleton<Engine>
{
	SINGLE(Engine)
private:
	HINSTANCE		m_hInst;
	HWND			m_hMainWnd;
	Vec2			m_WindowResolution;

	// FMOD 관리자 클래스
	FMOD::System*	m_FMODSystem;	// FMOD 최상위 관리자 객체


public:
	// GameObject 저장 불러오기 함수
	typedef void (*SAVE_GAMEOBJECT)(GameObject*, FILE*);
	typedef GameObject* (*LOAD_GAMEOBJECT)(FILE*);

	SAVE_GAMEOBJECT m_SaveGameObjectFunc;
	LOAD_GAMEOBJECT m_LoadGameObjectFunc;


public:
	int Init(HINSTANCE _Inst, const wstring& _TitleName, const WNDCLASSEXW& _WindowClass, Vec2 _WindResolution);
	void Progress();

	FMOD::System* GetFMODSystem() { return m_FMODSystem; }

public:
	void ChangeResolution(Vec2 _Resolution);
	HWND GetMainWnd() { return  m_hMainWnd; }

private:
	int CreateGameWindow(const wstring& _Title, const WNDCLASSEXW& _WindowClass, Vec2 _WindResolution);
};

#define FMOD_SYSTEM Engine::GetInst()->GetFMODSystem()


