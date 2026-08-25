#pragma once

#include "GameObjectEx.h"
#include "EditorUI.h"

class EditorMgr
	: public singleton<EditorMgr>
{
	SINGLE(EditorMgr)
private:
	vector<Ptr<GameObjectEx>>	m_vecEditorObj;
	bool						m_ShowDemo;

	// EditorUI µé °ü¸®
	map<string, Ptr<EditorUI>>  m_mapUI;

public:
	void Init();
	void Progress();

public:
	Ptr<EditorUI> FindUI(const string& _Name)
	{
		map<string, Ptr<EditorUI>>::iterator iter = m_mapUI.find(_Name);
		if (iter == m_mapUI.end())
			return nullptr;
		return iter->second;
	}

private:
	void CreateEditorGameObject();
	void InitImGui();
	void CreateEditorUI();
	void AddEditorUI(Ptr<EditorUI> _UI)
	{
		assert(m_mapUI.find(_UI->GetUIName()) == m_mapUI.end());				
		m_mapUI.insert(make_pair(_UI->GetUIName(), _UI));
	}
	void EditorObjectProgress();
	void ImGuiProgress();
};

#define FIND_UI(Type, Name) ((Type*)(EditorMgr::GetInst()->FindUI(Name).Get()))

