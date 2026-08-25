#ifndef _SKYBOX
#define _SKYBOX

#include "value.fx"
#include "func.fx"


struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float3 vLocalDir : POSITION;
};

VS_OUT VS_SkyBox(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(mul(float4(_in.vPos, 0.f), g_matView), g_matProj);
    output.vPosition.z = output.vPosition.w;        
    output.vUV = _in.vUV;
        
    // 로컬 공간에서 정점의 좌표가 곧 원점에서 정점을 향하는 방향벡터
    output.vLocalDir = normalize(_in.vPos);
    
    
    // 좌표계 변환 과정
    // Local * W => WorldPos
    // WorldPos * V => ViewPos
    // ViewPos * P => NDC
    return output;
}

float4 PS_SkyBox(VS_OUT _in) : SV_Target
{
    float4 Albedo = float4(0.4f, 0.4f, 0.4f, 1.f);
    
    // Sphere 타입 SkyBox 라면
    if(g_int_0 == 0)
    {
        if (g_btex_0)
            Albedo = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
    
    // Cube 타입
    else
    {
        if (g_btexcube_0)
            Albedo = g_texcube_0.Sample(g_sam_0, _in.vLocalDir);
    }     
    
    return Albedo;
}



#endif