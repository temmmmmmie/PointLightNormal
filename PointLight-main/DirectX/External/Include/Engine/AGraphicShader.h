#pragma once
#include "Asset.h"

enum class SHADER_PARAM
{
    INT,
    FLOAT,
    VEC2,
    VEC4,
    MATRIX,
    TEXTURE,
};

struct tShaderParam
{
    wstring         Desc;
    SHADER_PARAM    Param;
    int             Index;
    bool            IsInput; // UI 표시방법 (단순 입력, 드래그)
    float           Step;    // UI 에서 값의 이동 단위
};


class AGraphicShader :
    public Asset
{
private:
    ComPtr<ID3DBlob>			    m_VSBlob;
    ComPtr<ID3DBlob>			    m_HSBlob;
    ComPtr<ID3DBlob>			    m_DSBlob;
    ComPtr<ID3DBlob>			    m_GSBlob;
    ComPtr<ID3DBlob>		        m_PSBlob;
    ComPtr<ID3DBlob>		        m_ErrBlob;

    ComPtr<ID3D11VertexShader>	    m_VS;
    ComPtr<ID3D11HullShader>	    m_HS;
    ComPtr<ID3D11DomainShader>	    m_DS;
    ComPtr<ID3D11GeometryShader>    m_GS;
    ComPtr<ID3D11PixelShader>	    m_PS;

    ComPtr<ID3D11InputLayout>	    m_Layout;
    D3D11_PRIMITIVE_TOPOLOGY        m_Topology;

    RS_TYPE                         m_RSType;
    DS_TYPE                         m_DSType;
    BS_TYPE                         m_BSType;

    UINT                            m_StencilRef;

    vector<tShaderParam>            m_vecShaderParam;

public:
    int CreateVertexShader(const wstring& _RelativePath, const string& _FuncName);
    int CreateHullShader(const wstring& _RelativePath, const string& _FuncName);
    int CreateDomainShader(const wstring& _RelativePath, const string& _FuncName);
    int CreateGeometryShader(const wstring& _RelativePath, const string& _FuncName);
    int CreatePixelShader(const wstring& _RelativePath, const string& _FuncName);
    void SetRSType(RS_TYPE _Type) { m_RSType = _Type; }
    void SetDSType(DS_TYPE _Type, UINT _StencilRef = 0) { m_DSType = _Type; m_StencilRef = _StencilRef; }
    void SetBSType(BS_TYPE _Type) { m_BSType = _Type; }

    void Binding();

    void AddShaderParam(SHADER_PARAM _Type, int _Idx, wstring _Desc, bool _IsInput, float _Step) { m_vecShaderParam.push_back(tShaderParam{ _Desc , _Type , _Idx , _IsInput , _Step }); }
    const vector<tShaderParam>& GetShaderParam() { return m_vecShaderParam; }

    GET_SET(D3D11_PRIMITIVE_TOPOLOGY, Topology);

private:
    void CreateLayout();

public:
    virtual int Save(const wstring& _FilePath) override{return S_OK;};
    virtual int Load(const wstring& _FilePath) override{return S_OK;};


    CLONE_DISABLE(AGraphicShader);
public:
    AGraphicShader(bool _IsEngineAsset = false);
    virtual ~AGraphicShader();
};

