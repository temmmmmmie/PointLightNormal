#pragma once
#include "AssetUI.h"
class ComputeShaderUI :
    public AssetUI
{
    virtual void Tick_UI() override;

public:
    ComputeShaderUI();
    virtual ~ComputeShaderUI();
};

