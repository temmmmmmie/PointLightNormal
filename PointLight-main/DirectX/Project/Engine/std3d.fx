#ifndef _STD3D
#define _STD3D

#include "value.fx"
#include "func.fx"

struct VS_IN
{
    float3 vPos     : POSITION;
    float2 vUV      : TEXCOORD; 
    
    float3 vNormal  : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
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

VS_OUT VS_Std3D(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
            
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

float4 PS_Std3D(VS_OUT _in) : SV_Target
{
    float4 Albedo = float4(0.4f, 0.4f, 0.4f, 1.f);        
    
    // 색상 텍스쳐가 있으면
    if(g_btex_0)
        Albedo = g_tex_0.Sample(g_sam_0, _in.vUV);                 
    
    Albedo.rgb *= g_DiffCoef;
        
    // 노말 텍스쳐가 있으면
    float3 vViewNormal = _in.vViewNormal;
    if (g_btex_1)
    {
        // 1. 로컬 스페이스(오브젝트 스페이스) 기준으로 제작된 노말맵
        //  - 특정 모델에만 적용 가능한 제한적인 노말맵이다.
        //  - 계산 속도가 빠르다
        
        // 2. 탄젠트 스페이스 기준으로 제작된 노말맵
        //  - 모든 모델에 적용 가능한 범용적인 노말맵
        //  - 계산이 좀더 걸린다.
        
        // -1.f ~  1.f : 원래 의도
        // 0    ~ 255  : 픽셀에 정수로 변경해서 저장
        // 0.f  ~  1.f : 픽셀에 저장된 데이터를 샘플링 한 결과
        vViewNormal = g_tex_1.Sample(g_sam_0, _in.vUV).rgb;
        vViewNormal = vViewNormal * 2.f - 1.f;
        
        float3x3 matRot =
        {
            _in.vViewTangent,
            _in.vViewBinormal,            
            _in.vViewNormal,
        };
                
        vViewNormal = mul(vViewNormal, matRot);
    }    
            
    // 픽셀이 받는 빛 계산
    float3 vDiff = (float3) 0.f;
    float3 vAmb = (float3) 0.f;
    float3 vSpec = (float3) 0.f;
     
    for (int i = 0; i < Light3DCount; ++i)
    {
        CalcLight3D(_in.vViewPos, vViewNormal, i, vDiff, vAmb, vSpec);
    }
    
    // 빛 적용    
    //Albedo.xyz = (Albedo.xyz * (vDiff + vAmb)) + vSpec;
    Albedo.xyz = vDiff;
      
    return Albedo;
}


#endif