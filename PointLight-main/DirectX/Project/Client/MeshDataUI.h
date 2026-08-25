#pragma once
#include "AssetUI.h"
class MeshDataUI :
    public AssetUI
{
    virtual void Tick_UI() override;

public:
    MeshDataUI();
    virtual ~MeshDataUI();
};

