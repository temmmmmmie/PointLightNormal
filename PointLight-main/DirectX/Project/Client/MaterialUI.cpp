#include "pch.h"
#include "MaterialUI.h"

#include <Engine/AMaterial.h>
#include <Engine/AGraphicShader.h>
#include <Engine/PathMgr.h>

#include "TreeUI.h"

MaterialUI::MaterialUI()
	: AssetUI(ASSET_TYPE::MATERIAL)
{
}

MaterialUI::~MaterialUI()
{
}

void MaterialUI::Tick_UI()
{
	OutputAssetName();

	Ptr<AMaterial> pMtrl = GetAsset<AMaterial>();

	// 재질이 참조하는 쉐이더
	Ptr<AGraphicShader> pShader = pMtrl->GetShader();

	string ShaderName;
	if(nullptr != pShader)
		ShaderName = GetString(pShader->GetKey());

	ImGui::Text("Shader Name");
	ImGui::SameLine(120);
	ImGui::InputText("##ShaderName", (char*)ShaderName.c_str(), ShaderName.length() + 1, ImGuiInputTextFlags_ReadOnly);

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
				if (ASSET_TYPE::GRAPHICSHADER == pAsset->GetType())
				{
					pMtrl->SetShader((AGraphicShader*)pAsset.Get());
				}
			}
		}

		ImGui::EndDragDropTarget();
	}

	// Material Domain(렌더링 시점)
	ImGui::Text("Domain");
	ImGui::SameLine(120);

	static const char* arrRenderDomain[] =
	{
		"DOMAIN_OPAQUE",
		"DOMAIN_MASK",
		"DOMAIN_TRANSPARENT",
		"DOMAIN_PARTICLE",
		"DOMAIN_POSTPROCESS",
		"DOMAIN_DEBUG",
		"NONE",
	};

	RENDER_DOMAIN CurDomain = pMtrl->GetDomain();

	if (ImGui::BeginCombo("##DomainCombo", arrRenderDomain[(int)CurDomain], 0))
	{
		for (int i = 0; i < IM_ARRAYSIZE(arrRenderDomain); i++)
		{
			const bool is_selected = ((int)CurDomain == i);

			if (ImGui::Selectable(arrRenderDomain[i], is_selected))
			{
				CurDomain = (RENDER_DOMAIN)i;
				pMtrl->SetDomain(CurDomain);
			}			

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	// Shader 가 요구하는 파라미터에 대응하는 재질의 데이터를 노출
	ShaderParam();

	// 저장버튼
	ImGui::BeginDisabled(pMtrl->IsEngineAsset());
	{
		if (ImGui::Button("Save", Vec2(50, 20)))
		{
			// 파일로 저장
			wstring strPath = PathMgr::GetInst()->GetContentPath();
			strPath += pMtrl->GetKey();
			pMtrl->Save(strPath);
		}		
	}
	ImGui::EndDisabled();
}

void MaterialUI::ShaderParam()
{
	Ptr<AMaterial> pMtrl = GetAsset<AMaterial>();

	// 재질이 참조하는 쉐이더
	Ptr<AGraphicShader> pShader = pMtrl->GetShader();

	if (nullptr != pShader)
	{
		ImGui::Separator();
		ImGui::Text("Shader Pamamter");

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();


		// 재질이 사용하는 쉐이더가 요구하는 파라미터 타입들
		const vector<tShaderParam>& vecParam = pShader->GetShaderParam();

		for (size_t i = 0; i < vecParam.size(); ++i)
		{
			switch (vecParam[i].Param)
			{
			case SHADER_PARAM::INT:
			{
				SCALAR_PARAM ScalarType = (SCALAR_PARAM)((int)SCALAR_PARAM::INT_0 + vecParam[i].Index);

				int Data = pMtrl->GetScalar<int>(ScalarType);

				ImGui::Text(GetString(vecParam[i].Desc).c_str());
				ImGui::SameLine(120);

				if (vecParam[i].IsInput)
				{
					if (ImGui::InputInt("##InputInt", &Data, vecParam[i].Step))
					{
						pMtrl->SetScalar(ScalarType, Data);
					}
				}
				else
				{
					if (ImGui::DragInt("##InputInt", &Data, vecParam[i].Step))
					{
						pMtrl->SetScalar(ScalarType, Data);
					}
				}

			}
			break;
			case SHADER_PARAM::FLOAT:
			{
				SCALAR_PARAM ScalarType = (SCALAR_PARAM)((int)SCALAR_PARAM::FLOAT_0 + vecParam[i].Index);

				float Data = pMtrl->GetScalar<float>(ScalarType);

				ImGui::Text(GetString(vecParam[i].Desc).c_str());
				ImGui::SameLine(120);

				if (vecParam[i].IsInput)
				{
					if (ImGui::InputFloat("##InputFloat", &Data, vecParam[i].Step))
					{
						pMtrl->SetScalar(ScalarType, Data);
					}
				}
				else
				{
					if (ImGui::DragFloat("##DragFloat", &Data, vecParam[i].Step))
					{
						pMtrl->SetScalar(ScalarType, Data);
					}
				}
			}
			break;
			case SHADER_PARAM::VEC2:
			{
				SCALAR_PARAM ScalarType = (SCALAR_PARAM)((int)SCALAR_PARAM::VEC2_0 + vecParam[i].Index);

				Vec2 Data = pMtrl->GetScalar<Vec2>(ScalarType);

				ImGui::Text(GetString(vecParam[i].Desc).c_str());
				ImGui::SameLine(120);

				if (vecParam[i].IsInput)
				{
					if (ImGui::InputFloat2("##InputFloat2", Data))
					{
						pMtrl->SetScalar(ScalarType, Data);
					}
				}
				else
				{
					if (ImGui::DragFloat2("##DragFloat2", Data, vecParam[i].Step))
					{
						pMtrl->SetScalar(ScalarType, Data);
					}
				}
			}
			break;
			case SHADER_PARAM::VEC4:
			{
				SCALAR_PARAM ScalarType = (SCALAR_PARAM)((int)SCALAR_PARAM::VEC4_0 + vecParam[i].Index);

				Vec4 Data = pMtrl->GetScalar<Vec4>(ScalarType);

				ImGui::Text(GetString(vecParam[i].Desc).c_str());
				ImGui::SameLine(120);

				if (vecParam[i].IsInput)
				{
					if (ImGui::InputFloat4("##InputFloat4", Data))
					{
						pMtrl->SetScalar(ScalarType, Data);
					}
				}
				else
				{
					if (ImGui::DragFloat4("##DragFloat4", Data, vecParam[i].Step))
					{
						pMtrl->SetScalar(ScalarType, Data);
					}
				}
			}
			break;
			case SHADER_PARAM::MATRIX:
			{
				SCALAR_PARAM ScalarType = (SCALAR_PARAM)((int)SCALAR_PARAM::MAT_0 + vecParam[i].Index);

				Matrix Data = pMtrl->GetScalar<Matrix>(ScalarType);

				ImGui::Text(GetString(vecParam[i].Desc).c_str());

				Vec4 Row_0 = Vec4(Data._11, Data._12, Data._13, Data._14);
				Vec4 Row_1 = Vec4(Data._21, Data._22, Data._23, Data._24);
				Vec4 Row_2 = Vec4(Data._31, Data._32, Data._33, Data._34);
				Vec4 Row_3 = Vec4(Data._41, Data._42, Data._43, Data._44);

				bool IsChanged = false;
				if (ImGui::InputFloat4("##InputMat_0", Row_0))
					IsChanged = true;
				if (ImGui::InputFloat4("##InputMat_1", Row_1))
					IsChanged = true;
				if (ImGui::InputFloat4("##InputMat_2", Row_2))
					IsChanged = true;
				if (ImGui::InputFloat4("##InputMat_3", Row_3))
					IsChanged = true;

				if (IsChanged)
					pMtrl->SetScalar(ScalarType, Matrix(Row_0, Row_1, Row_2, Row_3));
			}
			break;
			case SHADER_PARAM::TEXTURE:
			{
				TEX_PARAM TexParam = (TEX_PARAM)(vecParam[i].Index);

				ImGui::Text(GetString(vecParam[i].Desc).c_str());
				ImGui::SameLine(120);


				Ptr<ATexture> pTex = pMtrl->GetTexture(TexParam);

				ImTextureID ID = 0;
				if (nullptr != pTex)
					ID = (ImTextureID)pTex->GetSRV().Get();

				ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
				ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
				ImGui::Image(ID, ImVec2(150.f, 150.f), Vec2(0.f, 0.f), Vec2(1.f, 1.f), tint_col, border_col);

				// Drop Check
				if (ImGui::BeginDragDropTarget())
				{
					const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentUI");

					if (payload)
					{
						TreeNode* pNode = *((TreeNode**)payload->Data);

						if (0 != pNode->GetData())
						{
							Ptr<Asset> pAsset = (Asset*)pNode->GetData();
							if (ASSET_TYPE::TEXTURE == pAsset->GetType())
							{
								pMtrl->SetTexture(TexParam, (ATexture*)pAsset.Get());
							}
						}
					}

					ImGui::EndDragDropTarget();
				}
			}
			break;
			default:
				break;
			}
		}
	}
}
