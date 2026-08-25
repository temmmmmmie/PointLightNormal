#include "pch.h"
#include "ContentUI.h"


#include <Engine/AssetMgr.h>
#include "TreeUI.h"

#include "EditorMgr.h"
#include "Inspector.h"


ContentUI::ContentUI()
	: EditorUI("ContentUI")
	, m_TreeUI(nullptr)
{
	m_TreeUI = new TreeUI;
	AddChildUI(m_TreeUI);
		
	// Tree 노드 클릭시 호출될 Delegate 등록
	m_TreeUI->AddSelectDelegate(this, (DELEGATE_1)&ContentUI::SelectAsset);

	// Tree Drag 기능 On
	m_TreeUI->SetDrag(true);

	// 트리 구성
	RenewContent();
}

ContentUI::~ContentUI()
{
}

void ContentUI::Tick_UI()
{
	if (AssetMgr::GetInst()->IsChanged())
	{
		RenewContent();
	}
}

void ContentUI::RenewContent()
{
	// 트리 초기화
	m_TreeUI->Clear();

	// 루트노드 가져옴
	Ptr<TreeNode> pRoot = m_TreeUI->GetRootNode();

	// 루트 밑으로 모든 에셋의 내용을 트리노드로 구성함
	for (UINT i = 0; i < (int)(ASSET_TYPE::END); ++i)
	{
		// 에셋 종류별 프레임 노드
		Ptr<TreeNode> pNewNode = m_TreeUI->AddNode(pRoot, ::GetString((ASSET_TYPE)i), 0);
		pNewNode->SetFrame(true);

		// 해당 에셋 모든 이름
		vector<wstring> vecNames;
		AssetMgr::GetInst()->GetAssetNames((ASSET_TYPE)i, vecNames);
		
		for (UINT j = 0; j < vecNames.size(); ++j)
		{						
			m_TreeUI->AddNode(pNewNode, GetString(vecNames[j]).c_str()
				           , (DWORD_PTR)AssetMgr::GetInst()->Find((ASSET_TYPE)i, vecNames[j]).Get());
		}	
	}
}

void ContentUI::SelectAsset(DWORD_PTR _Node)
{
	TreeNode* pNode = (TreeNode*)_Node;
	if (pNode->IsFrame())
		return;

	Asset* pSelectedAsset = (Asset*)pNode->GetData();

	FIND_UI(Inspector, "Inspector")->SetTargetAsset(pSelectedAsset);
}
