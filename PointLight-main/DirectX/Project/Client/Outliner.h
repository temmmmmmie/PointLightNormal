#pragma once
#include "EditorUI.h"

class Outliner :
    public EditorUI
{
private:
    class TreeUI* m_Tree;

public:
    virtual void Tick_UI() override;

public:
    void RenewLevel();

private:
    void SelectObject(DWORD_PTR _Node);
    void AddChildObject(DWORD_PTR _Src, DWORD_PTR _Dst);
    void AddGameObject(class TreeNode* pParentNode, GameObject* _Object);


public:
    Outliner();
    virtual ~Outliner();
};

