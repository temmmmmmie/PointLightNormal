#ifndef _VOLUEMESH
#define _VOLUEMESH

#include "value.fx"

struct VS_IN
{
    float3 vLocalPos : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
};

VS_OUT VS_VolumeMesh( VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
       
    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);    

    return output;
}

#endif