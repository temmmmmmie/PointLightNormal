#ifndef TESS_TEST
#define TESS_TEST

#include "value.fx"
#include "func.fx"

struct VS_IN
{
    float3 vPos : POSITION;    
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float3 vPos : POSITION;    
    float2 vUV : TEXCOORD;
};

VS_OUT VS_TessTest(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // 로컬 공간상의에서의 모델 좌표를 변형없이 그래도 HullShader 로 보낸다
    // 정점 분할을 로컬공간내에서 하겠다.
    output.vPos = _in.vPos;
    output.vUV = _in.vUV;
    
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
        
    output.Edge[0] = 16.f;
    output.Edge[1] = 16.f;
    output.Edge[2] = 16.f;
    output.Inside = 16.f;
    
    return output;
}

// Main HullShader
// 정점마다 수행되는 함수
struct HS_OUT
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;
};


[domain("tri")]
[partitioning("integer")]
//[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[maxtessfactor(32.f)]
[patchconstantfunc("PatchConstFunc")]
HS_OUT HS_TessTest(InputPatch<VS_OUT, 3> _InputPatch
            , int _VtxIdx : SV_OutputControlPointID
            , int _PatchIdx : SV_PrimitiveID)
{
    HS_OUT output = (HS_OUT) 0.f;
    
    output.vLocalPos = _InputPatch[_VtxIdx].vPos;
    output.vUV = _InputPatch[_VtxIdx].vUV;
    
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
};

[domain("tri")]
DS_OUT DS_TessTest(PatchTess _TessFactor, OutputPatch<HS_OUT, 3> _Patch, float3 _Weight : SV_DomainLocation)
{
    DS_OUT output = (DS_OUT) 0.f;
       
    float3 vLocalPos = (float3) 0.f;
    float2 vUV = (float2) 0.f;
    
    for (int i = 0; i < 3; ++i)
    {
        vLocalPos += _Patch[i].vLocalPos * _Weight[i];
        vUV += _Patch[i].vUV * _Weight[i];
    }
    
    output.vPosition = mul(float4(vLocalPos, 1.f), g_matWVP);
    output.vUV = vUV;
    
    return output;
}

float4 PS_TessTest(DS_OUT _in) : SV_Target
{
    return float4(1.f, 0.f, 1.f, 1.f);
}


#endif