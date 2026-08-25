#pragma once
#include "Component.h"

#include "CScript.h"

typedef void(CScript::* OVERLAP)(class CCollider2D*, class GameObject*, class CCollider2D*);

struct COLLIDER2D_DELEGATE
{
    CScript*    OverlapInst;
    OVERLAP     BeginOverlap;
    OVERLAP     Overlap;
    OVERLAP     EndOverlap;
};

class CCollider2D :
    public Component
{
private:
    Vec3        m_Offset;           // 충돌체가 오브젝트로부터 상대적인 좌표
    Vec3        m_Scale;            // 충돌체의 배율(오브젝트 크기대비)
    bool        m_IndependentScale; // 독립적인 크기를 가지는 옵션
    int         m_OverlapCount;     // 다른 충돌체와 겹쳐있는 횟수
    Matrix      m_matWorld;         // 충돌체의 월드행렬
    vector<COLLIDER2D_DELEGATE> m_vecDelegate;  // 이벤트 발생시 호출시킬 Delegate


public:
    Vec2 GetOffset() { return Vec2(m_Offset.x, m_Offset.y); }
    Vec2 GetSclae() { return Vec2(m_Scale.x, m_Scale.y); }

    void SetOffset(Vec2 _Offset) { m_Offset = Vec3(_Offset.x, _Offset.y, 0.f); }
    void SetScale(Vec2 _Scale) { m_Scale = Vec3(_Scale.x, _Scale.y, 1.f); }

    bool GetIndependentScale() { return m_IndependentScale; }
    void SetIndependentScale(bool _Independent) { m_IndependentScale = _Independent; }

    const Matrix& GetWorldMat() { return m_matWorld; }

    void AddDelegate(CScript* _Inst, OVERLAP _BeginOverlap, OVERLAP _Overlap, OVERLAP _EndOverlap)
    {
        m_vecDelegate.push_back(COLLIDER2D_DELEGATE{ _Inst , _BeginOverlap , _Overlap , _EndOverlap });
    }


public:
    virtual void FinalTick() override;
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

private:
    void BeginOverlap(CCollider2D* _Other);
    void Overlap(CCollider2D* _Other);
    void EndOverlap(CCollider2D* _Other);
    
    CLONE(CCollider2D);
public:
    CCollider2D();
    CCollider2D(const CCollider2D& _Origin);
    virtual ~CCollider2D();

    friend class CollisionMgr;
};

