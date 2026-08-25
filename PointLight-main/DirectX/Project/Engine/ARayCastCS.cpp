#include "pch.h"
#include "ARaycastCS.h"

#include "StructuredBuffer.h"

ARaycastCS::ARaycastCS()
	: m_FaceX(0)
	, m_FaceZ(0)
	, m_Ray{}
	, m_OutBuffer(nullptr)
{
	CreateComputeShader(L"HLSL\\raycast.fx", "CS_Raycast");

	m_ThreadPerGroupX = 32;
	m_ThreadPerGroupY = 32;
	m_ThreadPerGroupZ = 1;
}

ARaycastCS::~ARaycastCS()
{
}

int ARaycastCS::Binding()
{
	if (nullptr == m_OutBuffer)
		return E_FAIL;

	// Raycasting 을 정확하게 계산하기위해서 t0 에 높이맵도 전달
	m_Const.bTex[0] = !!m_HeightMap.Get();
	m_HeightMap->Binding_CS_SRV(0);

	m_OutBuffer->Binding_CS_UAV(0);

	m_Const.iArr[0] = m_FaceX;
	m_Const.iArr[1] = m_FaceZ;

	m_Const.v4Arr[0] = m_Ray.vStart;
	m_Const.v4Arr[1] = m_Ray.vDir;

	return S_OK;
}

void ARaycastCS::CalcGroupCount()
{
	m_GroupX = (m_FaceX * 2) / m_ThreadPerGroupX;
	m_GroupY =  m_FaceZ / m_ThreadPerGroupY;
	m_GroupZ = 1;

	if (0 < (m_FaceX * 2) % m_ThreadPerGroupX)
		m_GroupX += 1;

	if (0 < m_FaceZ % m_ThreadPerGroupY)
		m_GroupY += 1;
}

void ARaycastCS::Clear()
{
	m_OutBuffer->Clear_CS_UAV();
	m_OutBuffer = nullptr;

	m_HeightMap->Clear_CS_SRV();
	m_HeightMap = nullptr;
}