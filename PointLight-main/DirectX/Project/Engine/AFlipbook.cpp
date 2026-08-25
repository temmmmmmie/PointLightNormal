#include "pch.h"
#include "AFlipbook.h"


AFlipbook::AFlipbook()
	: Asset(ASSET_TYPE::FLIPBOOK)
	, m_PanelSize(Vec2(100.f, 100.f))
{
}

AFlipbook::~AFlipbook()
{
}

void AFlipbook::SetSprite(int _Idx, Ptr<ASprite> _Sprite)
{
	if (m_vecSprites.size() <= _Idx)
	{
		m_vecSprites.resize(_Idx + 1);
	}

	m_vecSprites[_Idx] = _Sprite;

	// PanelSize 는 Flipbook 을 구성하고 있는 모든 
	// 스프라이트들 중에서 제일큰 스프라이트 보다도 더 크가나 같아야 한다.
	if (nullptr != _Sprite)
	{
		if (m_PanelSize.x < _Sprite->GetSlice().x)
			m_PanelSize.x = _Sprite->GetSlice().x;

		if (m_PanelSize.y < _Sprite->GetSlice().y)
			m_PanelSize.y = _Sprite->GetSlice().y;
	}
}

void AFlipbook::SetPanelSize(Vec2 _PanelSize)
{
	m_PanelSize = _PanelSize;
}

int AFlipbook::Save(const wstring& _FilePath)
{
	return 0;
}

int AFlipbook::Load(const wstring& _FilePath)
{
	return 0;
}
