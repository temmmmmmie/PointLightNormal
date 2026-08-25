#pragma once
#include <Engine/CScript.h>

class CMonsterScript :
    public CScript
{
private:
    float       m_Speed;

public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

public:
    void BeginOverlap(CCollider2D* _OwnCollider, GameObject* _OtherObject, CCollider2D* _OtherCollider);

    CLONE(CMonsterScript);
public:
    CMonsterScript();
    virtual ~CMonsterScript();
};

