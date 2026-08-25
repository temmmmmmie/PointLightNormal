#ifndef _DECAL
#define _DECAL

#include "value.fx"


#define POSITION_TARGET g_tex_0
#define DATA_TARGET     g_tex_1
#define DECAL_TEX       g_tex_2
#define HasDecalTex     g_btex_2


struct VS_IN
{
    float3 vPos : POSITION;
    float3 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_Decal(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
       
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

struct PS_OUT
{
    float4 vColor       : SV_Target;
    float4 vEmissive    : SV_Target1;
};

PS_OUT PS_Decal(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
               
    float2 vScreenUV = _in.vPosition.xy / RenderResolution;
    float3 vViewPos = POSITION_TARGET.Sample(g_sam_0, vScreenUV).xyz;
    
    if (vViewPos.x == 0.f && vViewPos.y == 0.f && vViewPos.z == 0.f)
    {     
        discard;
    }
    
    float3 vWorldPos = mul(float4(vViewPos, 1.f), g_matViewInv).xyz;
    float3 vLocalPos = mul(float4(vWorldPos, 1.f), g_matWorldInv).xyz;
        
    if (vLocalPos.x < -0.5f || 0.5f < vLocalPos.x
        || vLocalPos.y < -0.5f || 0.5f < vLocalPos.y
        || vLocalPos.z < -0.5f || 0.5f < vLocalPos.z)
    {        
        discard;
    }
    
    if(0.f != g_float_0)
    {
        float ID = DATA_TARGET.Sample(g_sam_0, vScreenUV).x;        
        if (g_float_0 != ID)
            discard;
    }
    
        
    float4 vDecalColor = float4(1.f, 0.f, 1.f, 1.f);
            
    if (HasDecalTex)
    {
        float2 vUV = float2(vLocalPos.x + 0.5f, 1.f - (vLocalPos.z + 0.5f));        
        vDecalColor = DECAL_TEX.Sample(g_sam_0, vUV);        
    }
    
    // Decal 모드에 따라서, 어떤 렌더타겟에 문양을 출력할지 결정
    if(0 == g_int_0)
    {
        output.vColor = vDecalColor;
    }
    else
    {                
        vDecalColor.rgb *= vDecalColor.a;
        output.vEmissive = vDecalColor;
    }    
    
    return output;
}





#endif