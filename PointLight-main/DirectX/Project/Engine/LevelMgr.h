#pragma once

#include "Level.h"

class LevelMgr
	: public singleton<LevelMgr>
{
	SINGLE(LevelMgr);
private:
	Ptr<ALevel>		m_CurLevel;
	bool			m_LevelChanged;

public:
	Ptr<ALevel> GetCurrentLevel() { return m_CurLevel; }
	Ptr<GameObject> FindObjectByName(const wstring& _Name);

	void SetChanged() { m_LevelChanged = true; }
	bool IsChanged()
	{
		bool IsChanged = m_LevelChanged;
		m_LevelChanged = false;
		return IsChanged;
	}

private:
	void ChangeLevel(Ptr<ALevel> _NextLevel) 
	{ 
		m_CurLevel = _NextLevel; 
		m_LevelChanged = true;
	}

public:
	void Init();
	void Progress();

	friend class TaskMgr;
};

