#pragma once
#include "CRenderComponent.h"

#include "AFlipbook.h"

class CFlipbookRender :
    public CRenderComponent
{
private:
    vector<Ptr<AFlipbook>>  m_vecFlipbook;
    Ptr<AFlipbook>          m_CurFlipbook;
    int                     m_SpriteIdx;
    float                   m_AccTime;
    float                   m_FPS;
    int                     m_RepeatCount;      // 반복 횟수
    bool                    m_Finish;
    bool                    m_IsEmissive;       // 스스로 발광체인지 체크

public:
    void AddFlipbook(Ptr<AFlipbook> _Flipbook);
    void SetFlipbook(int _Idx, Ptr<AFlipbook> _Flipbook);
    void Play(int _FlipbookIdx, float _FPS, int _RepeatCount = 0, int _StartSpriteIdx = 0);
    void SetEmissive(bool _Emissive) { m_IsEmissive = _Emissive; }

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;


private:
    void CreateMaterial();

public:
    CLONE(CFlipbookRender);
    CFlipbookRender();
    virtual ~CFlipbookRender();
};

