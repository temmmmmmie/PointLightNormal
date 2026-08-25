#include "pch.h"
#include "TaskMgr.h"

#include "LevelMgr.h"
#include "Level.h"
#include "GameObject.h"

#include "RenderMgr.h"

TaskMgr::TaskMgr()
{

}

TaskMgr::~TaskMgr()
{

}

void TaskMgr::Tick()
{
	m_Garbage.clear();

	for (size_t i = 0; i < m_vecTask.size(); ++i)
	{
		switch (m_vecTask[i].Type)
		{
		case TASK_TYPE::CREATE_OBJECT:
		{
			int LayerIdx = (int)m_vecTask[i].Param0;
			GameObject* pNewObject = (GameObject*)m_vecTask[i].Param1;
			LevelMgr::GetInst()->GetCurrentLevel()->AddObject(LayerIdx, pNewObject);

			// Level 이 Play 중에 생성된 오브젝트는, 생성시점에 Begin 을 호출받아야 한다.
			if (LevelMgr::GetInst()->GetCurrentLevel()->GetState() == LEVEL_STATE::PLAY)
			{
				pNewObject->Begin();
			}
		}
			break;
		case TASK_TYPE::DELETE_OBJECT:
		{
			GameObject* pDeleteObject = (GameObject*)m_vecTask[i].Param0;
			pDeleteObject->m_IsDead = true;
			m_Garbage.push_back(pDeleteObject);
		}
			break;

		case TASK_TYPE::ADD_CHILD:
		{
			GameObject* pChild = (GameObject*)m_vecTask[i].Param0;
			GameObject* pParent = (GameObject*)m_vecTask[i].Param1;

			// 드랍 목적지가 있는 경우
			if (nullptr != pParent)
			{
				if (pParent->IsAncestor(pChild))
					return;

				pParent->AddChild(pChild);
			}

			// 드랍 목적지가 없다
			else
			{
				// Src 가 자식타입 오브젝트라면
				if (nullptr != pChild->GetParent())
				{
					// Src 를 최상위 부모로 만들어라
					pChild->DisconnectWithParent();
					pChild->RegisterToLayerAsParent();
				}
			}

			LevelMgr::GetInst()->SetChanged();
		}
			break;
		case TASK_TYPE::CHANGE_LEVEL_STATE:
		{
			ALevel* pLevel = LevelMgr::GetInst()->GetCurrentLevel().Get();

			LEVEL_STATE NextState = (LEVEL_STATE)m_vecTask[i].Param0;

			if (pLevel->ChangeLevelState(NextState))
			{
				ChangeLevelStateProcess(pLevel, NextState);
			}
		}

			break;
		case TASK_TYPE::CHANGE_LEVEL:
		{
			ALevel* pNextLevel = (ALevel*)m_vecTask[i].Param0;
			LEVEL_STATE NextState = (LEVEL_STATE)m_vecTask[i].Param1;

			LevelMgr::GetInst()->ChangeLevel(pNextLevel);

			if (pNextLevel->ChangeLevelState(NextState))
			{
				ChangeLevelStateProcess(pNextLevel, NextState);
			}
		}
			break;
		}
	}

	m_vecTask.clear();
}

void TaskMgr::ChangeLevelStateProcess(ALevel* _Level, LEVEL_STATE _State)
{
	if (LEVEL_STATE::PLAY == _State)
	{
		// 레벨을 시작하고
		_Level->Begin();

		// RenderMgr 의 렌더링 모드를 Play 모드로(Level 내부 카메라) 설정
		RenderMgr::GetInst()->ChangeRenderMode(RENDER_MODE::PLAY);
	}
	else if (LEVEL_STATE::PAUSE == _State || LEVEL_STATE::STOP == _State)
	{
		// RenderMgr 의 렌더링 모드를 EditorCamera 모드로 설정
		RenderMgr::GetInst()->ChangeRenderMode(RENDER_MODE::EDITOR);
	}
}
