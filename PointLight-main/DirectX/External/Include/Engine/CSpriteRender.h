#pragma once
#include "CRenderComponent.h"

#include "ASprite.h"

class CSpriteRender :
    public CRenderComponent
{
private:
    Ptr<ASprite>    m_Sprite;

public:
    GET_SET(Ptr<ASprite>, Sprite);

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

private:
    void CreateMaterial();

public:
    CLONE(CSpriteRender);
    CSpriteRender();
    virtual ~CSpriteRender();
};

