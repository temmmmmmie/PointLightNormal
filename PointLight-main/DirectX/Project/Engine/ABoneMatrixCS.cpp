#include "pch.h"
#include "ABoneMatrixCS.h"
#include "StructuredBuffer.h"

ABoneMatrixCS::ABoneMatrixCS()
	: m_FrameDataBuffer(nullptr)
	, m_OffsetMatBuffer(nullptr)
	, m_OutputBuffer(nullptr)
{
	CreateComputeShader(L"HLSL\\bonemat.fx", "CS_Animation3D");

	m_ThreadPerGroupX = 256;
	m_ThreadPerGroupY = 1;
	m_ThreadPerGroupZ = 1;
}

ABoneMatrixCS::~ABoneMatrixCS()
{
}

int ABoneMatrixCS::Binding()
{
	// 구조화버퍼 전달
	m_FrameDataBuffer->Binding_CS_SRV(16); // t16
	m_OffsetMatBuffer->Binding_CS_SRV(17); // t17
	m_OutputBuffer->Binding_CS_UAV(0);   // u0

	return S_OK;
}

void ABoneMatrixCS::CalcGroupCount()
{
	m_GroupX = (m_Const.iArr[0] / m_ThreadPerGroupX) + 1;
	m_GroupY = 1;
	m_GroupZ = 1;
}

void ABoneMatrixCS::Clear()
{
	m_FrameDataBuffer->Clear_CS_SRV();
	m_OffsetMatBuffer->Clear_CS_SRV();
	m_OutputBuffer->Clear_CS_UAV();

	m_FrameDataBuffer = nullptr;
	m_OffsetMatBuffer = nullptr;
	m_OutputBuffer = nullptr;
}