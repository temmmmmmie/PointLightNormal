#pragma once
#include "AComputeShader.h"

#include "StructuredBuffer.h"

class AParticleTickCS :
    public AComputeShader
{
private:
    Ptr<StructuredBuffer>   m_ParticleBuffer;
    Ptr<StructuredBuffer>   m_SpawnCount;

public:
    void SetParticleBuffer(Ptr<StructuredBuffer> _Buffer) { m_ParticleBuffer = _Buffer; }
    void SetSpawnCount(Ptr<StructuredBuffer> _Buffer) { m_SpawnCount = _Buffer; }

private:
    virtual int Binding() override;
    virtual void CalcGroupCount() override;
    virtual void Clear() override;

public:
    AParticleTickCS();
    virtual ~AParticleTickCS();
};

