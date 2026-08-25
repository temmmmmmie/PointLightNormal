#include "pch.h"
#include "ASetColorCS.h"



ASetColorCS::ASetColorCS()
{
	CreateComputeShader(L"HLSL\\test.fx", "CS_SetColor");

	// 그룹당 스레드수 기록
	m_ThreadPerGroupX = 32;
	m_ThreadPerGroupY = 32;
	m_ThreadPerGroupZ = 1;
}

ASetColorCS::~ASetColorCS()
{
}


int ASetColorCS::Binding()
{
	if (nullptr == m_TargetTex || nullptr == m_TargetTex->GetUAV())
		return E_FAIL;

	m_TargetTex->Binding_CS_UAV(0);

	m_Const.v4Arr[0] = m_Color;

	return S_OK;
}

void ASetColorCS::CalcGroupCount()
{
	m_GroupX = m_TargetTex->GetWidth() / m_ThreadPerGroupX;
	m_GroupY = m_TargetTex->GetHeight() / m_ThreadPerGroupY;
	m_GroupZ = 1;
}

void ASetColorCS::Clear()
{
	m_TargetTex->Clear_CS_UAV();
	m_TargetTex = nullptr;
}