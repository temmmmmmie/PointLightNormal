#include "pch.h"
#include "Outliner.h"

#include <Engine/LevelMgr.h>
#include "TreeUI.h"
#include "EditorMgr.h"
#include "Inspector.h"

Outliner::Outliner()
	: EditorUI("Outliner")
	, m_Tree(nullptr)
{
	m_Tree = new TreeUI;
	AddChildUI(m_Tree);

	// Tree 노드 클릭시 호출될 Delegate 등록
	m_Tree->AddSelectDelegate(this, (DELEGATE_1)&Outliner::SelectObject);
	m_Tree->AddDragDropDelegate(this, (DELEGATE_2)&Outliner::AddChildObject);

	// DragDrop 기능
	m_Tree->SetDrag(true);
	m_Tree->SetDrop(true, GetUIName().c_str()); // Self DragDrop
	
}

Outliner::~Outliner()
{
}


void Outliner::Tick_UI()
{
	// 레벨안에 있는 오브젝트의 상태를 Tree 에 반영
	if(LevelMgr::GetInst()->IsChanged())
		RenewLevel();
}

void Outliner::RenewLevel()
{
	// 트리 초기화
	m_Tree->Clear();
	
	// LevelMgr 로 부터 현재 레벨을 가져온다.
	Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurrentLevel();

	if (nullptr == pCurLevel)
		return;

	// 루트노드를 가져온다.
	Ptr<TreeNode> pRoot = m_Tree->GetRootNode();

	// 레벨안에 있는 물체들의 정보를 트리로 구성한다.
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		Ptr<Layer> pLayer = pCurLevel->GetLayer(i);

		// 각 레이어 소속 최상위 부모 오브젝트를 가져온다.
		const vector<Ptr<GameObject>>& vecObj = pLayer->GetParentObjects();

		for (size_t j = 0; j < vecObj.size(); ++j)
		{
			AddGameObject(pRoot.Get(), vecObj[j].Get());
		}
	}
}



void Outliner::AddGameObject(TreeNode* pParentNode, GameObject* _Object)
{
	TreeNode* pNewNode = m_Tree->AddNode(pParentNode, GetString(_Object->GetName()).c_str(), (DWORD_PTR)_Object).Get();

	const vector<Ptr<GameObject>>& vecChild = _Object->GetChild();

	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		AddGameObject(pNewNode, vecChild[i].Get());
	}
}

void Outliner::SelectObject(DWORD_PTR _Node)
{
	TreeNode* pNode = (TreeNode*)_Node;

	GameObject* pSelectObject = (GameObject*)pNode->GetData();

	FIND_UI(Inspector, "Inspector")->SetTargetObject(pSelectObject);
}

void Outliner::AddChildObject(DWORD_PTR _Src, DWORD_PTR _Dst)
{
	if (_Src == _Dst)
		return;

	TreeNode* pSrcNode = (TreeNode*)_Src;
	TreeNode* pDstNode = (TreeNode*)_Dst;

	GameObject* pSrcObj = (GameObject*)pSrcNode->GetData();
	GameObject* pDstObj = nullptr;

	if (nullptr != pDstNode)
		pDstObj = (GameObject*)pDstNode->GetData();

	AddChild(pSrcObj, pDstObj);
}