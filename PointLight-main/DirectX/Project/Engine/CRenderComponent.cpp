#include "pch.h"
#include "CRenderComponent.h"

#include "LevelMgr.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _Type)
	: Component(_Type)
	, m_FrustumCulling(true)
{
}

CRenderComponent::CRenderComponent(const CRenderComponent& _Origin)
	: Component(_Origin)
	, m_Mesh(_Origin.m_Mesh)	
{
	m_vecMtrls.resize(_Origin.m_vecMtrls.size());

	for (size_t i = 0; i < _Origin.m_vecMtrls.size(); ++i)
	{
		m_vecMtrls[i].CurMtrl = _Origin.m_vecMtrls[i].CurMtrl;
		m_vecMtrls[i].SharedMtrl = _Origin.m_vecMtrls[i].SharedMtrl;

		// 원본 오브젝트가 공유재질을 참조하고 있고, 현재 사용재질은 공유재질이 아닌경우
		if (_Origin.m_vecMtrls[i].SharedMtrl != _Origin.m_vecMtrls[i].CurMtrl)
		{
			assert(_Origin.m_vecMtrls[i].DynamicMtrl.Get());

			// 복사 렌더 컴포넌트도 별도의 동적재질을 생성한다.
			GetDynamicMaterial(i);

			// 원본 렌더컴포넌트의 동적재질 값을 현재 생성한 동적재질로 복사한다.
			*m_vecMtrls[i].DynamicMtrl.Get() = *_Origin.m_vecMtrls[i].DynamicMtrl.Get();
		}
		else
		{
			m_vecMtrls[i].CurMtrl = m_vecMtrls[i].SharedMtrl;
		}
	}
}

CRenderComponent::~CRenderComponent()
{
}

void CRenderComponent::SetMesh(Ptr<AMesh> _Mesh)
{
	// 렌더컴포넌트가 사용할 메시가 변경되면, 그것에 대응하는 재질들도 재설정 받아야 하므로 전부 초기화함
	m_Mesh = _Mesh;

	if (!m_vecMtrls.empty())
	{
		m_vecMtrls.clear();
		vector<tMtrlSet> vecMtrls;
		m_vecMtrls.swap(vecMtrls);
	}

	if (nullptr != m_Mesh)
		m_vecMtrls.resize(m_Mesh->GetSubsetCount());
}

Ptr<AMaterial> CRenderComponent::GetMaterial(UINT _Idx)
{
	return m_vecMtrls[_Idx].CurMtrl;
}

void CRenderComponent::SetMaterial(Ptr<AMaterial> _Mtrl, UINT _idx)
{
	// 재질이 변경되면 이전 재질의 복사본 객체인 DynamicMaterial 을 삭제한다.
	m_vecMtrls[_idx].SharedMtrl = _Mtrl;
	m_vecMtrls[_idx].CurMtrl = _Mtrl;
	m_vecMtrls[_idx].DynamicMtrl = nullptr;
}

Ptr<AMaterial> CRenderComponent::GetSharedMaterial(UINT _Idx)
{
	// 공유재질을 가져오는것으로 현재 사용재질을 동적재질에서 회복하도록 한다
	m_vecMtrls[_Idx].CurMtrl = m_vecMtrls[_Idx].SharedMtrl;

	return m_vecMtrls[_Idx].SharedMtrl;
}

Ptr<AMaterial> CRenderComponent::GetDynamicMaterial(UINT _Idx)
{
	// 현재 레벨의 상태가 Play 모드인지 확인
	assert(nullptr != LevelMgr::GetInst()->GetCurrentLevel()
		&& LEVEL_STATE::PLAY == LevelMgr::GetInst()->GetCurrentLevel()->GetState());

	// 원본 재질이 없다 --> Nullptr 반환
	if (nullptr == m_vecMtrls[_Idx].SharedMtrl)
	{
		m_vecMtrls[_Idx].CurMtrl = nullptr;
		m_vecMtrls[_Idx].DynamicMtrl = nullptr;
		return m_vecMtrls[_Idx].CurMtrl;
	}

	if (nullptr == m_vecMtrls[_Idx].DynamicMtrl)
	{
		m_vecMtrls[_Idx].DynamicMtrl = m_vecMtrls[_Idx].SharedMtrl->Clone();
		m_vecMtrls[_Idx].DynamicMtrl->SetName(m_vecMtrls[_Idx].SharedMtrl->GetName() + L"_Clone");
		m_vecMtrls[_Idx].CurMtrl = m_vecMtrls[_Idx].DynamicMtrl;
	}

	return m_vecMtrls[_Idx].CurMtrl;
}



void CRenderComponent::Render_ShadowMap(Ptr<AMaterial> _ShadowMapMtrl)
{
	_ShadowMapMtrl->Binding();

	for (int i = 0; i < GetMesh()->GetSubsetCount(); ++i)
	{
		GetMesh()->Render(i);
	}
}

void CRenderComponent::SaveToLevelFile(FILE* _File)
{
	// 어떤 메쉬를 참조하고 있었는지를 저장
	SaveAssetRef(m_Mesh.Get(), _File);

	// 어떤 재질을 참조하고 있었는지를 저장
	UINT iMtrlCount = GetMaterialCount();
	fwrite(&iMtrlCount, sizeof(UINT), 1, _File);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		SaveAssetRef(m_vecMtrls[i].SharedMtrl.Get(), _File);
	}
}

void CRenderComponent::LoadFromLevelFile(FILE* _File)
{
	// 어떤 메쉬를 참조하고 있었는지를 로드
	m_Mesh = LoadAssetRef<AMesh>(_File);

	// 어떤 재질을 참조하고 있었는지를 로드
	UINT iMtrlCount = GetMaterialCount();
	fread(&iMtrlCount, sizeof(UINT), 1, _File);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		m_vecMtrls[i].SharedMtrl = LoadAssetRef<AMaterial>(_File);
	}
}