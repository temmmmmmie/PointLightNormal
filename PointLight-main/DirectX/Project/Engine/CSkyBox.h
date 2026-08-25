#pragma once
#include "CRenderComponent.h"

enum class SKYBOX_TYPE
{
    SPHERE,
    CUBE,
};

class CSkyBox :
    public CRenderComponent
{
private:
    Ptr<ATexture>   m_SkyTex;
    Ptr<ATexture>   m_SkyCubeTex;

    SKYBOX_TYPE     m_Type;


public:
    void SetSkyBoxType(SKYBOX_TYPE _Type);
    SKYBOX_TYPE GetSkyBoxType() { return m_Type; }
    void SetSkyBoxTexture(Ptr<ATexture> _Tex);

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

private:
    void CreateMaterial();

    CLONE(CSkyBox);
public:
    CSkyBox();
    virtual ~CSkyBox();
};

