#include "pch.h"
#include "Level.h"

#include "LevelMgr.h"
#include "RenderMgr.h"

ALevel::ALevel()
	: Asset(ASSET_TYPE::LEVEL)
	, m_CurState((LEVEL_STATE)-1)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i] = new Layer(i);
	}

	ChangeLevelState(LEVEL_STATE::STOP);
}

ALevel::ALevel(const ALevel& _Origin)
	: Asset(_Origin)
	, m_arrLayer{}
	, m_CurState((LEVEL_STATE)-1)
{
	assert(LEVEL_STATE::STOP == _Origin.m_CurState);

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i] = _Origin.m_arrLayer[i]->Clone();		
	}
}

ALevel::~ALevel()
{
}

void ALevel::Begin()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->Begin();
	}
}

void ALevel::Tick()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->Tick();
	}
}

void ALevel::FinalTick()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->FinalTick();
	}
}

void ALevel::AddObject(int _LayerIdx, GameObject* _Object, bool _MoveWithChild)
{
	// 자신이 현재 레벨이라면, 변경점이 발생했음을 알림
	if (LevelMgr::GetInst()->GetCurrentLevel() == this)
		LevelMgr::GetInst()->SetChanged();

	m_arrLayer[_LayerIdx]->AddObject(_Object, _MoveWithChild);
}

Ptr<GameObject> ALevel::FindObjectByName(const wstring& _Name)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		const vector<Ptr<GameObject>>& vecParents = m_arrLayer[i]->GetParentObjects();

		list<GameObject*> queue;

		for (size_t j = 0; j < vecParents.size(); ++j)
		{
			queue.clear();
			queue.push_back(vecParents[j].Get());

			while (!queue.empty())
			{
				GameObject* pObject = queue.front();
				if (pObject->GetName() == _Name)
					return pObject;

				queue.pop_front();
				for (size_t k = 0; k < pObject->GetChild().size(); ++k)
				{
					queue.push_back(pObject->GetChild()[k].Get());					
				}				
			}
		}
	}

	return nullptr;
}

void ALevel::Deregister()
{
	// 이전 프레임에 등록된 GameObject 들 정리
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->Deregister();
	}
}

bool ALevel::ChangeLevelState(LEVEL_STATE _NextState)
{
	if (m_CurState == _NextState)
		return false;

	m_CurState = _NextState;

	return true;
}