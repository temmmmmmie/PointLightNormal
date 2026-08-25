#include "pch.h"
#include "CParticleRender.h"

#include "AssetMgr.h"
#include "CTransform.h"

#include "TimeMgr.h"

CParticleRender::CParticleRender()
	: CRenderComponent(COMPONENT_TYPE::PARTICLE_RENDER)
	, m_MaxParticle(200)
	, m_SpawnRate(1.f) // 초당 파티클을 10개 생성
	, m_AccTime(0.f)
{
	tParticle arrParticle[200] = {};

	float Theta = XM_2PI / m_MaxParticle;
	float Radius = 10.f;
	for (int i = 0; i < m_MaxParticle; ++i)
	{
		arrParticle[i].WorldPos = Vec3(Radius * cosf(Theta * i), Radius * sinf(Theta * i), 0.f);
		arrParticle[i].Velocity = arrParticle[i].WorldPos;
		arrParticle[i].Velocity.Normalize();
		arrParticle[i].Velocity *= 100.f;

		arrParticle[i].WorldScale = Vec3(10.f, 10.f, 1.f);
		arrParticle[i].Color = Vec4(1.f, 0.f, 0.f, 1.f);	

		arrParticle[i].Active = false;
		arrParticle[i].Life = 3.f;
		arrParticle[i].Age = 0.f;
	}

	m_ParticleBuffer = new StructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_MaxParticle, SB_TYPE::SRV_UAV, false, arrParticle);

	m_SpawnCount = new StructuredBuffer;
	m_SpawnCount->Create(sizeof(int), 1, SB_TYPE::SRV_UAV, true);

	// Particle 입자가 사용할 Mesh 설정
	SetMesh(FIND(AMesh, L"PointMesh"));

	// ParticleRender 전용 재질 생성 및 설정
	CreateMaterial();

	m_TickCS = new AParticleTickCS;
}

CParticleRender::~CParticleRender()
{
	
}


void CParticleRender::FinalTick()
{
	// 이번 프레임에 활성화 시킬 파티클의 개수
	CalcSpawnCount();

	m_TickCS->SetParticleBuffer(m_ParticleBuffer);
	m_TickCS->SetSpawnCount(m_SpawnCount);

	m_TickCS->Execute();
}

void CParticleRender::CalcSpawnCount()
{
	float Limit = 1.f / m_SpawnRate;

	m_AccTime += DT;
	if (Limit < m_AccTime)
	{
		m_AccTime -= Limit;

		int SpawnCount = 3;
		m_SpawnCount->SetData(&SpawnCount);
	}
	else
	{
		int SpawnCount = 0;
		m_SpawnCount->SetData(&SpawnCount);
	}
}



void CParticleRender::Render()
{
	m_ParticleBuffer->Binding(17);		
	
	GetMaterial(0)->SetScalar(VEC4_0, Transform()->GetWorldPos());
	GetMaterial(0)->SetTexture(TEX_0, m_ParticleTex);
	GetMaterial(0)->Binding();

	GetMesh()->Render_Particle(m_MaxParticle);	

	m_ParticleBuffer->Clear();
}


void CParticleRender::CreateMaterial()
{
	wstring MtrlName = L"ParticleMtrl";
	wstring ShaderName = L"ParticleShader";

	Ptr<AMaterial> pMtrl = AssetMgr::GetInst()->Find<AMaterial>(MtrlName);

	if (nullptr == pMtrl)
	{
		Ptr<AGraphicShader> pShader = AssetMgr::GetInst()->Find<AGraphicShader>(ShaderName);

		if (nullptr == pShader)
		{
			pShader = new AGraphicShader;
			pShader->SetName(ShaderName);
			pShader->CreateVertexShader(L"HLSL\\particle.fx", "VS_Particle");
			pShader->CreateGeometryShader(L"HLSL\\particle.fx", "GS_Particle");
			pShader->CreatePixelShader(L"HLSL\\particle.fx", "PS_Particle");
			pShader->SetRSType(RS_TYPE::CULL_NONE);
			pShader->SetBSType(BS_TYPE::ALPHABLEND);
			pShader->SetDSType(DS_TYPE::NO_WRITE);

			// 파티클 쉐이더 입력으로 PointMesh 를 사용하기 때문
			pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

			AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());
		}

		pMtrl = new AMaterial;
		pMtrl->SetName(MtrlName);
		pMtrl->SetShader(pShader);
		pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_PARTICLE);
		AssetMgr::GetInst()->AddAsset(pMtrl->GetName(), pMtrl.Get());
	}

	SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(MtrlName), 0);
}


void CParticleRender::SaveToLevelFile(FILE* _File)
{
	CRenderComponent::SaveToLevelFile(_File);


}

void CParticleRender::LoadFromLevelFile(FILE* _File)
{
	CRenderComponent::LoadFromLevelFile(_File);


}