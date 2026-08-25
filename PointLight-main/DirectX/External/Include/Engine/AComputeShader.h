#pragma once
#include "Asset.h"

class AComputeShader :
    public Asset
{
private:
    ComPtr<ID3DBlob>			m_CSBlob;   // 컴퓨트쉐이더 컴파일 바이너리 데이터
    ComPtr<ID3DBlob>			m_ErrBlob;  // 실패한 경우, 에러메세지 저장
    ComPtr<ID3D11ComputeShader>	m_CS;       // 컴파일한 결과물로 생성시킨 ComputeShader 객체

    // 그룹 당 스레드 수
protected:
    tMtrlConst                  m_Const;

    UINT                        m_ThreadPerGroupX;
    UINT                        m_ThreadPerGroupY;
    UINT                        m_ThreadPerGroupZ;

    // 그룹 수
    UINT                        m_GroupX;
    UINT                        m_GroupY;
    UINT                        m_GroupZ;






protected:
    int CreateComputeShader(const wstring& _RelativePath, const string& _FuncName);

protected:
    virtual int Binding() = 0;
    virtual void CalcGroupCount() = 0;
    virtual void Clear() = 0;

public:
    int Execute();

public:
    virtual int Save(const wstring& _FilePath) override { return S_OK; }

private:
    virtual int Load(const wstring& _FilePath) override { return S_OK; };

    CLONE_DISABLE(AComputeShader);
public:
    AComputeShader();
    virtual ~AComputeShader();
};

