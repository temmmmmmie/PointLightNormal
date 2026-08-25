#pragma once
#include "Asset.h"
#include "ATexture.h"

class ASprite :
    public Asset
{
private:
    Ptr<ATexture>   m_Atlas;
    Vec2            m_LeftTop;
    Vec2            m_Slice;
    Vec2            m_Offset;
   

public:
    GET(Ptr<ATexture>, Atlas);
    GET(Vec2, LeftTop);
    GET(Vec2, Slice);
    GET(Vec2, Offset);

    void SetAtlas(Ptr<ATexture> _Atlas);
    void SetLeftTop(Vec2 _Pixel);
    void SetSlice(Vec2 _Pixel);
    void SetOffset(Vec2 _Pixel);


public:
    virtual int Save(const wstring& _FilePath) override { return S_OK; };

private:
    virtual int Load(const wstring& _FilePath) override { return S_OK; };

    CLONE_DISABLE(ASprite);
public:
    ASprite();
    virtual ~ASprite();
};

