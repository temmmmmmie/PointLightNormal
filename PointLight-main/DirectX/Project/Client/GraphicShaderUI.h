#pragma once
#include "AssetUI.h"
class GraphicShaderUI :
    public AssetUI
{
    virtual void Tick_UI() override;

public:
    GraphicShaderUI();
    virtual ~GraphicShaderUI();
};

