#ifndef _STD3D_DEFERRED
#define _STD3D_DEFERRED

#include "value.fx"
#include "func.fx"

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    
    // 3D Animatino 관련 정보
    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float Pow : FOG;
    
    float3 vViewPos : POSTION;
    
    float3 vViewNormal : NORMAL;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;
};

VS_OUT VS_Std3D_Deferred(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
            
    //if (g_Anim)
    //{
    //    Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal
    //          , _in.vWeights, _in.vIndices, 0);
    //}
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
            
    // 정점의 월드공간상에서의 위치를 구한다.
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    
    // 로컬공간에서의 Normal 방향을 World 공간으로 변경
    // 이동정보는 무시(동차좌표 0 으로 설정)
    // 월드행렬을 곱한 결과를 정규화(Normalize), 크기가 변경됐을 수 있기 때문
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV).xyz);
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV).xyz);
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV).xyz);
    
    return output;
}

struct PS_OUT
{
    float4 vColor       : SV_Target;
    float4 vNormal      : SV_Target1;
    float4 vPosition    : SV_Target2;
    float4 vEmissive    : SV_Target3;
    float4 vData        : SV_Target4;
};

PS_OUT PS_Std3D_Deferred(VS_OUT _in) 
{    
    PS_OUT output = (PS_OUT) 0.f;
    
    output.vColor = float4(0.4f, 0.4f, 0.4f, 1.f);
    
    // 색상 텍스쳐가 있으면
    if (g_btex_0)
    {
        output.vColor.xyz = g_tex_0.Sample(g_sam_0, _in.vUV).rgb;
        //output.vEmissive.xyz = g_tex_0.Sample(g_sam_0, _in.vUV).rgb;
    }
            
    output.vNormal = float4(_in.vViewNormal, 1.f);
    
    // 노말맵이 있으면
    if (g_btex_1)
    {       
        output.vNormal.xyz = g_tex_1.Sample(g_sam_0, _in.vUV).rgb;
        output.vNormal.xyz = output.vNormal * 2.f - 1.f;
        
        float3x3 matRot =
        {
            _in.vViewTangent,
            _in.vViewBinormal,
            _in.vViewNormal,
        };
                
        output.vNormal.xyz = mul(output.vNormal.xyz, matRot);
    }
      
    // 물체의 view 공간 좌표
    output.vPosition = float4(_in.vViewPos, 1.f);
        
    // Emissive Texture 가 있으면
    if (g_btex_3)
    {
        output.vEmissive = g_tex_3.Sample(g_sam_0, _in.vUV);
    }
    
    output.vData.r = g_float_0;
    
    return output;
}


#endif