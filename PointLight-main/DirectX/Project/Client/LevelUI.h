#pragma once
#include "AssetUI.h"
class LevelUI :
    public AssetUI
{
    virtual void Tick_UI() override;

public:
    LevelUI();
    virtual ~LevelUI();
};

