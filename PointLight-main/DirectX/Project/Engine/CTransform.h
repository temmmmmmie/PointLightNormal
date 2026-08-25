#pragma once
#include "Component.h"

class CTransform :
    public Component
{
private:
    Vec3    m_RelativePos;
    Vec3    m_RelativeScale;
    Vec3    m_RelativeRot;

    Vec3    m_LocalDir[(UINT)DIR::END]; // 부모를 고려하지 않은, 본인의 회전상태로만 계산한 방향벡터
    Vec3    m_WorldDir[(UINT)DIR::END]; // 부모의 상태까지 모두 적용시킨, 월드공간상에서의 최종 방향벡터

    bool    m_IndependentScale;
    bool    m_Changed;

    Matrix  m_matWorld;
    Matrix  m_matWorldInv;

public:
    GET(Vec3, RelativePos);
    GET(Vec3, RelativeScale);
    GET(Vec3, RelativeRot);
    GET_SET_REF(Matrix, matWorld);
    const Matrix& GetmatWorldInv() { return m_matWorldInv; }

    Vec3 GetWorldPos() { return m_matWorld.Translation(); }

    void SetIndependentScale(bool _Independent) { m_IndependentScale = _Independent; m_Changed = true; }
    bool IsIndependentScale() { return m_IndependentScale ; }

    void SetChange() { m_Changed = true; }
    bool IsChanged() { return m_Changed; }
    void Clear() { m_Changed = false; }

    void SetRelativePos(Vec3 _Pos) 
    {
        if (m_RelativePos == _Pos)
            return;
        m_Changed = true;
        m_RelativePos = _Pos; 
    }
    void SetRelativeScale(Vec3 _Scale) {
        if (m_RelativeScale == _Scale)
            return;
        m_Changed = true;
        m_RelativeScale = _Scale;
    }
    void SetRelativeRot(Vec3 _Rot) {
        if (m_RelativeRot == _Rot)
            return;
        m_Changed = true;
        m_RelativeRot = _Rot;
    }

    Vec3 GetLocalDir(DIR _Dir) { return m_LocalDir[(UINT)_Dir]; }
    Vec3 GetWorldDir(DIR _Dir) { return m_WorldDir[(UINT)_Dir]; }


    CTransform& operator = (const CTransform& _Other);

public:
    // 위치정보를 상수버퍼에 전달 및 바인딩
    void Binding();
    void BindingWithOutWorld();
    virtual void FinalTick() override;
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    CLONE(CTransform);
public:
    CTransform();
    virtual ~CTransform();
};

