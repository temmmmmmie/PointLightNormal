#include "pch.h"
#include "Engine.h"

#include "Device.h"
#include "PathMgr.h"
#include "TimeMgr.h"
#include "KeyMgr.h"
#include "AssetMgr.h"
#include "LevelMgr.h"
#include "RenderMgr.h"
#include "TaskMgr.h"
#include "FontMgr.h"

Engine::Engine()
    : m_hInst(nullptr)
    , m_hMainWnd(nullptr)
    , m_FMODSystem(nullptr)
{
}

Engine::~Engine()
{
    if (nullptr != m_FMODSystem)
        m_FMODSystem->release();
}

int Engine::Init(HINSTANCE _Inst, const wstring& _TitleName, const WNDCLASSEXW&  _WindowClass , Vec2 _WindResolution)
{
    m_hInst = _Inst;

    // 메인 윈도우 생성
    if (FAILED(CreateGameWindow(_TitleName, _WindowClass, _WindResolution)))
    {
        MessageBox(m_hMainWnd, L"윈도우 생성 실패", L"엔진 초기화 실패", MB_OK);
        return E_FAIL;
    }

    // GPU 장치 초기화
    if (FAILED(Device::GetInst()->Init(m_hMainWnd)))
    {
        MessageBox(m_hMainWnd, L"Device 초기화 실패", L"엔진 초기화 실패", MB_OK);
        return E_FAIL;
    }

    // Manager 초기화
    PathMgr::GetInst()->Init();
    AssetMgr::GetInst()->Init();
    RenderMgr::GetInst()->Init();
    FontMgr::GetInst()->Init();

    // FMOD 초기화		
    FMOD::System_Create(&m_FMODSystem);
    assert(m_FMODSystem);

    // 32개 채널 생성
    FMOD_RESULT result = m_FMODSystem->init(32, FMOD_DEFAULT, nullptr);

    return S_OK;
}

void Engine::Progress()
{
    TimeMgr::GetInst()->Tick();
    KeyMgr::GetInst()->Tick();

    // 레벨 업데이트
    LevelMgr::GetInst()->Progress();   

    // Task
    TaskMgr::GetInst()->Tick();
}



int Engine::CreateGameWindow(const wstring& _Title, const WNDCLASSEXW& _WindowClass, Vec2 _WindResolution)
{
    RegisterClassExW(&_WindowClass);

    m_hMainWnd = CreateWindowW(L"Window Class", L"My Game", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, m_hInst , nullptr);

    if (!m_hMainWnd)
    {
        return E_FAIL;
    }

    // 해상도 설정
    ChangeResolution(_WindResolution);

    // 윈도우를 화면에 노출
    ShowWindow(m_hMainWnd, true);
    UpdateWindow(m_hMainWnd);

    return S_OK;
}


void Engine::ChangeResolution(Vec2 _Resolution)
{
    // 원하는 해상도를 가지기 위해서 실제 윈도우 크기가 어떠해야 하는가 계산
    RECT rt = { 0, 0, (int)_Resolution.x, (int)_Resolution.y };
    AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, (bool)GetMenu(m_hMainWnd));
    SetWindowPos(m_hMainWnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);

    // 최종 생성된 윈도우의 내부 렌더링 영역 해상도값을 확인(요청되로 안됐을 수도 있기 때문)
    GetClientRect(m_hMainWnd, &rt);
    m_WindowResolution.x = rt.right - rt.left;
    m_WindowResolution.y = rt.bottom - rt.top;
}