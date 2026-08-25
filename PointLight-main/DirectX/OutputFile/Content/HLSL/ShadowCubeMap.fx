#ifndef _SHADOWCUBEMAP
#define _SHADOWCUBEMAP

#include "value.fx"

struct VS_IN
{
    float3 vLocalPos : POSITION;
};

struct VS_OUT
{
    float4 ViewProjPos : SV_POSITION;
    float3 WorldPos : POSITION;
};

VS_OUT VS_ShadowCubeMap(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    float4 vWorld = mul(float4(_in.vLocalPos, 1.0f), g_matWorld);
    
    output.ViewProjPos = mul(vWorld, g_Mat_ViewProj);
    output.WorldPos = vWorld.xyz;
    
    return output;
}

#define LIGHTPOS g_vec4_0;
#define RADIUS   g_float_0;
float PS_ShadowCubeMap(VS_OUT input) : SV_Target
{
    float3 lightworldpos = LIGHTPOS;
    float dist = length(input.WorldPos - lightworldpos);
    return dist / RADIUS;
}
#endif