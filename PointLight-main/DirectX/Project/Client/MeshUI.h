#pragma once
#include "AssetUI.h"

class MeshUI :
    public AssetUI
{
private:

public:
    virtual void Tick_UI() override;

public:
    MeshUI();
    virtual ~MeshUI();
};

