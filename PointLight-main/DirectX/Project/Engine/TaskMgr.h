#pragma once

class TaskMgr
	: public singleton<TaskMgr>
{
	SINGLE(TaskMgr);
private:
	vector<tTaskInfo>		m_vecTask;
	vector<Ptr<GameObject>> m_Garbage;

public:
	void AddTask(const tTaskInfo& _info) { m_vecTask.push_back(_info); }
	void Tick();

private:
	void ChangeLevelStateProcess(class ALevel* _Level, LEVEL_STATE _State);
};