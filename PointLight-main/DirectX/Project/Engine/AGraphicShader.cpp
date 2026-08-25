#include "pch.h"
#include "AGraphicShader.h"

#include "Device.h"
#include "PathMgr.h"


AGraphicShader::AGraphicShader(bool _IsEngineAsset)
	: Asset(ASSET_TYPE::GRAPHICSHADER, _IsEngineAsset)
	, m_Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, m_RSType(RS_TYPE::CULL_BACK)
	, m_DSType(DS_TYPE::LESS)
	, m_BSType(BS_TYPE::DEFAULT)
	, m_StencilRef(0)
{
}

AGraphicShader::~AGraphicShader()
{
}

int AGraphicShader::CreateVertexShader(const wstring& _RelativePath, const string& _FuncName)
{
	// 버텍스 쉐이더
	wstring HLSLPath = PathMgr::GetInst()->GetContentPath();
	HLSLPath += _RelativePath;

	int Flag = 0;
#ifdef _DEBUG
	Flag = D3DCOMPILE_DEBUG;
#endif

	// 버텍스 쉐이더 컴파일
	if (FAILED(D3DCompileFromFile(HLSLPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _FuncName.c_str(), "vs_5_0", Flag
		, 0, m_VSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf())))
	{
		if (nullptr == m_ErrBlob)
			MessageBox(nullptr, L"쉐이더 파일을 찾을 수 없음", L"쉐이더 컴파일 실패", MB_OK);
		else
			MessageBoxA(nullptr, (const char*)m_ErrBlob->GetBufferPointer(), "쉐이더 컴파일 실패", MB_OK);

		return E_FAIL;
	}

	// 버텍스 쉐이더 객체 생성
	DEVICE->CreateVertexShader(m_VSBlob->GetBufferPointer(), m_VSBlob->GetBufferSize(), nullptr, m_VS.GetAddressOf());

	// InputLayout 만들기
	CreateLayout();

	return S_OK;
}

int AGraphicShader::CreateHullShader(const wstring& _RelativePath, const string& _FuncName)
{
	wstring HLSLPath = PathMgr::GetInst()->GetContentPath();
	HLSLPath += _RelativePath;

	int Flag = 0;
#ifdef _DEBUG
	Flag = D3DCOMPILE_DEBUG;
#endif

	// 픽셀 쉐이더 컴파일
	if (FAILED(D3DCompileFromFile(HLSLPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _FuncName.c_str(), "hs_5_0", Flag
		, 0, m_HSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf())))
	{
		if (nullptr == m_ErrBlob)
			MessageBox(nullptr, L"쉐이더 파일을 찾을 수 없음", L"쉐이더 컴파일 실패", MB_OK);
		else
			MessageBoxA(nullptr, (const char*)m_ErrBlob->GetBufferPointer(), "쉐이더 컴파일 실패", MB_OK);
		return E_FAIL;
	}

	// 쉐이더 객체 생성
	DEVICE->CreateHullShader(m_HSBlob->GetBufferPointer(), m_HSBlob->GetBufferSize(), nullptr, m_HS.GetAddressOf());

	return S_OK;
}

int AGraphicShader::CreateDomainShader(const wstring& _RelativePath, const string& _FuncName)
{
	wstring HLSLPath = PathMgr::GetInst()->GetContentPath();
	HLSLPath += _RelativePath;

	int Flag = 0;
#ifdef _DEBUG
	Flag = D3DCOMPILE_DEBUG;
#endif

	// 픽셀 쉐이더 컴파일
	if (FAILED(D3DCompileFromFile(HLSLPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _FuncName.c_str(), "ds_5_0", Flag
		, 0, m_DSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf())))
	{
		if (nullptr == m_ErrBlob)
			MessageBox(nullptr, L"쉐이더 파일을 찾을 수 없음", L"쉐이더 컴파일 실패", MB_OK);
		else
			MessageBoxA(nullptr, (const char*)m_ErrBlob->GetBufferPointer(), "쉐이더 컴파일 실패", MB_OK);
		return E_FAIL;
	}

	// 쉐이더 객체 생성
	DEVICE->CreateDomainShader(m_DSBlob->GetBufferPointer(), m_DSBlob->GetBufferSize(), nullptr, m_DS.GetAddressOf());

	return S_OK;
}

int AGraphicShader::CreateGeometryShader(const wstring& _RelativePath, const string& _FuncName)
{
	wstring HLSLPath = PathMgr::GetInst()->GetContentPath();
	HLSLPath += _RelativePath;

	int Flag = 0;
#ifdef _DEBUG
	Flag = D3DCOMPILE_DEBUG;
#endif

	// 픽셀 쉐이더 컴파일
	if (FAILED(D3DCompileFromFile(HLSLPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _FuncName.c_str(), "gs_5_0", Flag
		, 0, m_GSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf())))
	{
		if (nullptr == m_ErrBlob)
			MessageBox(nullptr, L"쉐이더 파일을 찾을 수 없음", L"쉐이더 컴파일 실패", MB_OK);
		else
			MessageBoxA(nullptr, (const char*)m_ErrBlob->GetBufferPointer(), "쉐이더 컴파일 실패", MB_OK);
		return E_FAIL;
	}

	// 쉐이더 객체 생성
	DEVICE->CreateGeometryShader(m_GSBlob->GetBufferPointer(), m_GSBlob->GetBufferSize(), nullptr, m_GS.GetAddressOf());

	return S_OK;
}

int AGraphicShader::CreatePixelShader(const wstring& _RelativePath, const string& _FuncName)
{
	wstring HLSLPath = PathMgr::GetInst()->GetContentPath();
	HLSLPath += _RelativePath;

	int Flag = 0;
#ifdef _DEBUG
	Flag = D3DCOMPILE_DEBUG;
#endif

	// 픽셀 쉐이더 컴파일
	if (FAILED(D3DCompileFromFile(HLSLPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
								 , _FuncName.c_str(), "ps_5_0", Flag
								 , 0, m_PSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf())))
	{
		if (nullptr == m_ErrBlob)
			MessageBox(nullptr, L"쉐이더 파일을 찾을 수 없음", L"쉐이더 컴파일 실패", MB_OK);
		else
			MessageBoxA(nullptr, (const char*)m_ErrBlob->GetBufferPointer(), "쉐이더 컴파일 실패", MB_OK);
		return E_FAIL;
	}

	// 픽셀 쉐이더 객체 생성
	DEVICE->CreatePixelShader(m_PSBlob->GetBufferPointer(), m_PSBlob->GetBufferSize(), nullptr, m_PS.GetAddressOf());


	return S_OK;
}

void AGraphicShader::Binding()
{
	// TriangleList : 정점 3개로 구성된 삼각형 면(Face, Plane)
	// LineStrip : 정점을 잇는 라인에 걸리는 픽셀
	CONTEXT->IASetPrimitiveTopology(m_Topology);

	// Layout
	// 하나의 정점이 어떻게 구성되어있는지에 대한 정보
	CONTEXT->IASetInputLayout(m_Layout.Get());

	// VertexShader Stage
	CONTEXT->VSSetShader(m_VS.Get(), 0, 0);

	// HullShader Stage
	CONTEXT->HSSetShader(m_HS.Get(), 0, 0);

	// DomainShader Stage
	CONTEXT->DSSetShader(m_DS.Get(), 0, 0);
	
	// GeometryShader Stage
	CONTEXT->GSSetShader(m_GS.Get(), 0, 0);

	// RaterizerState
	CONTEXT->RSSetState(Device::GetInst()->GetRSState(m_RSType).Get());

	// PixelShader Stage
	CONTEXT->PSSetShader(m_PS.Get(), 0, 0);

	// DepthStencilState
	CONTEXT->OMSetDepthStencilState(Device::GetInst()->GetDSState(m_DSType).Get(), m_StencilRef);

	// BlendState
	CONTEXT->OMSetBlendState(Device::GetInst()->GetBSState(m_BSType).Get(), nullptr, 0xffffffff);
}


void AGraphicShader::CreateLayout()
{
	// InputLayout 생성
	// 정점 구조 정보 생성
	D3D11_INPUT_ELEMENT_DESC arrElement[21] = {};

	// POSITION
	arrElement[0].SemanticName = "POSITION";
	arrElement[0].SemanticIndex = 0;
	arrElement[0].AlignedByteOffset = 0;
	arrElement[0].Format = DXGI_FORMAT_R32G32B32_FLOAT; // 12
	arrElement[0].InputSlot = 0;
	arrElement[0].InstanceDataStepRate = 0;
	arrElement[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	arrElement[1].SemanticName = "COLOR";
	arrElement[1].SemanticIndex = 0;
	arrElement[1].AlignedByteOffset = 12;
	arrElement[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 16
	arrElement[1].InputSlot = 0;
	arrElement[1].InstanceDataStepRate = 0;
	arrElement[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	arrElement[2].SemanticName = "TEXCOORD";
	arrElement[2].SemanticIndex = 0;
	arrElement[2].AlignedByteOffset = 28;
	arrElement[2].Format = DXGI_FORMAT_R32G32_FLOAT; // 8
	arrElement[2].InputSlot = 0;
	arrElement[2].InstanceDataStepRate = 0;
	arrElement[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	arrElement[3].SemanticName = "TANGENT";
	arrElement[3].SemanticIndex = 0;
	arrElement[3].AlignedByteOffset = 36;
	arrElement[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	arrElement[3].InputSlot = 0;
	arrElement[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	arrElement[3].InstanceDataStepRate = 0;

	arrElement[4].SemanticName = "NORMAL";
	arrElement[4].SemanticIndex = 0;
	arrElement[4].AlignedByteOffset = 48;
	arrElement[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	arrElement[4].InputSlot = 0;
	arrElement[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	arrElement[4].InstanceDataStepRate = 0;

	arrElement[5].SemanticName = "BINORMAL";
	arrElement[5].SemanticIndex = 0;
	arrElement[5].AlignedByteOffset = 60;
	arrElement[5].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	arrElement[5].InputSlot = 0;
	arrElement[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	arrElement[5].InstanceDataStepRate = 0;

	arrElement[6].SemanticName = "BLENDWEIGHT";
	arrElement[6].SemanticIndex = 0;
	arrElement[6].AlignedByteOffset = 72;
	arrElement[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	arrElement[6].InputSlot = 0;
	arrElement[6].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	arrElement[6].InstanceDataStepRate = 0;

	arrElement[7].SemanticName = "BLENDINDICES";
	arrElement[7].SemanticIndex = 0;
	arrElement[7].AlignedByteOffset = 88;
	arrElement[7].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	arrElement[7].InputSlot = 0;
	arrElement[7].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	arrElement[7].InstanceDataStepRate = 0;
		
	if (FAILED(DEVICE->CreateInputLayout(arrElement, 8
										, m_VSBlob->GetBufferPointer()
										, m_VSBlob->GetBufferSize()
										, m_Layout.GetAddressOf())))
	{
		MessageBox(nullptr, L"레이아웃 생성 실패", L"레이아웃 생성 실패", MB_OK);
		return;
	}
}