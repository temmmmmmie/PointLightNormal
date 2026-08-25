#ifndef _MERGE
#define _MREGE

#include "value.fx"
#include "func.fx"


// ====================
// SelectedTargetShader
// Mesh : RectMesh
#define COLOR_TARGET    g_tex_0
#define DIFFUSE_TARGET  g_tex_1
#define SPECULAR_TARGET g_tex_2
#define EMISSIVE_TARGET g_tex_3
// ====================
struct VS_IN
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_Merge(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = float4(_in.vLocalPos.xy * 2.f, 0.f, 1.f);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Merge(VS_OUT _in) : SV_Target
{
    float3 vColor       = COLOR_TARGET.Sample(g_sam_0, _in.vUV).xyz;
    float3 vDiffuse     = DIFFUSE_TARGET.Sample(g_sam_0, _in.vUV).xyz;
    float3 vSpecular    = SPECULAR_TARGET.Sample(g_sam_0, _in.vUV).xyz;
    float3 vEmissive    = EMISSIVE_TARGET.Sample(g_sam_0, _in.vUV).xyz;
    
    float3 vOutColor = (vColor * vDiffuse) + vSpecular + vEmissive;
    
    return float4(vOutColor, 1.f);
}

#endif