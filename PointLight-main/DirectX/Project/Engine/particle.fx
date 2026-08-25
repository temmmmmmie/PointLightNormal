#ifndef _PARTICLE
#define _PARTICLE

#include "value.fx"
#include "struct.fx"

StructuredBuffer<tParticle> g_Buffer : register(t17);

#define OBJECT_POS g_vec4_0

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;     
    uint InstID : SV_InstanceID;
};

struct VS_OUT
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;
    uint InstID : FOG;
};

VS_OUT VS_Particle(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
            
    output.vLocalPos = _in.vPos;
    output.vUV = _in.vUV;
    output.InstID = _in.InstID;
    
    return output;
}

// GeometryShader
// 정점을 파이프라인 도중에 생성시킬 수 있는 쉐이더
// 출력 스트림에 정점을 하나도 넣지 않으면, 파이프라인이 중단된다(가장 빠른 시점에 파이프라인 중단 방법)
// 1. 파이프라인 제어(중도 취소 가능)
// 2. 빌보드 효과
struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    uint InstID : FOG;
};

// GS 로 생성시킬 수 있는 최대 정점 수
[maxvertexcount(6)]
void GS_Particle(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _OutStream)
{
    // 비활성화된 파티클이면 다음회차로 넘어감
    if (false == g_Buffer[_in[0].InstID].Active)
        return;
    
    tParticle particle = g_Buffer[_in[0].InstID];
            
    float3 vWorldPos = particle.WorldPos + OBJECT_POS.xyz;
    float3 vViewPos = mul(float4(vWorldPos, 1.f), g_matView);
    
    
    GS_OUT OutVtx[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
        
    OutVtx[0].vPosition = mul(float4(vViewPos.x - particle.WorldScale.x / 2.f, vViewPos.y + particle.WorldScale.y / 2.f, vViewPos.z, 1.f), g_matProj);
    OutVtx[1].vPosition = mul(float4(vViewPos.x + particle.WorldScale.x / 2.f, vViewPos.y + particle.WorldScale.y / 2.f, vViewPos.z, 1.f), g_matProj);
    OutVtx[2].vPosition = mul(float4(vViewPos.x + particle.WorldScale.x / 2.f, vViewPos.y - particle.WorldScale.y / 2.f, vViewPos.z, 1.f), g_matProj);
    OutVtx[3].vPosition = mul(float4(vViewPos.x - particle.WorldScale.x / 2.f, vViewPos.y - particle.WorldScale.y / 2.f, vViewPos.z, 1.f), g_matProj);
          
    OutVtx[0].vUV = float2(0.f, 0.f);
    OutVtx[1].vUV = float2(1.f, 0.f);
    OutVtx[2].vUV = float2(1.f, 1.f);
    OutVtx[3].vUV = float2(0.f, 1.f);
        
    for (int i = 0; i < 4; ++i)
    {
        OutVtx[i].InstID = _in[0].InstID;
    }    
    
    _OutStream.Append(OutVtx[0]);
    _OutStream.Append(OutVtx[1]);
    _OutStream.Append(OutVtx[2]);
    _OutStream.RestartStrip();    
    
    _OutStream.Append(OutVtx[0]);
    _OutStream.Append(OutVtx[2]);
    _OutStream.Append(OutVtx[3]);
    _OutStream.RestartStrip();
}


float4 PS_Particle(GS_OUT _in) : SV_Target
{    
    float4 vColor = (float4) g_Buffer[_in.InstID].Color;
    
    if (g_btex_0)
    {
        vColor *= g_tex_0.Sample(g_sam_0, _in.vUV);
    }
    
    return vColor;
}

#endif