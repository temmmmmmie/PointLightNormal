#pragma once
#include "CRenderComponent.h"

#include "StructuredBuffer.h"
#include "ATexture.h"
#include "ASprite.h"
#include "ASprite.h"

struct tTileInfo
{
    Vec2 vLeftTop;
    Vec2 vSlice;
};

class CTileRender :
    public CRenderComponent
{
private:
    UINT                    m_Col;
    UINT                    m_Row;
    Vec2                    m_TileSize;
    Ptr<StructuredBuffer>   m_Buffer; // 각각의 타일 정보를 담은 구조화버퍼
    Ptr<ATexture>           m_Atlas;
    vector<tTileInfo>       m_vecTileInfo;
    
public:
    void SetColRow(UINT _Col, UINT _Row)
    {
        m_Col = _Col; m_Row = _Row; 
        m_vecTileInfo.resize(m_Col * m_Row);
    }

    void SetTileSize(Vec2 _Size) { m_TileSize = _Size; }
    void SetAltas(Ptr<ATexture> _Atlas) { m_Atlas = _Atlas; }
    int SetSprite(UINT _Col, UINT _Row, Ptr<ASprite> _Sprite);


public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

private:
    void CreateMaterial();
    void SetBuffer();

public:
    CLONE(CTileRender)
    CTileRender();
    CTileRender(const CTileRender& _Origin);
    virtual ~CTileRender();
};

