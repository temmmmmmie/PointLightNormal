#pragma once // 중복참조 방지

#include "ConstBuffer.h"
#include "ATexture.h"

class Device
	: public singleton<Device>
{
	SINGLE(Device)
private:
	HWND							m_hMainWnd;
	Vec2							m_RenderResolution;

	// Dx11 최상위 클래스
	ComPtr<ID3D11Device>			m_Device;				// GPU VRAM 메모리 할당 및 DX 관련객체 생성
	ComPtr<ID3D11DeviceContext>		m_Context;				// GPU 동작 명령 실행

	ComPtr<IDXGISwapChain>			m_SwapChain;			// GPU 메모리에(렌더타겟) 그려진 이미지를 Window 로 송출

	ComPtr<ID3D11RasterizerState>	m_RSState[(UINT)RS_TYPE::END];
	ComPtr<ID3D11DepthStencilState> m_DSState[(UINT)DS_TYPE::END];
	ComPtr<ID3D11BlendState>		m_BSState[(UINT)BS_TYPE::END];
	
	ComPtr<ID3D11SamplerState>		m_Sampler[3];

	Ptr<ConstBuffer>				m_arrCB[(UINT)CB_TYPE::END];



public:
	int Init(HWND _hMainWnd);

	void TargetClear();
	void Present() 
	{    
		// RenderTarget 에 그려진 그림을 Swapchain 의 목적지 윈도우로 출력
		m_SwapChain->Present(0, 0);
	}

	ComPtr<ID3D11Device> GetDevice() { return m_Device; }
	ComPtr<ID3D11DeviceContext> GetContext() { return m_Context; }
	Ptr<ConstBuffer> GetConstBuffer(CB_TYPE _Type) { return m_arrCB[(UINT)_Type]; }
	ComPtr<ID3D11RasterizerState> GetRSState(RS_TYPE _Type) { return m_RSState[(UINT)_Type]; }
	ComPtr<ID3D11DepthStencilState> GetDSState(DS_TYPE _Type) { return m_DSState[(UINT)_Type]; }
	ComPtr<ID3D11BlendState> GetBSState(BS_TYPE _Type) { return m_BSState[(UINT)_Type]; }

	GET(Vec2, RenderResolution);
							

private:
	int CreateSwapChain();
	int CreateView();
	void CreateRasterizerState();
	void CreateDepthStencilState();
	void CreateBlendState();
	void CreateConstBuffer();
	void CreateSampler();	

public:
	void BindingSampler();
};

