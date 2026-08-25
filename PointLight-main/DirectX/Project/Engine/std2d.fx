#ifndef _STD2D
#define _STD2D

#include "value.fx"
#include "func.fx"

// HLSL 5.0 기준 문법
struct VS_IN
{
    float2 vUV : TEXCOORD;  // Sementic
    float4 vColor : COLOR; // Sementic
    float3 vPos : POSITION; // Sementic
};

struct VS_OUT
{
    // System Value sement, 특수 시멘틱
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    float4 vColor : COLOR;
    
    float3 vWorldPos : POSITION;
};


// 입력으로 들어온 모델정보를 처리하는 
// 위치를 원하는 위치로 배치시킨다(좌표계 변환)
// Object Space  --> World Space --> ViewSpace     ---> ProjectionSpace
// Model Space                       CameraSpace        NDC(-1 ~ 1)
// Local Space

VS_OUT VS_Std2D(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
            
    // 모델의 로컬정점 위치는 float3
    // 월드행렬은 4x4
    // 모델 정점의 4번째 동차좌표를 1로 확장 ( Why? 상태행렬의 4행에 이동량이 기록되도록 약속함)
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vColor = _in.vColor;
    output.vUV = _in.vUV ;
    
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
    
    return output;
}

float4 PS_Std2D(VS_OUT _in) : SV_Target
{
    float4 vColor = float4(1.f, 0.f, 1.f, 1.f);
    
    if (1 == g_int_0)
    {
        return float4(1.f, 0.f, 0.f, 1.f);
    }
    
    if (g_btex_0)
    {
        vColor = g_tex_0.Sample(g_sam_1, _in.vUV);
    }
    
    if (vColor.a <= 0.1f)
        discard; // 픽셀 쉐이더 중단
    
    float3 LightColor = (float3) 0.f;
    CalcLight2D(_in.vWorldPos, LightColor);
    vColor.rgb *= LightColor;
    
    return vColor;
}

float4 PS_Std2D_AlphaBlend(VS_OUT _in) : SV_Target
{
    float4 vColor = float4(1.f, 0.f, 1.f, 1.f);
    
    if (g_btex_0)
    {
        vColor = g_tex_0.Sample(g_sam_1, _in.vUV);
    }
        
    return vColor;
}

float4 PS_Monochrome(VS_OUT _in) : SV_Target
{
    float4 vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    
    float Aver = (vColor.r + vColor.g + vColor.b) / 3.f;
    
    vColor.rgb = Aver;
    
    return vColor;
}


#endif