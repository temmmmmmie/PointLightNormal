#ifndef _STRUCT
#define _STRUCT



struct tLightInfo
{
    uint    Type;       // 광원 타입
    float3  LightColor; // 빛의 색상
    float3  Ambient;    // 빛으로 인해서 발생하는 최소한의 빛(환경광)
    float3  WorldPos;   // 광원의 위치
    float3  Direction;  // 광원의 빛이 향하는 방향
    float   Radius;     // 광원의 영향 범위
    float   Angle;      // 광원을 발사하는 각도범위
};



struct tParticle
{
    float3  WorldPos;
    float3  WorldScale;
    float4  Color;
    
    float3  Velocity;

    float   Mass;
    float   Age; // 파티클 나이(생성 이후 시간)
    float   Life; // 파티클 수명(최대 유지시간)
    float   NormalizedAge; // 파티클의 나이를 수명 대비 정규화한 값(Age/Life)

    int     Active; // 활성화 상태
};


struct tRaycastOut
{
    float2 Location;
    uint Distance;
    int Success;
};



struct tSkinningInfo
{
    float3 vPos;
    float3 vTangent;
    float3 vBinormal;
    float3 vNormal;
};

#endif