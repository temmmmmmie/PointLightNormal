#pragma once
#include "EditorUI.h"

class ContentUI :
    public EditorUI
{
private:
    class TreeUI*   m_TreeUI;

public:
    virtual void Tick_UI() override;
    void RenewContent();

private:
    void SelectAsset(DWORD_PTR _Node);

public:
    ContentUI();
    virtual ~ContentUI();
};

