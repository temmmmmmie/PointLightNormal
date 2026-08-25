#ifndef _LANDSCAPE
#define _LANDSCAPE

#include "value.fx"
#include "func.fx"


#define FACE_X      g_int_0
#define FACE_Z      g_int_1
#define HEIGHT_MAP  g_tex_0
#define IsHeightMap g_btex_0



struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vNormal : NORMAL;
};

struct VS_OUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vNormal : NORMAL;
};

VS_OUT VS_LandScape(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // 로컬 공간상의에서의 모델 좌표를 변형없이 그래도 HullShader 로 보낸다
    // 정점 분할을 로컬공간내에서 하겠다.
    output.vPos = _in.vPos;
    output.vUV = _in.vUV;
    output.vNormal = _in.vNormal;
    
    return output;
}


// ==========
// HullShader
// ==========
// Patch : 도형을 구성하는 최소한의 구조
// 패치상수함수 (PatchConstantFunc)
// 패치당 한번씩 수행되는 함수
struct PatchTess
{
    float Edge[3] : SV_TessFactor;
    float Inside : SV_InsideTessFactor;
};

PatchTess PatchConstFunc(InputPatch<VS_OUT, 3> _InputPatch, int _PatchIdx : SV_PrimitiveID)
{
    PatchTess output = (PatchTess) 0.f;
        
    float Factor = g_float_0;
    
    if (Factor == 0.f)
        Factor = 1.f;
        
    output.Edge[0] = Factor;
    output.Edge[1] = Factor;
    output.Edge[2] = Factor;
    output.Inside = Factor;
    
    return output;
}

// Main HullShader
// 정점마다 수행되는 함수
struct HS_OUT
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;    
    float3 vNormal : NORMAL;
};


[domain("tri")]
//[partitioning("integer")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[maxtessfactor(32.f)]
[patchconstantfunc("PatchConstFunc")]
HS_OUT HS_LandScape(InputPatch<VS_OUT, 3> _InputPatch
            , int _VtxIdx : SV_OutputControlPointID
            , int _PatchIdx : SV_PrimitiveID)
{
    HS_OUT output = (HS_OUT) 0.f;
    
    output.vLocalPos    = _InputPatch[_VtxIdx].vPos;
    output.vUV          = _InputPatch[_VtxIdx].vUV;    
    output.vNormal      = _InputPatch[_VtxIdx].vNormal;
    
    return output;
}

// =============
// Domain Shader
// =============
// HullShader 에서 요청한 분할 개수에 맞게 도형이 쪼개짐(Tesselator)
// 생성된 모든 도형(정점)들 에 대해서 다시 DomainShader 가 호출된다.
struct DS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;
};

[domain("tri")]
DS_OUT DS_LandScape(PatchTess _TessFactor, OutputPatch<HS_OUT, 3> _Patch
                  , float3 _Weight : SV_DomainLocation)
{
    DS_OUT output = (DS_OUT) 0.f;
       
    float3 vLocalPos = (float3) 0.f;
    float2 vUV = (float2) 0.f;
    float3 vNormal = (float3) 0.f;
    
    
    for (int i = 0; i < 3; ++i)
    {
        vLocalPos += _Patch[i].vLocalPos * _Weight[i];
        vUV += _Patch[i].vUV * _Weight[i];        
        vNormal += _Patch[i].vNormal * _Weight[i];
    }
    
    
    if (IsHeightMap)
    {
        float2 FullUV = vUV / float2(FACE_X, FACE_Z);
        vLocalPos.y = HEIGHT_MAP.SampleLevel(g_sam_0, FullUV, 0).r;
    }
           
    output.vPosition = mul(float4(vLocalPos, 1.f), g_matWVP);
    output.vUV = vUV;
        
    output.vViewPos = mul(float4(vLocalPos, 1.f), g_matWV).xyz;
    output.vViewNormal = normalize(mul(float4(vNormal, 0.f), g_matWV).xyz);    
    
    return output;
}


struct PS_OUT
{
    float4 vColor : SV_Target;
    float4 vNormal : SV_Target1;
    float4 vPosition : SV_Target2;
    float4 vEmissive : SV_Target3;
    float4 vData : SV_Target4;
};

PS_OUT PS_LandScape(DS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    output.vColor = float4(0.4f, 0.4f, 0.4f, 1.f);
    output.vNormal = float4(_in.vViewNormal, 1.f);
    output.vPosition = float4(_in.vViewPos, 1.f);    
    
    return output;    
}





#endif