#include "pch.h"
#include "AParticleTickCS.h"



AParticleTickCS::AParticleTickCS()
{
	CreateComputeShader(L"HLSL\\particle_tick.fx", "CS_ParticleTick");

	// 그룹당 스레드수 기록
	m_ThreadPerGroupX = 32;
	m_ThreadPerGroupY = 1;
	m_ThreadPerGroupZ = 1;
}

AParticleTickCS::~AParticleTickCS()
{
}

int AParticleTickCS::Binding()
{
	if (nullptr == m_ParticleBuffer || nullptr == m_ParticleBuffer->GetUAV()
		|| nullptr == m_SpawnCount || nullptr == m_SpawnCount->GetUAV())
		return E_FAIL;

	m_ParticleBuffer->Binding_CS_UAV(0);
	m_SpawnCount->Binding_CS_UAV(1);

	m_Const.iArr[0] = m_ParticleBuffer->GetElementCount();

	return S_OK;
}

void AParticleTickCS::CalcGroupCount()
{
	m_GroupX = m_ParticleBuffer->GetElementCount() / m_ThreadPerGroupX;
	m_GroupY = 1;
	m_GroupZ = 1;

	if (m_ParticleBuffer->GetElementCount() % m_ThreadPerGroupX)
		m_GroupX += 1;
}

void AParticleTickCS::Clear()
{
	m_ParticleBuffer->Clear_CS_UAV();
	m_ParticleBuffer = nullptr;

	m_SpawnCount->Clear_CS_UAV();
	m_SpawnCount = nullptr;
}