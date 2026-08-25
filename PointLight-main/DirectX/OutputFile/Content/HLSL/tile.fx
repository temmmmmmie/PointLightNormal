#ifndef _TILE
#define _TILE

#include "value.fx"
#include "func.fx"

#define COL     g_int_0
#define ROW     g_int_1

struct tTileInfo
{
    float2 LeftTop;
    float2 Slice;
};
StructuredBuffer<tTileInfo> g_TileInfo : register(t18);

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    float3 vWorldPos : POSITION;
};

VS_OUT VS_Tile(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    _in.vPos.xy += float2(0.5f, -0.5f);
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV * float2(COL, ROW);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
    return output;
}

float4 PS_Tile(VS_OUT _in) : SV_Target
{
    float4 vColor = float4(1.f, 0.f, 0.f, 1.f);
     
    int2 ColRow = (int2) _in.vUV;
    int TileIdx = ColRow.y * COL + ColRow.x;
 
    if (g_btex_0)
    {
        float2 vUV = g_TileInfo[TileIdx].LeftTop + frac(_in.vUV) * g_TileInfo[TileIdx].Slice;
        vColor = g_tex_0.Sample(g_sam_0, vUV);
    }
    
    // ±¤¿ø ¿¬»ê
    float3 LightColor = (float3) 0.f;
    CalcLight2D(_in.vWorldPos, LightColor);
    vColor.rgb *= LightColor;    
    
    return vColor;
}

#endif