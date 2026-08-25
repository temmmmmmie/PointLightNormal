#pragma once
#include "AssetUI.h"
class PrefabUI :
    public AssetUI
{
    virtual void Tick_UI() override;

public:
    PrefabUI();
    virtual ~PrefabUI();
};

