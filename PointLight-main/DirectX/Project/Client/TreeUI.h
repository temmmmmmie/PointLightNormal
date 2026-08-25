#pragma once
#include "EditorUI.h"

struct TreeNode
    : public Entity
{
private:
    class TreeUI*           Owner;

    string                  Name;
    DWORD_PTR               Data;

    TreeNode*               Parent;
    vector<Ptr<TreeNode>>   vecChildNode;

    bool                    m_Frame;

public:
    const string& GetNodeName() { return Name; }
    DWORD_PTR GetData() { return Data; }

private:
    void ClickCheck();
    void DragCheck();
    void DropCheck();

public:
    void AddChildNode(Ptr<TreeNode> _ChildNode)
    {
        vecChildNode.push_back(_ChildNode);
        _ChildNode->Parent = this;
    }
    void SetFrame(bool _Frame) { m_Frame = _Frame; }
    bool IsFrame() { return m_Frame; }

public:
    void Tick_UI();

public:
    CLONE(TreeNode);
    TreeNode()
        : Owner(nullptr)
        , Data(0)
        , Parent(nullptr)
        , m_Frame(false)
    {}
    ~TreeNode() {}

    friend class TreeUI;
};


class TreeUI :
    public EditorUI
{
private:
    Ptr<TreeNode>   m_RootNode;
    Ptr<TreeNode>   m_SelectedNode;
    bool            m_ShowRoot;
    bool            m_IsDrag;
    bool            m_IsDrop;
    string          m_DropPayloadKey;

    EditorUI*       m_Inst;
    DELEGATE_1      m_SelectedFunc;

    Ptr<TreeNode>   m_DragNode;
    Ptr<TreeNode>   m_DropNode;

    EditorUI*       m_DragDropInst;
    DELEGATE_2      m_DragDropFunc;


public:
    Ptr<TreeNode> GetRootNode() { return m_RootNode; }
    Ptr<TreeNode> AddNode(Ptr<TreeNode> _ParentNode, const string _Name, DWORD_PTR _Data);
    void ShowRoot(bool _Show) { m_ShowRoot = _Show; }
    void Clear() { m_RootNode = new TreeNode; m_RootNode->Name = "RootNode"; }
    void SetDrag(bool _Drag) { m_IsDrag = _Drag; }
    void SetDrop(bool _Drop, const string& _PayloadKey) { m_IsDrop = _Drop; m_DropPayloadKey = _PayloadKey; }

public:
    void AddSelectDelegate(EditorUI* _Inst, DELEGATE_1 _Func) { m_Inst = _Inst; m_SelectedFunc = _Func; }
    void AddDragDropDelegate(EditorUI* _Inst, DELEGATE_2 _Func) { m_DragDropInst = _Inst; m_DragDropFunc = _Func; }


private:
    void RegisterSelectedNode(Ptr<TreeNode> _Node);
    void RegisterDragNode(Ptr<TreeNode> _Node) { m_DragNode = _Node; }
    void RegisterDropNode(Ptr<TreeNode> _Node) { m_DropNode = _Node; }
    void CallDragDropDelegate(TreeNode* _DragNode, TreeNode* _DropNode);

public:
    virtual void Tick_UI() override;

public:
    TreeUI();
    virtual ~TreeUI();

    friend class TreeNode;
};

