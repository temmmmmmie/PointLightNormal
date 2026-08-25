#pragma once
#include "AssetUI.h"
class FlipbookUI :
    public AssetUI
{
    virtual void Tick_UI() override;

public:
    FlipbookUI();
    virtual ~FlipbookUI();
};

