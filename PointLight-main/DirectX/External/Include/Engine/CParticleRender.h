#pragma once
#include "CRenderComponent.h"

#include "StructuredBuffer.h"
#include "AParticleTickCS.h"

struct tSpawnCount
{
    int SpawnCount;
    int Padding[3];
};


// 입자(파티클) 시뮬레이션 및 렌더링
class CParticleRender :
    public CRenderComponent
{
private:
    UINT                    m_MaxParticle;      // 최대 파티클 개수
    Ptr<StructuredBuffer>   m_ParticleBuffer;   // 파티클 정보가 저장되어있는 버퍼
    Ptr<StructuredBuffer>   m_SpawnCount;       // 파티클을 몇개 활성화 시킬지를 저장한 버퍼

    Ptr<ATexture>           m_ParticleTex;      // 파티클이 사용할 텍스쳐
    Ptr<AParticleTickCS>    m_TickCS;

    float                   m_SpawnRate;        // 초당 파티클 생성 개수
    float                   m_AccTime;

public:
    void SetParticleTexture(Ptr<ATexture> _Tex) { m_ParticleTex = _Tex; }

private:
    void CalcSpawnCount();

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

private:
    void CreateMaterial();

    CLONE_DISABLE(CParticleRender);
public:    
    CParticleRender();
    virtual ~CParticleRender();
};

