#ifndef _POSTPROCESS
#define _POSTPROCESS

#include "value.fx"

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position; 
    float2 vUV : TEXCOORD;
};

VS_OUT VS_Monochrome(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    //output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;    
    
    return output;
}


float4 PS_Monochrome(VS_OUT _in) : SV_Target
{    
    float4 vOutColor = (float4) 0.f;
    
    
    // 픽셀 좌표
    // _in.vPosition.xy    
    // ScreenUV 화면 전체기준 UV
    float2 vScreenUV = _in.vPosition.xy / RenderResolution;
    
    if(g_btex_0)
    {        
        _in.vUV += EngineTime * 0.1f;
        float4 vNoise = g_tex_0.Sample(g_sam_0, _in.vUV);
        
        // 0 ~ 1
        // -0.5 ~ 0.5f
        vNoise.rg -= 0.5f;
        
        // 노이즈 += 범위를 3%로 설정
        vNoise.rg *= 0.03f;        
        vScreenUV += vNoise.rg;        
    }
       
    
    vOutColor = g_PostProcess.Sample(g_sam_0, vScreenUV);
    vOutColor.rgb = (vOutColor.r + vOutColor.g + vOutColor.b) / 3.f;
    
    return vOutColor;
}

#endif