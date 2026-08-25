#include "pch.h"
#include "ComponentUI.h"

ComponentUI::ComponentUI(const string _Name, COMPONENT_TYPE _Type)
	: EditorUI(_Name)
	, m_ComType(_Type)
{
	UseSeperator(true);
}

ComponentUI::~ComponentUI()
{
}

void ComponentUI::Tick_UI()
{
	OutputComponentName();
}

void ComponentUI::OutputComponentName()
{
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.3f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.1f, 0.3f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.3f, 1.f));

	ImGui::Button(::GetString(m_ComType));

	ImGui::PopStyleColor(3);
	ImGui::PopID();


	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
}

void ComponentUI::SetTargetObject(Ptr<GameObject> _Object)
{
	if (nullptr == _Object || nullptr == _Object->GetComponent(m_ComType))
	{
		m_TargetObject = nullptr;
		SetActive(false);		
	}
	else
	{
		m_TargetObject = _Object;
		SetActive(true);
	}	
}