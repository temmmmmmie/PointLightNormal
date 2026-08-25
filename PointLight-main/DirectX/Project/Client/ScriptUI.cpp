#include "pch.h"
#include "ScriptUI.h"

#include "TreeUI.h"
#include <Source/ScriptMgr.h>

ScriptUI::ScriptUI()
	: ComponentUI("ScriptUI", COMPONENT_TYPE::SCRIPT)
{
}

ScriptUI::~ScriptUI()
{
}

void ScriptUI::SetScript(CScript* _Script)
{
	m_TargetScript = _Script;

	if (nullptr == m_TargetScript)
		SetActive(false);
	else
		SetActive(true);
}

void ScriptUI::Tick_UI()
{
	m_ItemHeight = 0;

	// 담당 스크립트 이름 출력
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.3f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.1f, 0.3f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.3f, 1.f));
	 
	ImGui::Button(GetString(ScriptMgr::GetScriptName(m_TargetScript.Get())).c_str());
	AddItemHeight();

	ImGui::PopStyleColor(3);
	ImGui::PopID();

	// Script 파라미터 노출
	const vector<tScriptParam>& vecParam = m_TargetScript->GetScriptParam();

	for (size_t i = 0; i < vecParam.size(); ++i)
	{
		switch (vecParam[i].Param)
		{
		case SCRIPT_PARAM::INT:
			break;
		case SCRIPT_PARAM::FLOAT:
		{			
			ImGui::Text(GetString(vecParam[i].Desc).c_str());
			ImGui::SameLine(120);

			if (vecParam[i].IsInput)
				ImGui::InputFloat("##InputFloat", (float*)vecParam[i].Data, vecParam[i].Step);
			else
				ImGui::DragFloat("##DragFloat", (float*)vecParam[i].Data, vecParam[i].Step);

			AddItemHeight();
		}
			break;
		case SCRIPT_PARAM::VEC2:
			break;
		case SCRIPT_PARAM::VEC4:
			break;
		case SCRIPT_PARAM::MATRIX:
			break;
		case SCRIPT_PARAM::TEXTURE:
			break;
		case SCRIPT_PARAM::PREFAB:
		{
			ImGui::Text(GetString(vecParam[i].Desc).c_str());
			ImGui::SameLine(120);
			
			Ptr<APrefab> pPrefab = (*((Ptr<APrefab>*)vecParam[i].Data));

			string PrefabName = "None";
			if (nullptr != pPrefab)
			{
				PrefabName = GetString(pPrefab->GetKey());
			}

			string Label = "##" + GetString(vecParam[i].Desc);
			ImGui::InputText(Label.c_str(), (char*)PrefabName.c_str(), PrefabName.length() + 1, ImGuiInputTextFlags_ReadOnly);

			// DropCheck
			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentUI");

				if (payload)
				{
					TreeNode* pNode = *((TreeNode**)payload->Data);

					if (0 != pNode->GetData())
					{
						Ptr<Asset> pAsset = (Asset*)pNode->GetData();
						if (ASSET_TYPE::PREFAB == pAsset->GetType())
						{
							(*((Ptr<APrefab>*)vecParam[i].Data)) = (APrefab*)pAsset.Get();
						}
					}
				}

				ImGui::EndDragDropTarget();
			}

			AddItemHeight();
		}
			break;
		case SCRIPT_PARAM::MATERIAL:
			break;
		default:
			break;
		}		
	}

	SetChildSize(Vec2(0.f, (float)m_ItemHeight));
}

void ScriptUI::AddItemHeight()
{
	ImVec2 vSize = ImGui::GetItemRectSize();
	m_ItemHeight += vSize.y + 3.f;
}
