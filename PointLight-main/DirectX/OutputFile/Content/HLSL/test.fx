#ifndef _TEST
#define _TEST

#include "value.fx"

// t 레지스터는 바인딩 된 데이터를 읽기만 가능
//Texture2D g_Target : register(t16);

// ComputeShader 전용 레지스터 u(Unorderd Access Register)
RWTexture2D<float4> g_Target : register(u0);


// 그룹당 스레드 수 : limit( HLSL 5.0 1024 )
[numthreads(32, 32, 1)]
void CS_SetColor(int3 _ID : SV_DispatchThreadID)
{
    // SV_GroupID           : 스레드가 속한 그룹의 인덱스 ID
    // SV_GroupThreadID     : 스레드가 속한 그룹 내에서 스레드의 인덱스 ID
    // SV_GroupIndex        : 스레드가 속한 그룹 내에서 스레드의 1차원 인덱스 ID
    // SV_DispatchThreadID  : 모든 스레드를 통틀어서 해당 스레드의 고유한 인덱스
    
    g_Target[_ID.xy] = g_vec4_0;
}


#endif