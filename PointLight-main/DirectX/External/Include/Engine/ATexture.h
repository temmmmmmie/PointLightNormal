#pragma once
#include "Asset.h"

class ATexture :
    public Asset
{
private:
    // DiretxTex 라이브러리에서 제공하는 클래스
    ScratchImage                        m_Image; // 저장장치(폴더) 에 있는 데이터를 SystemMem 으로 로딩
    ComPtr<ID3D11Texture2D>             m_Tex2D; // GPU 메모리에 Texture 이미지 데이터를 저장 및 관리
    D3D11_TEXTURE2D_DESC                m_Desc;

    ComPtr<ID3D11RenderTargetView>      m_RTV;
    ComPtr<ID3D11DepthStencilView>      m_DSV;
    ComPtr<ID3D11ShaderResourceView>    m_SRV;    // Texture2D 를 t 레지스터에 바인딩하는 역할
    ComPtr<ID3D11UnorderedAccessView>   m_UAV;    // Texture2D 를 u 레지스터(ComputeShader 전용)에 바인딩하는 역할

    int                                 m_RecentSRVNum;
    int                                 m_RecentUAVNum;


public:
    ComPtr<ID3D11Texture2D> GetTex2D() { return m_Tex2D; }
    UINT GetWidth() { return m_Desc.Width; }
    UINT GetHeight() { return m_Desc.Height; }
    Vec2 GetResolution() { return Vec2(m_Desc.Width, m_Desc.Height); }
    DXGI_FORMAT GetFormat() { return m_Desc.Format; }

    ComPtr<ID3D11RenderTargetView>    GetRTV() {return m_RTV;}
    ComPtr<ID3D11DepthStencilView>    GetDSV() {return m_DSV;}
    ComPtr<ID3D11ShaderResourceView>  GetSRV() {return m_SRV;}
    ComPtr<ID3D11UnorderedAccessView> GetUAV() { return m_UAV;}

    Pixel* GetPixels() { return (Pixel*)m_Image.GetPixels(); }
    bool IsCubeMap() { return m_Desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE; }


    // RenderingPipeline 에 t 바인딩
    void Binding(UINT _RegisterNum);

    // ComputeShader 시점에 t 바인딩
    void Binding_CS_SRV(UINT _RegisterNum);

    // ComputeShader 시점에 u 바인딩
    void Binding_CS_UAV(UINT _RegisterNum);

    // ComputeShader 시점에 t 에 바인딩한 것을 클리어
    void Clear_CS_SRV();

    // ComputeShader 시점에 u 에 바인딩한 것을 클리어
    void Clear_CS_UAV();


    static void Clear(UINT _RegisterNum);

    int Create(ComPtr<ID3D11Texture2D> _Tex2D);
    int Create(UINT _Width, UINT _Height, DXGI_FORMAT _Format, UINT _BindFlag, D3D11_USAGE _Usage);
    int CreateCubeMap(UINT _Width, UINT _Height, DXGI_FORMAT _Format, UINT _BindFlag, D3D11_USAGE _Usage);


public:
    virtual int Save(const wstring& _FilePath) override;

private:
    virtual int Load(const wstring& _FilePath) override;

    
    CLONE_DISABLE(ATexture);
public:
    ATexture();
    virtual ~ATexture();

    friend class CLight3D;
};

