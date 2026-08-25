#include "pch.h"
#include "TextureUI.h"

#include <Engine/ATexture.h>

TextureUI::TextureUI()
	: AssetUI(ASSET_TYPE::TEXTURE)
{
}

TextureUI::~TextureUI()
{
}

void TextureUI::Tick_UI()
{
	OutputAssetName();

	Ptr<ATexture> pTex = GetAsset<ATexture>();

	int Width = (int)pTex->GetWidth();
	int Height = (int)pTex->GetHeight();

	// 텍스쳐 해상도
	ImGui::Text("Width");
	ImGui::SameLine(100);
	ImGui::InputInt("##Width", &Width, 0, 0, ImGuiInputTextFlags_ReadOnly);

	ImGui::Text("Height");
	ImGui::SameLine(100);
	ImGui::InputInt("##Height", &Height, 0, 0, ImGuiInputTextFlags_ReadOnly);


	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
	ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
	ImGui::Image((ImTextureID)pTex->GetSRV().Get(), ImVec2(150.f, 150.f), Vec2(0.f, 0.f), Vec2(1.f, 1.f), tint_col, border_col);
}
