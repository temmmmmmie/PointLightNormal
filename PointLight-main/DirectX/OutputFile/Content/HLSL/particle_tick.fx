#ifndef _PARTICLE_TICK
#define _PARTICLE_TICK

#include "value.fx"
#include "struct.fx"



#define PI 3.1415926535f

float3 GetRandom(float _NomalizedThreadID, in Texture2D _NoiseTex)
{
    float2 vUV = (float2) 0.f;
        
    vUV.x = _NomalizedThreadID + EngineTime * 0.1f;
    vUV.y = 0.5f + sin(vUV.x * 100.f * PI) * 0.5f;

    return _NoiseTex.SampleLevel(g_sam_0, vUV, 0).xyz;
}



RWStructuredBuffer<tParticle>   g_Buffer        : register(u0);
RWStructuredBuffer<int>         g_SpawnCount    : register(u1);
Texture2D                       g_NoiseTex      : register(t16);


#define MAX_PARTICLE    g_int_0
#define Particle        g_Buffer[_ID.x]

[numthreads(32, 1, 1)]
void CS_ParticleTick(int3 _ID : SV_DispatchThreadID)
{    
    if (MAX_PARTICLE <= _ID.x )
        return;
    
    
    // 1. 스레드 동기화
    if (false == Particle.Active)
    {
        // 한 Tick 에 여러개의 스래드가 활성화 될 때 문제가 발생할 수 있다.
        while (0 < g_SpawnCount[0])
        {
            int SpawnCount = g_SpawnCount[0];
            int Origin = 0;
            InterlockedCompareExchange(g_SpawnCount[0], SpawnCount, g_SpawnCount[0] - 1, Origin);
            
            if (Origin == SpawnCount)
            {
                Particle.Active = true;
                Particle.Age = 0.f;
                Particle.WorldPos = float3(0.f, 0.f, 0.f);
                break;
            }
        }
    }
    
    else
    {
        // 2. 랜덤 생성
        float NormalizedThreadID = (float) _ID.x / (float) MAX_PARTICLE;
        float3 Random = GetRandom(NormalizedThreadID, g_NoiseTex);
        
        // Min ~ Max
        // (Max - Min) * Random.x + Min;
        
        Particle.WorldPos += Particle.Velocity * DT;
        Particle.Age += DT;
        
        // 스레드가 담당한 파티클의 수명이 다한 경우
        if (Particle.Life < Particle.Age)
        {
            Particle.Active = false;
        }
    }
}

#endif