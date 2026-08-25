#pragma once

class TimeMgr
	: public singleton<TimeMgr>
{
	SINGLE(TimeMgr)
private:
	LARGE_INTEGER	m_Frequency;
	LARGE_INTEGER	m_CurCount;
	LARGE_INTEGER	m_PrevCount;

	int				m_FrameCount;
	wchar_t			m_String[256];

	float			m_DeltaTime; // 1 프레임 수행하는데 걸리는 시간
	float			m_AccTime;	 // 누적 시간

	float			m_EngineDT;
	float			m_EngineTime;

public:
	float GetDeltaTime() { return m_DeltaTime; }

public:
	void Tick();
	void Render();
};

#define DT			TimeMgr::GetInst()->GetDeltaTime()
#define ENGINE_DT	g_Global.EngineDT
#define ENGINE_TIME g_Global.EngineTime