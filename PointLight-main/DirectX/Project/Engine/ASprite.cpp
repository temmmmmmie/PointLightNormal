#include "pch.h"
#include "ASprite.h"

ASprite::ASprite()
	: Asset(ASSET_TYPE::SPRITE)
{
}

ASprite::~ASprite()
{
}

void ASprite::SetAtlas(Ptr<ATexture> _Atlas)
{
	m_Atlas = _Atlas;

	m_LeftTop = Vec2(0.f, 0.f);
	m_Slice = Vec2(0.f, 0.f);
	m_Offset = Vec2(0.f, 0.f);
}

void ASprite::SetLeftTop(Vec2 _Pixel)
{
	assert(nullptr != m_Atlas);
	m_LeftTop = Vec2(_Pixel.x / (float)m_Atlas->GetWidth(), _Pixel.y / (float)m_Atlas->GetHeight());
}

void ASprite::SetSlice(Vec2 _Pixel)
{
	assert(nullptr != m_Atlas);
	m_Slice = Vec2(_Pixel.x / (float)m_Atlas->GetWidth(), _Pixel.y / (float)m_Atlas->GetHeight());
}

void ASprite::SetOffset(Vec2 _Pixel)
{
	assert(nullptr != m_Atlas);
	m_Offset = Vec2(_Pixel.x / (float)m_Atlas->GetWidth(), _Pixel.y / (float)m_Atlas->GetHeight());
}
