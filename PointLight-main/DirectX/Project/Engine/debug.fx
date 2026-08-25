#ifndef _DEBUG
#define _DEBUG

#include "value.fx"

struct VS_IN
{
    float2 vUV : TEXCOORD; // Sementic
    float3 vPos : POSITION; // Sementic
    float3 vNormal : NORMAL;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    float3 vViewNormal : NORMAL;
    float3 vViewPos : POSITION;
};

VS_OUT VS_Debug(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
            
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    output.vViewPos = normalize(mul(float4(_in.vPos, 1.f), g_matWV));
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV));
    
    return output;
}

float4 PS_Debug(VS_OUT _in) : SV_Target
{       
    float4 vOutColor = g_vec4_0;
        
    // Sphere 타입인 경우
    if(4 == g_int_0)
    {
        float3 vEye = normalize(_in.vViewPos);
        float Dot = dot(vEye, _in.vViewNormal);              
        float Alpha = saturate(1.f - Dot);                
        Alpha = pow(Alpha, 2.5f);        
        vOutColor.a = Alpha;
    }
    
    return vOutColor;
}


// ====================
// SelectedTargetShader
// Mesh : RectMesh
// ====================
struct VS_SELECTED_IN
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_SELECTED_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

VS_SELECTED_OUT VS_Selected(VS_SELECTED_IN _in)
{
    VS_SELECTED_OUT output = (VS_SELECTED_OUT) 0.f;
    
    output.vPosition = float4(_in.vLocalPos.xy * 2.f, 0.f, 1.f);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Selected(VS_SELECTED_OUT _in) : SV_Target
{
    return g_tex_0.Sample(g_sam_0, _in.vUV);
}

#endif