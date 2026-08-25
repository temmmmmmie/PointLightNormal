#pragma once
#include "ComponentUI.h"

class Light3DUI :
    public ComponentUI
{

public:
    virtual void Tick_UI() override;

public:
    Light3DUI();
    virtual ~Light3DUI();
};

