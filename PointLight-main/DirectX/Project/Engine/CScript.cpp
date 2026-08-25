#include "pch.h"
#include "CScript.h"

#include "GameObject.h"

CScript::CScript(UINT _ScriptType)
	: Component(COMPONENT_TYPE::SCRIPT)
	, m_Type(_ScriptType)
{
}

CScript::~CScript()
{
}

void CScript::FinalTick()
{
}

CRenderComponent* CScript::GetRenderComponent()
{
	return GetOwner()->GetRenderCom().Get();
}
