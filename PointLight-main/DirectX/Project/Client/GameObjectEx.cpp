#include "pch.h"
#include "GameObjectEx.h"

void GameObjectEx::FinalTick()
{
	// Component
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (GetComponent((COMPONENT_TYPE)i))
			GetComponent((COMPONENT_TYPE)i)->FinalTick();
	}

	// Child Object
	const vector<Ptr<GameObject>>& vecChild = GetChild();
	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		((GameObjectEx*)vecChild[i].Get())->FinalTick();
	}

	// Transform 변경점 해제
	Transform()->Clear();
}