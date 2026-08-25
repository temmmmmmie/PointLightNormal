#pragma once
#include "RenderComponentUI.h"

class MeshRenderUI :
    public RenderComponentUI
{
private:


public:
    virtual void Tick_UI() override;

private:
    void SelectMesh(DWORD_PTR _ListUI);
    void SelectMaterial(DWORD_PTR _ListUI);

public:
    MeshRenderUI();
    virtual ~MeshRenderUI();
};

