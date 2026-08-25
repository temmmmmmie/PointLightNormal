#ifndef _FLIPBOOK
#define _FLIPBOOK

#include "value.fx"

#define LEFT_TOP            g_vec2_0
#define SLICE               g_vec2_1
#define OFFSET              g_vec2_2
#define PANEL_SIZE          g_vec2_3


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

VS_OUT VS_Flipbook(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Flipbook(VS_OUT _in) : SV_Target
{
    float4 vColor = float4(1.f, 0.f, 1.f, 1.f);
  
    // Atlas 텍스쳐가 있다면
    if (g_btex_0)
    {
        // PanelSize 를 고려한 새로운 LeftTop 의 UV 위치
        float2 LeftTop = (LEFT_TOP + SLICE * 0.5f - PANEL_SIZE * 0.5f);        
        float2 vUV = LeftTop + (PANEL_SIZE * _in.vUV) - OFFSET;
        
        vColor = g_tex_0.Sample(g_sam_0, vUV);
        
        if (LEFT_TOP.x <= vUV.x && vUV.x <= LEFT_TOP.x + SLICE.x
            && LEFT_TOP.y <= vUV.y && vUV.y <= LEFT_TOP.y + SLICE.y)
        {
            vColor = g_tex_0.Sample(g_sam_0, vUV);
        }
        else
        {
            vColor = float4(1.f, 1.f, 0.f, 1.f);
            //discard;
        }
    }
    
    if(g_int_0)
        return vColor;
    else
        vColor.rgb *= g_Light2D[0].LightColor;
 
    return vColor;
}




#endif