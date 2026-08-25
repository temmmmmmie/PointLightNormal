#include "pch.h"
#include "MeshRenderUI.h"

#include "EditorMgr.h"
#include "ListUI.h"
#include "TreeUI.h"

MeshRenderUI::MeshRenderUI()
	: RenderComponentUI("MeshRenderUI", COMPONENT_TYPE::MESHRENDER)
{
}

MeshRenderUI::~MeshRenderUI()
{
}

void MeshRenderUI::Tick_UI()
{
	ComponentUI::Tick_UI();

	Ptr<CMeshRender> pMeshRender = GetTargetObject()->MeshRender();

	// Mesh
	{
		Ptr<AMesh> pMesh = pMeshRender->GetMesh();

		string MeshKey = "None";
		if(pMesh.Get())
		{
			MeshKey = string(pMesh->GetKey().begin(), pMesh->GetKey().end());
		}

		ImGui::Text("Mesh");
		ImGui::SameLine(120);
		ImGui::SetNextItemWidth(200);
		ImGui::InputText("##MeshName", (char*)MeshKey.c_str(), MeshKey.length() + 1, ImGuiInputTextFlags_ReadOnly);

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
					if (ASSET_TYPE::MESH == pAsset->GetType())
					{
						pMeshRender->SetMesh((AMesh*)pAsset.Get());
					}
				}
			}

			ImGui::EndDragDropTarget();
		}


		ImGui::SameLine();

		if (ImGui::Button("##MeshBtn", Vec2(19.f, 19.f)))
		{
			// 1. EditorMgr 에서 ListUI 를 찾아서 활성화 시킨다.
			ListUI* pListUI = (ListUI*)EditorMgr::GetInst()->FindUI("ListUI").Get();
			pListUI->SetActive(true);

			// 2. AssetMgr 에서 Mesh 이름 목록을 받아와야 한다.
			vector<wstring> vecNames;
			AssetMgr::GetInst()->GetAssetNames(ASSET_TYPE::MESH, vecNames);

			// 3. List 에 Mesh 이름 목록을 전달한다.
			pListUI->AddString(vecNames);

			// 4. List 에, 특정 항목이 선택됐을때 수행해야 하는 작업(함수) 를 등록시킨다.
			// - CallBack, Delegate
			pListUI->AddDelegate(this, (DELEGATE_1)&MeshRenderUI::SelectMesh);
		}
	}

	// Material
	{		
		Ptr<AMaterial> pMtrl = pMeshRender->GetMaterial(0);

		string MtrlKey = "None";	
		if(pMtrl.Get())
		{
			MtrlKey = string(pMtrl->GetKey().begin(), pMtrl->GetKey().end());
		}

		ImGui::Text("Material");
		ImGui::SameLine(120);
		ImGui::SetNextItemWidth(200);
		ImGui::InputText("##MtrlName", (char*)MtrlKey.c_str(), MtrlKey.length() + 1, ImGuiInputTextFlags_ReadOnly);

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
					if (ASSET_TYPE::MATERIAL == pAsset->GetType())
					{
						pMeshRender->SetMaterial((AMaterial*)pAsset.Get(), 0);
					}
				}				
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();
		if (ImGui::Button("##MtrlBtn", Vec2(19.f, 19.f)))
		{
			// 1. EditorMgr 에서 ListUI 를 찾아서 활성화 시킨다.
			ListUI* pListUI = (ListUI*)EditorMgr::GetInst()->FindUI("ListUI").Get();
			pListUI->SetActive(true);

			// 2. AssetMgr 에서 Material 이름 목록을 받아와야 한다.
			vector<wstring> vecNames;
			AssetMgr::GetInst()->GetAssetNames(ASSET_TYPE::MATERIAL, vecNames);

			// 3. List 에 Material 이름 목록을 전달한다.
			pListUI->AddString(vecNames);

			// 4. List 에, 특정 항목이 선택됐을때 수행해야 하는 작업(함수) 를 등록시킨다.
			// - CallBack, Delegate
			pListUI->AddDelegate(this, (DELEGATE_1)&MeshRenderUI::SelectMaterial);
		}
	}
}

void MeshRenderUI::SelectMesh(DWORD_PTR _ListUI)
{
	// ListUI 가 마지막으로 더블클릭한 문자열이 누군지 받아온다.
	ListUI* pListUI = (ListUI*)_ListUI;
	wstring MeshName = GetWString(pListUI->GetSelectedString());

	// 선택한 문자열에 해당하는 Mesh 를 AssetMgr 로부터 찾아낸다.
	Ptr<AMesh> pMesh = AssetMgr::GetInst()->Find<AMesh>(MeshName);

	// MeshRenerUI 가 타겟팅 하고있는 실제 컴포넌트가, 새롭게 선택한 Mesh 를 사용하도록 세팅해준다.
	Ptr<CMeshRender> pMeshRender = GetTargetObject()->MeshRender();
	pMeshRender->SetMesh(pMesh);
}

void MeshRenderUI::SelectMaterial(DWORD_PTR _ListUI)
{
	// ListUI 가 마지막으로 더블클릭한 문자열이 누군지 받아온다.
	ListUI* pListUI = (ListUI*)_ListUI;
	wstring MtrlName = GetWString(pListUI->GetSelectedString());

	// 선택한 문자열에 해당하는 AMaterial 를 AssetMgr 로부터 찾아낸다.
	Ptr<AMaterial> pMtrl = AssetMgr::GetInst()->Find<AMaterial>(MtrlName);

	// MeshRenerUI 가 타겟팅 하고있는 실제 컴포넌트가, 새롭게 선택한 AMaterial 를 사용하도록 세팅해준다.
	Ptr<CMeshRender> pMeshRender = GetTargetObject()->MeshRender();
	pMeshRender->SetMaterial(pMtrl, 0);
}
