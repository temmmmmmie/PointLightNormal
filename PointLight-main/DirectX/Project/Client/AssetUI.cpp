#include "pch.h"
#include "AssetUI.h"


AssetUI::AssetUI(ASSET_TYPE _Type)
	: EditorUI(::GetString(_Type))
	, m_Type(_Type)
{
}

AssetUI::~AssetUI()
{
}

void AssetUI::OutputAssetName()
{
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.3f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.1f, 0.3f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.3f, 1.f));

	ImGui::Button(::GetString(m_Type));

	ImGui::PopStyleColor(3);
	ImGui::PopID();

	ImGui::Text("Name");
	ImGui::SameLine(60);
	string AssetName = GetString(m_TargetAsset->GetKey());
	ImGui::InputText("##AssetName", (char*)AssetName.c_str(), AssetName.length() + 1, ImGuiInputTextFlags_ReadOnly);

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
}