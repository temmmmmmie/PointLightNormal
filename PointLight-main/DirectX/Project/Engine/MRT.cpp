#include "pch.h"
#include "MRT.h"

MRT::MRT()
	: m_RTCount(0)
	, m_ViewPort{}
{
}

MRT::~MRT()
{
}

void MRT::Create(Ptr<ATexture>* _RT, int _RTCount, Ptr<ATexture> _DSTex)
{
	assert(_RTCount > 0);

	Vec2 vResol = Vec2(_RT[0]->GetWidth(), _RT[0]->GetHeight());

	m_RTCount = _RTCount;
	for (int i = 0; i < _RTCount; ++i)
	{
		// MRT 를 구성하는 모든 렌더타겟들은 해상도가 일치해야 한다.
		assert(vResol == Vec2(_RT[i]->GetWidth(), _RT[i]->GetHeight()));

		m_arrRT[i] = _RT[i];
	}

	// MRT 를 구성하고 있는 렌더타겟중에 대표격으로 
	// 0번 타겟의 해상도로 ViewPort 를 성정한다
	m_ViewPort.Width = _RT[0]->GetWidth();
	m_ViewPort.Height = _RT[0]->GetHeight();
	m_ViewPort.MinDepth = 0.f;
	m_ViewPort.MaxDepth = 1.f;


	m_DSTex = _DSTex;	
}

void MRT::SetClearColor(Vec4* _ClearColor)
{
	for (int i = 0; i < m_RTCount; ++i)
	{
		m_Clear[i] = _ClearColor[i];
	}
}

void MRT::OMSet()
{
	ID3D11RenderTargetView* arrRTView[8] = {};
	for (int i = 0; i < m_RTCount; ++i)
	{
		arrRTView[i] = 	m_arrRT[i]->GetRTV().Get();
	}

	if(nullptr == m_DSTex)
		CONTEXT->OMSetRenderTargets(m_RTCount, arrRTView, nullptr);
	else
		CONTEXT->OMSetRenderTargets(m_RTCount, arrRTView, m_DSTex->GetDSV().Get());		
	
	CONTEXT->RSSetViewports(1, &m_ViewPort);
}

void MRT::DepthOnlyOMSet()
{
	CONTEXT->OMSetRenderTargets(0, nullptr, m_DSTex->GetDSV().Get());

	CONTEXT->RSSetViewports(1, &m_ViewPort);
}


void MRT::ClearRT()
{
	// RenderTarget 클리어
	for (int i = 0; i < m_RTCount; ++i)
	{
		CONTEXT->ClearRenderTargetView(m_arrRT[i]->GetRTV().Get(), m_Clear[i]);
	}
}

void MRT::ClearDS()
{
	// DepthStencilTarget 클리어
	// 깊이의 범위는 0 ~ 1
	// 최대값으로 초기화 해야 그것보다 가까운 물체들이 깊이테스트를 통과해서 그려질 수 있다.
	// 통과한 물체는 자신이 그려지는 영역에 해당하는 부분에 깊이값을 자신의 것으로 대체한다.
	// (다음 렌더링 될 물체가 그것을 기반으로 깊이테스트를 할 수 있도록 하기 위함)
	CONTEXT->ClearDepthStencilView(m_DSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}
