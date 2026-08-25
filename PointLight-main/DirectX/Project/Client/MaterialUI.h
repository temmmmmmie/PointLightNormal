#pragma once
#include "AssetUI.h"
class MaterialUI :
    public AssetUI
{

public:
    virtual void Tick_UI() override;

private:
    void ShaderParam();

public:
    MaterialUI();
    virtual ~MaterialUI();
};

