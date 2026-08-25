#pragma once
#include "Entity.h"

// ∑ª¥ı≈∏∞Ÿ¿« ¡˝«’√º
class MRT :
    public Entity
{
private:
    Ptr<ATexture>   m_arrRT[8];
    int             m_RTCount;
    Vec4            m_Clear[8];

    D3D11_VIEWPORT  m_ViewPort;

    Ptr<ATexture>   m_DSTex;

public:
    void Create(Ptr<ATexture>* _RT, int _RTCount, Ptr<ATexture> _DSTex);
    void SetClearColor(Vec4* _ClearColor);

    void OMSet();
    void DepthOnlyOMSet();

    void Clear() { ClearRT(); ClearDS(); }
    void ClearRT();
    void ClearDS();

    UINT GetRTCount() { return m_RTCount; }
    Ptr<ATexture> GetRT(int _Idx) { return m_arrRT[_Idx]; }
    const D3D11_VIEWPORT& GetViewPort() { return m_ViewPort; }

    CLONE(MRT);
public:
    MRT();
    virtual ~MRT();
};

