#include "pch.h"
#include "EditorUI.h"


EditorUI::EditorUI(const string& _UIName)
	: m_UIName(_UIName)
	, m_Active(true)
	, m_ParentUI(nullptr)
	, m_UseSeperator(false)
{
}

EditorUI::~EditorUI()
{
}

void EditorUI::Tick()
{
	string UIName = m_UIName + m_SubName;

	if (nullptr == GetParentUI())
	{
		bool Active = m_Active;

		ImGui::Begin(UIName.c_str(), &Active);
		
		// UI 의 x 버튼을 눌러서 비활성화 한 경우를 대비해서 체크한다.
		SetActive(Active);

		// UI 본인이 출력할 내용
		Tick_UI();

		
		// 자식 UI 들 Tick 호출
		for (size_t i = 0; i < m_vecChildUI.size(); ++i)
		{
			if (m_vecChildUI[i]->IsActive())
			{
				m_vecChildUI[i]->Tick();
				if(m_vecChildUI[i]->m_UseSeperator)
					ImGui::Separator();
			}			
		}

		ImGui::End();
	}

	else
	{
		ImGui::BeginChild(UIName.c_str(), m_ChildSize);

		// UI 본인이 출력할 내용
		Tick_UI();

		// 자식 UI 들 Tick 호출
		for (size_t i = 0; i < m_vecChildUI.size(); ++i)
		{
			if (m_vecChildUI[i]->IsActive())
			{
				m_vecChildUI[i]->Tick();
			}
		}

		ImGui::EndChild();
	}
}

void EditorUI::AddChildUI(EditorUI* _ChildUI)
{
	m_vecChildUI.push_back(_ChildUI);
	_ChildUI->m_ParentUI = this;
}
