#include "pch.h"
#include "AComputeShader.h"

#include "Device.h"
#include "PathMgr.h"

AComputeShader::AComputeShader()
	: Asset(ASSET_TYPE::COMPUTESHADER)
	, m_Const{}
{
}

AComputeShader::~AComputeShader()
{
}

int AComputeShader::CreateComputeShader(const wstring& _RelativePath, const string& _FuncName)
{
	wstring HLSLPath = PathMgr::GetInst()->GetContentPath();
	HLSLPath += _RelativePath;

	int Flag = 0;
#ifdef _DEBUG
	Flag = D3DCOMPILE_DEBUG;
#endif

	// 컴퓨트 쉐이더 컴파일
	if (FAILED(D3DCompileFromFile(HLSLPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _FuncName.c_str(), "cs_5_0", Flag
		, 0, m_CSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf())))
	{
		if (nullptr == m_ErrBlob)
			MessageBox(nullptr, L"쉐이더 파일을 찾을 수 없음", L"쉐이더 컴파일 실패", MB_OK);
		else
			MessageBoxA(nullptr, (const char*)m_ErrBlob->GetBufferPointer(), "쉐이더 컴파일 실패", MB_OK);
		return E_FAIL;
	}

	// 컴퓨트 쉐이더 객체 생성
	if (FAILED(DEVICE->CreateComputeShader(m_CSBlob->GetBufferPointer(), m_CSBlob->GetBufferSize(), nullptr, m_CS.GetAddressOf())))
	{
		MessageBox(nullptr, L"쉐이더 객체 생성 실패", L"쉐이더 생성 실패", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

int AComputeShader::Execute()
{
	// 리소스 바인딩
	if (FAILED(Binding()))
		return E_FAIL;

	// 그룹 개수 계산
	CalcGroupCount();

	// 상수데이터 CS 에 바인딩
	static Ptr<ConstBuffer> pCB = Device::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pCB->SetData(&m_Const);
	pCB->Binding_CS();
	 
	// 컴퓨트 쉐이더 실행
	CONTEXT->CSSetShader(m_CS.Get(), nullptr, 0);
	CONTEXT->Dispatch(m_GroupX, m_GroupY, m_GroupZ);

	// 리소스 클리어
	Clear();
}