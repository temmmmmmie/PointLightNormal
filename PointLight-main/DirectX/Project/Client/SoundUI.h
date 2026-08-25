#pragma once
#include "AssetUI.h"
class SoundUI :
    public AssetUI
{
    virtual void Tick_UI() override;

public:
    SoundUI();
    virtual ~SoundUI();
};

