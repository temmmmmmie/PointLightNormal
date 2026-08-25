#include "pch.h"
#include "ATexture.h"

#include "Device.h"



ATexture::ATexture()
	: Asset(ASSET_TYPE::TEXTURE)
	, m_Desc{}
	, m_RecentSRVNum(-1)
	, m_RecentUAVNum(-1)
{
}

ATexture::~ATexture()
{
}

int ATexture::Load(const wstring& _FilePath)
{
	path Ext = path(_FilePath).extension();

	HRESULT hr = S_OK;

	// DirectxTex 라이브러리 함수
	// .dds
	if (Ext == L".dds" || Ext == L".DDS")
		hr = LoadFromDDSFile(_FilePath.c_str(), DDS_FLAGS_NONE, nullptr, m_Image);
	// .tga
	else if (Ext == L".tga" || Ext == L".TGA")
		hr = LoadFromTGAFile(_FilePath.c_str(), nullptr, m_Image);
	// WIC(Window Image Component) - bmp, jpg, jpeg, png
	else if (Ext == L".png" || Ext == L".PNG"
		|| Ext == L".jpg" || Ext == L".JPG"
		|| Ext == L".jpeg" || Ext == L".JPEG"
		|| Ext == L".bmp" || Ext == L".BMP")
		hr = LoadFromWICFile(_FilePath.c_str(), WIC_FLAGS_NONE, nullptr, m_Image);

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"시스템 메모리 로딩 실패", L"텍스쳐 로딩 실패", MB_OK);
		return E_FAIL;
	}

	CreateShaderResourceView(DEVICE, m_Image.GetImages(), m_Image.GetImageCount(), m_Image.GetMetadata(), m_SRV.GetAddressOf());

	m_SRV->GetResource((ID3D11Resource**)m_Tex2D.GetAddressOf());

	m_Tex2D->GetDesc(&m_Desc);

	return S_OK;
}

int ATexture::Create(ComPtr<ID3D11Texture2D> _Tex2D)
{
	m_Tex2D = _Tex2D;

	m_Tex2D->GetDesc(&m_Desc);

	if (m_Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf());
	}

	if (m_Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf());
	}

	if (m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		DEVICE->CreateShaderResourceView(m_Tex2D.Get(), nullptr, m_SRV.GetAddressOf());
	}

	return S_OK;
}

int ATexture::Create(UINT _Width, UINT _Height, DXGI_FORMAT _Format, UINT _BindFlag, D3D11_USAGE _Usage)
{
	m_Desc.Width = _Width;
	m_Desc.Height = _Height;
	m_Desc.Format = _Format;
	m_Desc.ArraySize = 1;
	m_Desc.BindFlags = _BindFlag;

	m_Desc.Usage = _Usage;
	if (m_Desc.Usage == D3D11_USAGE_DYNAMIC)
		m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	else
		m_Desc.CPUAccessFlags = 0;

	m_Desc.MiscFlags = 0;
	m_Desc.SampleDesc.Count = 1;
	m_Desc.SampleDesc.Quality = 0;
	m_Desc.MipLevels = 1;    // 원본 텍스쳐의 저화질 버전 이미지 생성, 1 == 원본만

	// Texture2D 생성
	if (FAILED(DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf())))
		return E_FAIL;

	// Texture 의 용도에 맞는 View 를 생성한다.
	if (m_Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
		DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf());
	if (m_Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
		DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf());
	if (m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		DEVICE->CreateShaderResourceView(m_Tex2D.Get(), nullptr, m_SRV.GetAddressOf());
	if (m_Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
		DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), nullptr, m_UAV.GetAddressOf());

	return S_OK;
}

int ATexture::CreateCubeMap(UINT _Width, UINT _Height, DXGI_FORMAT _Format, UINT _BindFlag, D3D11_USAGE _Usage)
{
	m_Desc.Width = _Width;
	m_Desc.Height = _Height;
	m_Desc.Format = _Format;
	m_Desc.ArraySize = 6;
	m_Desc.BindFlags = _BindFlag;

	m_Desc.Usage = _Usage;
	if (m_Desc.Usage == D3D11_USAGE_DYNAMIC)
		m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	else
		m_Desc.CPUAccessFlags = 0;

	m_Desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	m_Desc.SampleDesc.Count = 1;
	m_Desc.SampleDesc.Quality = 0;
	m_Desc.MipLevels = 1;    // 원본 텍스쳐의 저화질 버전 이미지 생성, 1 == 원본만

	// Texture2D 생성
	if (FAILED(DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf())))
		return E_FAIL;

	// Texture 의 용도에 맞는 View 를 생성한다.
	if (m_Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvDesc.Texture2DArray.MipSlice = 0;
		rtvDesc.Texture2DArray.FirstArraySlice = 0;
		rtvDesc.Texture2DArray.ArraySize = 6;
		DEVICE->CreateRenderTargetView(m_Tex2D.Get(), &rtvDesc, m_RTV.GetAddressOf());
	}
	if (m_Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL) {
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		dsvDesc.Texture2DArray.ArraySize = 6;
		DEVICE->CreateDepthStencilView(m_Tex2D.Get(), &dsvDesc, m_DSV.GetAddressOf());
	}
	
	if (m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE) {
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = 1;
		DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &srvDesc, m_SRV.GetAddressOf());
	}
	
	if (m_Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
		DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), nullptr, m_UAV.GetAddressOf());

	return S_OK;
}


void ATexture::Binding(UINT _RegisterNum)
{
	CONTEXT->VSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->HSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->DSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->GSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->PSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
}

void ATexture::Binding_CS_SRV(UINT _RegisterNum)
{
	m_RecentSRVNum = _RegisterNum;
	CONTEXT->CSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
}

void ATexture::Binding_CS_UAV(UINT _RegisterNum)
{
	m_RecentUAVNum = _RegisterNum;
	UINT i = -1;
	CONTEXT->CSSetUnorderedAccessViews(_RegisterNum, 1, m_UAV.GetAddressOf(), &i);
}

void ATexture::Clear_CS_SRV()
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	CONTEXT->CSSetShaderResources(m_RecentSRVNum, 1, &pSRV);
	m_RecentSRVNum = -1;
}

void ATexture::Clear_CS_UAV()
{
	ID3D11UnorderedAccessView* pUAV = nullptr;
	UINT i = -1;
	CONTEXT->CSSetUnorderedAccessViews(m_RecentUAVNum, 1, &pUAV, &i);
	m_RecentUAVNum = -1;
}

void ATexture::Clear(UINT _RegisterNum)
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	CONTEXT->VSSetShaderResources(_RegisterNum, 1, &pSRV);
	CONTEXT->HSSetShaderResources(_RegisterNum, 1, &pSRV);
	CONTEXT->DSSetShaderResources(_RegisterNum, 1, &pSRV);
	CONTEXT->GSSetShaderResources(_RegisterNum, 1, &pSRV);
	CONTEXT->PSSetShaderResources(_RegisterNum, 1, &pSRV);
}



int ATexture::Save(const wstring& _FilePath)
{
	// GPU -> System
	CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);

	// System -> File
	wstring strRelativePath = PathMgr::GetInst()->GetRelativePath(_FilePath);
	SetRelativePath(strRelativePath);

	HRESULT hr = E_FAIL;
	if (1 == m_Image.GetMetadata().arraySize)
	{
		// png, jpg, jpeg, bmp, 
		hr = SaveToWICFile(*m_Image.GetImages()
			, WIC_FLAGS_NONE
			, GetWICCodec(WICCodecs::WIC_CODEC_PNG)
			, _FilePath.c_str());
	}

	else
	{
		hr = SaveToDDSFile(m_Image.GetImages()
			, m_Image.GetMetadata().arraySize
			, m_Image.GetMetadata()
			, DDS_FLAGS_NONE
			, _FilePath.c_str());
	}

	return hr;
}
