#include "pch.h"
#include "MeshUI.h"

#include <Engine/AMesh.h>

MeshUI::MeshUI()
	: AssetUI(ASSET_TYPE::MESH)
{
}

MeshUI::~MeshUI()
{
}

void MeshUI::Tick_UI()
{
	OutputAssetName();

	Ptr<AMesh> pMesh = GetAsset<AMesh>();

	int VtxCount = (int)pMesh->GetVtxCount();

	ImGui::Text("Vertex Count");
	ImGui::SameLine(120);
	ImGui::InputInt("##VtxCnt", &VtxCount, 0, 0, ImGuiInputTextFlags_ReadOnly);

}