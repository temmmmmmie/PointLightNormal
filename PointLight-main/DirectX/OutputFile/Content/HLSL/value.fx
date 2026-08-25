#ifndef _VALUE
#define _VALUE

#include "struct.fx"

// b : 상수버퍼를 바인딩 할 수 있는 레지스터
cbuffer TRANSFORM : register(b0)
{
    row_major Matrix g_matWorld; // 물체의 위치 크기 회전 정보
    row_major Matrix g_matView; // 카메라가 기준이 되는 좌표계로 이동시키는 행렬
    row_major Matrix g_matProj; // View 공간에 있는 좌표를 2차원으로 투영시킴
    
    row_major Matrix g_matWorldInv;
    row_major Matrix g_matViewInv; 
    row_major Matrix g_matProjInv; 
    
    row_major Matrix g_matWV; // View 공간에 있는 좌표를 2차원으로 투영시킴
    row_major Matrix g_matWVP; // View 공간에 있는 좌표를 2차원으로 투영시킴
}

// 4096
cbuffer MATERIAL : register(b1)
{
    float4  g_DiffCoef;
    float4  g_SpecCoef;
    float4  g_AmbCoef;
    float4  g_EmissCoef;
    
    int     g_int_0;
    int     g_int_1;
    int     g_int_2;
    int     g_int_3;
    
    float   g_float_0;
    float   g_float_1;
    float   g_float_2;
    float   g_float_3;
    
    float2  g_vec2_0;
    float2  g_vec2_1;
    float2  g_vec2_2;
    float2  g_vec2_3;
    
    float4  g_vec4_0;
    float4  g_vec4_1;
    float4  g_vec4_2;
    float4  g_vec4_3;
    
    row_major Matrix g_mat_0;
    row_major Matrix g_mat_1;
        
    int g_btex_0;
    int g_btex_1;
    int g_btex_2;
    int g_btex_3;
    int g_btex_4;
    int g_btex_5;    
    int g_btexcube_0;
    int g_btexcube_1;
    
    // 3D Animation 정보
    int g_Anim;
    int g_BoneCount;
    int2 padding;
    
    row_major Matrix g_Mat_ViewProj;
}

cbuffer GLOBAL : register(b2)
{
    float2  RenderResolution;
    uint    Light2DCount;    
    uint    Light3DCount;    
    float   DT;              
    float   Time;            
    float   EngineDT;        
    float   EngineTime;      
}

// t : Texture 가 바인딩되는 레지스터
Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);
Texture2D g_tex_5 : register(t5);

TextureCube g_texcube_0 : register(t6);
TextureCube g_texcube_1 : register(t7);





StructuredBuffer<tLightInfo> g_Light2D : register(t13);
StructuredBuffer<tLightInfo> g_Light3D : register(t14);
Texture2D g_PostProcess : register(t15);

// Animation3D Bone Matrix Buffer
StructuredBuffer<matrix> g_arrBoneMat : register(t17);

// s : 샘플러가 바인딩되는 레지스터
SamplerState g_sam_0 : register(s0); // Anisotrophic
SamplerState g_sam_1 : register(s1); // MIN_MAP_MIP_POINT

#define PI 3.1415926535f
#define CAM_WORLDPOS -g_matView._41_42_43

#endif