#include "pch.h"
#include "ListUI.h"

#include <Engine/KeyMgr.h>

ListUI::ListUI()
	: EditorUI("ListUI")
	, m_SelectedIdx(-1)
	, m_Inst(nullptr)
	, m_MemFunc(nullptr)
{
}

ListUI::~ListUI()
{
}

void ListUI::Tick_UI()
{
	for (size_t i = 0; i < m_vecList.size(); ++i)
	{
		UINT Flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanFullWidth;

		if(i == m_SelectedIdx)
			Flags |= ImGuiTreeNodeFlags_Selected;

		if (ImGui::TreeNodeEx(m_vecList[i].c_str(), Flags))
		{
			// 클릭 검사
			if (ImGui::IsItemClicked())
			{
				m_SelectedIdx = i;
			}

			// 더블클릭 검사
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				// ListUI 비활성화
				SetActive(false);

				// 등록된 CallBack 이나 Delegate 가 있으면 호출
				if (m_Inst && m_MemFunc)
					(m_Inst->*m_MemFunc)((DWORD_PTR)this);
			}

			ImGui::TreePop();
		}		
	}
}

void ListUI::Activate()
{
	m_vecList.clear();
}

void ListUI::Deactivate()
{
	
}