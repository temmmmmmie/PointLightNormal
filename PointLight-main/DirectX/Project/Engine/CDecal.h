#pragma once
#include "CRenderComponent.h"

enum class DECAL_MODE
{
    COLOR,
    EMISSIVE,
};

class CDecal :
    public CRenderComponent
{
private:
    Ptr<ATexture>   m_DecalTex;
    DECAL_MODE      m_Mode;
    Ptr<GameObject> m_Object;

public:
    void SetDecalTex(Ptr<ATexture> _Tex);
    void SetDecalMode(DECAL_MODE _Mode) { m_Mode = _Mode; }
    void SetSpecifiedObject(Ptr<GameObject> _Object);

private:
    void CreateMaterial();

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

public:
    CLONE(CDecal);
public:
    CDecal();
    virtual ~CDecal();
};

