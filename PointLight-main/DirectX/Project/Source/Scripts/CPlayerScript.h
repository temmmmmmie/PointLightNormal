#pragma once
#include <Engine/CScript.h>


class CPlayerScript :
    public CScript
{
private:
    float           m_Speed;
    float           m_Pow;
    Ptr<APrefab>    m_MissilePref;


public:
    void SetSpeed(float _Speed) { m_Speed = _Speed; }
    void SetMissilePrefab(Ptr<APrefab> _Prefab) { m_MissilePref = _Prefab; }

public:
    void BeginOverlap(CCollider2D* _OwnCollider, GameObject* _OtherObject, CCollider2D* _OtherCollider);
    void Overlap(CCollider2D* _OwnCollider, GameObject* _OtherObject, CCollider2D* _OtherCollider);
    void EndOverlap(CCollider2D* _OwnCollider, GameObject* _OtherObject, CCollider2D* _OtherCollider);


public:
    virtual void Begin() override;
    virtual void Tick() override;

    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;


    CLONE(CPlayerScript);
public:
    CPlayerScript();
    virtual ~CPlayerScript();
};

