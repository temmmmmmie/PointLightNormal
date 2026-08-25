#include "pch.h"
#include "TreeUI.h"

// ========
// TreeNode
// ========
void TreeNode::ClickCheck()
{
	// 클릭 검사
	if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		Owner->RegisterSelectedNode(this);
	}
}

void TreeNode::DragCheck()
{
	// Drag 체크
	if (false == Owner->m_IsDrag)
		return;

	if (ImGui::BeginDragDropSource())
	{
		ImGui::Text(Name.c_str());

		TreeNode* pThis = this;
		ImGui::SetDragDropPayload(Owner->GetParentUI()->GetUIName().c_str(), &pThis, sizeof(TreeNode*));

		Owner->RegisterDragNode(this);

		ImGui::EndDragDropSource();
	}
}

void TreeNode::DropCheck()
{
	if (false == Owner->m_IsDrop)
		return;

	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(Owner->m_DropPayloadKey.c_str());

		if (payload)
		{
			TreeNode* pDragNode = *((TreeNode**)payload->Data);			

			Owner->RegisterDropNode(this);

			Owner->CallDragDropDelegate(pDragNode, this);
		}

		ImGui::EndDragDropTarget();
	}
}

void TreeNode::Tick_UI()
{	
	UINT Flags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick
		       | ImGuiTreeNodeFlags_OpenOnArrow;
	
	string padding;

	// 자식노드가 없으면 LeafNode(단말노드)
	if (vecChildNode.empty())
	{
		Flags |= ImGuiTreeNodeFlags_Leaf;

		// 단말노드면서 Frame 노드인 경우 들여쓰기가 되기 때문에, 공백 3칸 보정 들어간다.
		if (m_Frame)
			padding = "   ";
	}	

	if (m_Frame)
		Flags |= ImGuiTreeNodeFlags_Framed;
	
	if(Owner->m_SelectedNode == this)
		Flags |= ImGuiTreeNodeFlags_Selected;

	// ID 문자열 추가
	char szID[50] = {};
	_itoa_s(GetID(), szID, 50, 10);

	// 노드의 출력 내용은, (패딩 + 이름 + ##ID)
	string strNodeName = string(padding + Name + "##");
	strNodeName += szID;

	if (ImGui::TreeNodeEx(strNodeName.c_str(), Flags))
	{		
		ClickCheck();

		DragCheck();

		DropCheck();

		for (size_t i = 0; i < vecChildNode.size(); ++i)
		{
			vecChildNode[i]->Tick_UI();
		}

		ImGui::TreePop();
	}

	else
	{
		ClickCheck();

		DragCheck();

		DropCheck();
	}
}


// ======
// TreeUI
// ======
TreeUI::TreeUI()
	: EditorUI("EditorUI")
	, m_ShowRoot(false)
	, m_Inst(nullptr)
	, m_SelectedFunc(nullptr)
	, m_IsDrag(false)
	, m_IsDrop(false)
{
	m_RootNode = new TreeNode;
	m_RootNode->Name = "RootNode";
	m_RootNode->Owner = this;
}

TreeUI::~TreeUI()
{
}

Ptr<TreeNode> TreeUI::AddNode(Ptr<TreeNode> _ParentNode, const string _Name, DWORD_PTR _Data)
{
	Ptr<TreeNode> pNewNode = new TreeNode;

	pNewNode->Owner = this;
	pNewNode->Name = _Name;
	pNewNode->Data = _Data;

	_ParentNode->AddChildNode(pNewNode);

	return pNewNode;
}

void TreeUI::RegisterSelectedNode(Ptr<TreeNode> _Node)
{
	m_SelectedNode = _Node;

	if (m_Inst && m_SelectedFunc)
	{
		(m_Inst->*m_SelectedFunc)((DWORD_PTR)_Node.Get());
	}
}

void TreeUI::CallDragDropDelegate(TreeNode* _DragNode, TreeNode* _DropNode)
{
	if (m_DragDropInst && m_DragDropFunc)
	{
		(m_DragDropInst->*m_DragDropFunc)((DWORD_PTR)_DragNode, (DWORD_PTR)_DropNode);
	}

	m_DragNode = nullptr;
	m_DropNode = nullptr;
}

void TreeUI::Tick_UI()
{
	if(m_ShowRoot)
		m_RootNode->Tick_UI();
	else
	{
		for (size_t i = 0; i < m_RootNode->vecChildNode.size(); ++i)
		{
			m_RootNode->vecChildNode[i]->Tick_UI();
		}
	}


	// 마우스를 뗀 프레임 == 드랍이 발생 했었어야함
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		// Drag 노드는 있는데, Drop 노드는 없는 경우
		if (nullptr != m_DragNode && nullptr == m_DropNode)
		{
			if(m_DragDropInst && m_DragDropFunc)
				(m_DragDropInst->*m_DragDropFunc)((DWORD_PTR)m_DragNode.Get(), (DWORD_PTR)nullptr);
		}

		m_DragNode = nullptr;
		m_DropNode = nullptr;		
	}
}
