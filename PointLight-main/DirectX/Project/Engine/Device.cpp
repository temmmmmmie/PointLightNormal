#include "pch.h"
#include "Device.h"

#include "AssetMgr.h"

Device::Device()
    : m_hMainWnd(nullptr)
{
}

Device::~Device()
{
       
}

int Device::Init(HWND _hMainWnd)
{
    // Main Window 확인
    m_hMainWnd = _hMainWnd;

    // Main Window 해상도 확인
    RECT rt = {};
    GetClientRect(m_hMainWnd, &rt);
    m_RenderResolution = Vec2((float)(rt.right - rt.left), (float)(rt.bottom - rt.top));

    // 전역상수 데이터에, 렌더링 타겟 해상도 갱신
    g_Global.RenderResolution = m_RenderResolution;

    // Flag
    UINT Flag = 0;
    
#ifdef _DEBUG
    Flag = D3D11_CREATE_DEVICE_DEBUG;
#endif

    // ID3D11Device, ID3D11DeviceContext 생성
    if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, Flag
        , 0, 0, D3D11_SDK_VERSION, m_Device.GetAddressOf(), nullptr, m_Context.GetAddressOf())))
    {
        return E_FAIL;
    }

    // SwapChain + RenderTargetTexture 생성
    if (FAILED(CreateSwapChain()))
    {
        MessageBox(m_hMainWnd, L"SwapChain 생성 실패", L"Device 초기화 실패", MB_OK);
        return E_FAIL;
    }


    // RenderTargetView, DepthStencilTexture, DepthStencilView
    if (FAILED(CreateView()))
    {
        MessageBox(m_hMainWnd, L"View 생성 실패", L"Device 초기화 실패", MB_OK);
        return E_FAIL;
    }         

    // 레스터라이저 스테이트 생성하기
    CreateRasterizerState();

    // 깊이스텐실 스테이트 생성
    CreateDepthStencilState();

    // 블렌드스테이트 생성
    CreateBlendState();

    // 상수버퍼 생성해두기
    CreateConstBuffer();

    // 샘플러 생성 및 바인딩
    CreateSampler();
    BindingSampler();

    return S_OK;
}

void Device::TargetClear()
{
    
}

int Device::CreateSwapChain()
{
    DXGI_SWAP_CHAIN_DESC Desc = {};

    Desc.OutputWindow = m_hMainWnd;  // RenderTarget 이 출력될 윈도우
    Desc.Windowed = true;            // 전체화면 모드 OR 윈도우 모드

    // RenderTarget 설정 옵션
    // RenderTarget 개수
    Desc.BufferCount = 1;
     
    // RenderTarget 해상도를 출력 윈도우 비트맵이랑 동일한 크기로 설정
    Desc.BufferDesc.Width = (UINT)m_RenderResolution.x;
    Desc.BufferDesc.Height = (UINT)m_RenderResolution.y;

    // 렌더타겟 픽셀 포맷
    Desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

    // 화면 갱신률 60 프레임
    Desc.BufferDesc.RefreshRate.Numerator   = 60;
    Desc.BufferDesc.RefreshRate.Denominator = 1;

    // 화면 렌더링 옵션(기본값)
    Desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    Desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

    // 렌더타겟 용도 설정(렌더타겟으로)
    Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    
    // 장면 출력후, 이전 그림 유지할 필요 없음
    Desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    Desc.Flags = 0;
    Desc.SampleDesc.Count = 1;
    Desc.SampleDesc.Quality = 0;
    
    // DXGIDevice
    // DXGIAdapter
    // DXGIFactory

    ComPtr<IDXGIDevice> pDXGIDevice = nullptr;
    ComPtr<IDXGIAdapter> pAdapter = nullptr;
    ComPtr<IDXGIFactory> pFactory = nullptr;

    m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)pDXGIDevice.GetAddressOf());
    pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)pAdapter.GetAddressOf());
    pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)pFactory.GetAddressOf());

    if (FAILED(pFactory->CreateSwapChain(m_Device.Get(), &Desc, m_SwapChain.GetAddressOf())))
    {
        return E_FAIL;
    }


    return S_OK;
}

int Device::CreateView()
{
    // RenderTargetView
    // SwapChain 으로부터 RenderTarget 주소값을 받아온다.
    ComPtr<ID3D11Texture2D> Tex2D;
    m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)Tex2D.GetAddressOf());

    // RenderTarget 텍스쳐 생성 후 에셋매니저에 등록
    Ptr<ATexture> pRTTex = new ATexture;
    pRTTex->Create(Tex2D);
    AssetMgr::GetInst()->AddAsset(L"RenderTargetTex", pRTTex.Get());

    // DepthStencil 용 텍스쳐 생성
    Ptr<ATexture> pDSTex = new ATexture;   
    pDSTex->Create((UINT)m_RenderResolution.x, (UINT)m_RenderResolution.y
                 , DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT);

    // DepthStencil 텍스쳐 생성 후 에셋매니저에 등록
    AssetMgr::GetInst()->AddAsset(L"DepthStencilTex", pDSTex.Get());

    return S_OK;
}

void Device::CreateRasterizerState()
{
    D3D11_RASTERIZER_DESC Desc = {};

    // Cull Back
    m_RSState[(UINT)RS_TYPE::CULL_BACK] = nullptr;

    // Cull Front
    Desc.CullMode = D3D11_CULL_FRONT;
    Desc.FillMode = D3D11_FILL_SOLID;
    DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::CULL_FRONT].GetAddressOf());

    // Cull None
    Desc.CullMode = D3D11_CULL_NONE;
    Desc.FillMode = D3D11_FILL_SOLID;
    DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::CULL_NONE].GetAddressOf());

    // WireFrame
    Desc.CullMode = D3D11_CULL_NONE;
    Desc.FillMode = D3D11_FILL_WIREFRAME;
    DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::WIRE_FRAME].GetAddressOf());
}

void Device::CreateDepthStencilState()
{
    m_DSState[(UINT)DS_TYPE::LESS] = nullptr;

    D3D11_DEPTH_STENCIL_DESC Desc = {};

    // Less Equal
    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    Desc.StencilEnable = false;
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::LESS_EQUAL].GetAddressOf());
    
    // NoTest
    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    Desc.StencilEnable = false;
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::NO_TEST].GetAddressOf());

    // NoWrite
    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_LESS;
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    Desc.StencilEnable = false;
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::NO_WRITE].GetAddressOf());

    // NoTest NoWrite
    Desc.DepthEnable = false;
    Desc.StencilEnable = false;
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::NO_TEST_NO_WRITE].GetAddressOf());

    
    // VOLUME_GREATER - 뒷면을 렌더링 할때만 사용할 예정
    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_GREATER;
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // VolueMesh 는 자신의 깊이를 남기지 않는다.

    Desc.StencilEnable = true;    
    Desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    Desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;// 깊이, 스텐실 모두 통과
    Desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
    Desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;

    Desc.FrontFace = Desc.BackFace;
    Desc.StencilReadMask = 0xffffffff;
    Desc.StencilWriteMask = 0xffffffff;
    if (FAILED(DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::VOLUME_GREATER].GetAddressOf())))
    {
        assert(nullptr);
    }


    // VOLUME_LESS - 앞면을 렌더링 할때만 사용할 예정
    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_LESS;
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // VolueMesh 는 자신의 깊이를 남기지 않는다.

    Desc.StencilEnable = true;
    Desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    Desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;// 깊이, 스텐실 모두 통과
    Desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
    Desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;

    Desc.BackFace = Desc.FrontFace;
    Desc.StencilReadMask = 0xffffffff;
    Desc.StencilWriteMask = 0xffffffff;
    if (FAILED(DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::VOLUME_LESS].GetAddressOf())))
    {
        assert(nullptr);
    }


    // VOLUME_CHECK
    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_GREATER;
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // VolueMesh 는 자신의 깊이를 남기지 않는다.

    Desc.StencilEnable = true;
    Desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    Desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;// 깊이, 스텐실 모두 통과
    Desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    Desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

    Desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    Desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_DECR;   // Greater 기준, 앞면 렌더링이 성공하면 오히려 실패
    Desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; // Greater 기준, 앞면 렌더링이 실패하면 오히려 실패
    Desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    
    Desc.StencilReadMask = 0xffffffff;
    Desc.StencilWriteMask = 0xffffffff;
    if (FAILED(DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::VOLUME_CHECK].GetAddressOf())))
    {
        assert(nullptr);
    }

    // VOLUME_EQUAL
    Desc.DepthEnable = false;
    Desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // VolueMesh 는 자신의 깊이를 남기지 않는다.

    Desc.StencilEnable = true;
    Desc.BackFace.StencilFunc = D3D11_COMPARISON_LESS;
    Desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;// 깊이, 스텐실 모두 통과
    Desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
    Desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;

    Desc.FrontFace = Desc.BackFace;
    Desc.StencilReadMask = 0xffffffff;
    Desc.StencilWriteMask = 0xffffffff;
    if (FAILED(DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::VOLUME_EQUAL].GetAddressOf())))
    {
        assert(nullptr);
    }
}

void Device::CreateBlendState()
{
    // =======
    // Default
    // =======
    m_BSState[(UINT)BS_TYPE::DEFAULT] = nullptr;

    // ===============
    // AlphaBlendState
    // ===============
    D3D11_BLEND_DESC Desc = {};
    Desc.AlphaToCoverageEnable = false; 
    Desc.IndependentBlendEnable = false; 
    Desc.RenderTarget[0].BlendEnable = true;

    // RGB 끼리 혼합공식
    Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

    // A 끼리 혼합 공식
    Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

    Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::ALPHABLEND].GetAddressOf());

    // =======
    // ONE ONE
    // =======
    Desc.AlphaToCoverageEnable = false;
    Desc.IndependentBlendEnable = false;
    Desc.RenderTarget[0].BlendEnable = true;

    // RGB 끼리 혼합공식
    Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

    // A 끼리 혼합 공식
    Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

    Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::ONE_ONE].GetAddressOf());



    // =====
    // Decal
    // =====
    Desc.AlphaToCoverageEnable = false;
    Desc.IndependentBlendEnable = true;
    Desc.RenderTarget[0].BlendEnable = true;
        
    // --- 0 번 타겟 ---
    // RGB 끼리 혼합공식
    Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

    // A 끼리 혼합 공식
    Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    // --- 1 번 타겟 ---
    // RGB 끼리 혼합공식
    Desc.RenderTarget[1].BlendOp = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[1].SrcBlend = D3D11_BLEND_ONE;
    Desc.RenderTarget[1].DestBlend = D3D11_BLEND_ONE;

    // A 끼리 혼합 공식
    Desc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ONE;
    Desc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_ZERO;
    Desc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::DECAL].GetAddressOf());


}


void Device::CreateConstBuffer()
{
    m_arrCB[(UINT)CB_TYPE::TRANFORM] = new ConstBuffer;
    m_arrCB[(UINT)CB_TYPE::TRANFORM]->Create(CB_TYPE::TRANFORM, sizeof(tTransform));
    
    m_arrCB[(UINT)CB_TYPE::MATERIAL] = new ConstBuffer;
    m_arrCB[(UINT)CB_TYPE::MATERIAL]->Create(CB_TYPE::MATERIAL, sizeof(tMtrlConst));

    m_arrCB[(UINT)CB_TYPE::GLOBAL] = new ConstBuffer;
    m_arrCB[(UINT)CB_TYPE::GLOBAL]->Create(CB_TYPE::GLOBAL, sizeof(tGlobalData));
}

void Device::CreateSampler()
{
    D3D11_SAMPLER_DESC Desc = {};
    Desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    Desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    Desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    Desc.Filter = D3D11_FILTER_ANISOTROPIC; // 이방성 필터링
    Desc.MinLOD = 0;
    Desc.MaxLOD = 0;
    DEVICE->CreateSamplerState(&Desc, m_Sampler[0].GetAddressOf());

    Desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    Desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    Desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    Desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // 지정된 픽셀의 색상을 블러없이 가져옴
    Desc.MinLOD = 0;
    Desc.MaxLOD = 0;
    DEVICE->CreateSamplerState(&Desc, m_Sampler[1].GetAddressOf());
}



void Device::BindingSampler()
{
    CONTEXT->VSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->HSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->DSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->GSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->PSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->CSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());

    CONTEXT->VSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->HSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->DSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->GSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->PSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->CSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
}
