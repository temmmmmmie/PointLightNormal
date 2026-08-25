#include "pch.h"
#include "AWeightMapCS.h"
#include "StructuredBuffer.h"

AWeightMapCS::AWeightMapCS()
	: AComputeShader()
	, m_WeightMap(nullptr)
	, m_RaycastOut(nullptr)
	, m_WeightIdx(0)
	, m_WeightMapWidth(0)
	, m_WeightMapHeight(0)
{
	CreateComputeShader(L"HLSL\\weightmap.fx", "CS_WeightMap");

	m_ThreadPerGroupX = 32;
	m_ThreadPerGroupY = 32;
	m_ThreadPerGroupZ = 1;
}

AWeightMapCS::~AWeightMapCS()
{
}

int AWeightMapCS::Binding()
{
	if (nullptr == m_WeightMap || nullptr == m_BrushTex || nullptr == m_RaycastOut
		|| 0 == m_WeightMapWidth || 0 == m_WeightMapHeight)
		return E_FAIL;

	m_Const.iArr[0] = m_WeightMapWidth;
	m_Const.iArr[1] = m_WeightMapHeight;
	m_Const.iArr[2] = m_WeightIdx;
	m_Const.v2Arr[0] = m_BrushScale;

	m_BrushTex->Binding_CS_SRV(0);
	m_WeightMap->Binding_CS_UAV(0);
	m_RaycastOut->Binding_CS_SRV(20);

	return S_OK;
}

void AWeightMapCS::CalcGroupCount()
{
	m_GroupX = m_WeightMapWidth / m_ThreadPerGroupX;
	m_GroupY = m_WeightMapHeight / m_ThreadPerGroupY;
	m_GroupZ = 1;

	if (0 < (UINT)m_WeightMapWidth % m_ThreadPerGroupX)
		m_GroupX += 1;

	if (0 < (UINT)m_WeightMapHeight % m_ThreadPerGroupY)
		m_GroupY += 1;
}

void AWeightMapCS::Clear()
{
	m_BrushTex->Clear_CS_SRV();
	m_WeightMap->Clear_CS_UAV();
	m_RaycastOut->Clear_CS_SRV();
}