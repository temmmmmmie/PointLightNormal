#pragma once
#include "EditorUI.h"

#include <Engine/GameObject.h>

class ComponentUI :
    public EditorUI
{
private:
    Ptr<GameObject>         m_TargetObject;
    const COMPONENT_TYPE    m_ComType;


public:
    virtual void  Tick_UI() override;

protected:
    void OutputComponentName();

public:
    COMPONENT_TYPE GetComponentType() { return m_ComType; }
    void SetTargetObject(Ptr<GameObject> _Object);
    Ptr<GameObject> GetTargetObject() { return m_TargetObject; }

public:
    ComponentUI(const string _Name, COMPONENT_TYPE _Type);
    virtual ~ComponentUI();
};

