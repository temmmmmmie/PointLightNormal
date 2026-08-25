#ifndef _SHADOWMAP
#define _SHADOWMAP

#include "value.fx"

struct VS_IN
{
    float3 vLocalPos : POSITION;    
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float3 vProjPos : POSITION1;
    float3 vViewPos : POSITION2;
};

VS_OUT VS_ShadowMap(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);    
    output.vProjPos = output.vPosition.xyz / output.vPosition.w;    
    output.vViewPos = mul(float4(_in.vLocalPos, 1.f), g_matWV).xyz;  
    
    return output;
}

float PS_ShadowMap(VS_OUT _in) : SV_Target
{
    //return _in.vProjPos.z;
    return _in.vViewPos.z;
}




#endif