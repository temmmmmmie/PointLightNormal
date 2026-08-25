#ifndef _LIGHT
#define _LIGHT

#include "value.fx"
#include "func.fx"


// ==============
// DirLightShader
// MRT : LightMRT
//       - DiffuseTargetTex
//       - SpecularTargetTex
// Mesh : RectMesh
#define POSIIONT_TARGET g_tex_0
#define NORMAL_TARGET   g_tex_1
#define SHADOW_MAP      g_tex_2
// ===============
struct VS_DIR_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_DIR_OUT
{
    float4 vPosition : SV_Position;
    float3 vLocalDir : POSITION;
    float2 vUV : TEXCOORD;
};

VS_DIR_OUT VS_DirLight(VS_DIR_IN _in)
{
    VS_DIR_OUT output = (VS_DIR_OUT) 0.f;

    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    output.vUV = _in.vUV;
        
    return output;
}

struct PS_DIR_OUT
{
    float4 vDiffuse : SV_Target;
    float4 vSpecular : SV_Target1;
};

PS_DIR_OUT PS_DirLight(VS_DIR_OUT _in)
{
    PS_DIR_OUT output = (PS_DIR_OUT) 0.f;
        
    float4 ViewPos = POSIIONT_TARGET.Sample(g_sam_0, _in.vUV);
    
    if (ViewPos.x == 0.f && ViewPos.y == 0.f && ViewPos.z == 0.f)
        discard;    
    
    float3 vWorldPos = mul(float4(ViewPos.xyz, 1.f), g_matViewInv).xyz;
    
    
    
    // 그림자 테스트  
    // 기록된 물체의 월드 위치를 광원쪽으로 투영시켜야한다.    
    float4 vViewPos = mul(float4(vWorldPos, 1.f), g_mat_0);
    float4 vProjPos = mul(vViewPos, g_mat_1);
    vProjPos.xyz /= vProjPos.w;    
    
    float ShadowRatio = 1.f;    
    float2 vShadowMapUV = float2(vProjPos.x * 0.5f + 0.5f
                               , 1.f - (vProjPos.y * 0.5f + 0.5f));   
    
    // UV 범위를 벗어났다 ==> 광원 시점에서 물체를 그리지 못했다(광원 시야 범위 밖이라서)
    // 그림자 판정 보류
    if (0.f <= vShadowMapUV.x && vShadowMapUV.x <= 1.f 
        && 0.f <= vShadowMapUV.y && vShadowMapUV.y <= 1.f)
    {
        float ViewZ = SHADOW_MAP.Sample(g_sam_0, vShadowMapUV).x;
        if (ViewZ + 0.5f < vViewPos.z)
        {
            ShadowRatio = 0.1f;
        }
    }
    
    float4 ViewNormal = NORMAL_TARGET.Sample(g_sam_0, _in.vUV);    
    
    float3 vDiffuse = (float3) 0.f;
    float3 vAmbient = (float3) 0.f;
    float3 vSpecular = (float3) 0.f;
    CalcLight3D(ViewPos.xyz, ViewNormal.xyz, g_int_0, vDiffuse, vAmbient, vSpecular);
            
    output.vDiffuse = float4(vDiffuse * ShadowRatio + vAmbient, 1.f);
    output.vSpecular = float4(vSpecular * ShadowRatio, 1.f);
    
    return output;
}



// ===========================
// PointLightShader
// MRT : LightMRT
//       - DiffuseTargetTex
//       - SpecularTargetTex
// Mesh : SphereMesh
#define POSIIONT_TARGET g_tex_0
#define NORMAL_TARGET   g_tex_1
#define SHADOW_CUBEMAP  g_texcube_0
#define LIGHTPOS        g_vec4_0;
#define RADIUS          g_float_0;
// ============================
VS_DIR_OUT VS_PointLight(VS_DIR_IN _in)
{
    VS_DIR_OUT output = (VS_DIR_OUT) 0.f;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
        
    return output;
}

PS_DIR_OUT PS_PointLight(VS_DIR_OUT _in)
{
    PS_DIR_OUT output = (PS_DIR_OUT) 0.f;
    
    // 픽셀 좌표
    // _in.vPosition.xy / 해상도
    // ScreenUV 화면 전체기준 UV
    float2 vScreenUV = _in.vPosition.xy / RenderResolution;
    
    float4 ViewPos = POSIIONT_TARGET.Sample(g_sam_0, vScreenUV);
    if (ViewPos.x == 0.f && ViewPos.y == 0.f && ViewPos.z == 0.f)
        discard;
    
    float3 vWorldPos = mul(float4(ViewPos.xyz, 1.f), g_matViewInv);
    float3 vLocalPos = mul(float4(vWorldPos.xyz, 1.f), g_matWorldInv);
    
    if (length(vLocalPos) >= 0.5f)
    {
        discard;
    }
    
    
    float ShadowRatio = 1.f;
    
    float3 lightworldpos = LIGHTPOS;
    float3 L = vWorldPos - lightworldpos;
    float currentDist = length(L);

    float shadowDepth = SHADOW_CUBEMAP.Sample(g_sam_0, L).r;
    shadowDepth *= RADIUS;

    if (currentDist > shadowDepth + 0.5f)
        ShadowRatio = 0.1f;

    
    float4 ViewNormal = NORMAL_TARGET.Sample(g_sam_0, vScreenUV);
    
    float3 vDiffuse = (float3) 0.f;
    float3 vAmbient = (float3) 0.f;
    float3 vSpecular = (float3) 0.f;
    CalcLight3D(ViewPos.xyz, ViewNormal.xyz, g_int_0, vDiffuse, vAmbient, vSpecular);
            
    output.vDiffuse = float4(vDiffuse * ShadowRatio + vAmbient, 1.f);
    output.vSpecular = float4(vSpecular * ShadowRatio, 1.f);
    
    return output;
}



#endif