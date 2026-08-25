#pragma once
#include <Engine/CScript.h>

class CEditorCamMoveScript :
    public CScript
{
private:
    float   m_Speed;

public:
    virtual void Tick() override;
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

private:
    void MoveCamera2D();
    void MoveCamera3D();


    CLONE(CEditorCamMoveScript);
public:
    CEditorCamMoveScript();
    virtual ~CEditorCamMoveScript();
};

