#pragma once
#include "AssetUI.h"
class SpriteUI :
    public AssetUI
{
    virtual void Tick_UI() override;

public:
    SpriteUI();
    virtual ~SpriteUI();
};

