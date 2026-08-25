#ifndef _SPRITE
#define _SPRITE

#include "value.fx"

#define LEFT_TOP g_vec2_0
#define SLICE    g_vec2_1
#define OFFSET   g_vec2_2

struct VS_IN
{
    float3 vPos : POSITION; // Sementic
    float2 vUV : TEXCOORD; // Sementic
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_Sprite(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV ;
    
    return output;
}

float4 PS_Sprite(VS_OUT _in) : SV_Target
{
    float4 vColor = float4(1.f, 0.f, 1.f, 1.f);
    
    // Atlas 텍스쳐가 있다면
    if(g_btex_0)
    {
        float2 vUV = LEFT_TOP + (_in.vUV * SLICE) - OFFSET;
        
        if (LEFT_TOP.x <= vUV.x && vUV.x <= LEFT_TOP.x + SLICE.x
            && LEFT_TOP.y <= vUV.y && vUV.y <= LEFT_TOP.y + SLICE.y)
        {
            vColor = g_tex_0.Sample(g_sam_0, vUV);
        }
        else
        {
            discard;
        }
    }
 
    return vColor;
}

#endif