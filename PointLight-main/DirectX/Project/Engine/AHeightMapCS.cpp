#include "pch.h"
#include "AHeightMapCS.h"

#include "StructuredBuffer.h"

AHeightMapCS::AHeightMapCS()
    : m_RaycastOut(nullptr)
{
    CreateComputeShader(L"HLSL\\heightmap.fx", "CS_HeightMap");

    m_ThreadPerGroupX = 32;
    m_ThreadPerGroupY = 32;
    m_ThreadPerGroupZ = 1;
}

AHeightMapCS::~AHeightMapCS()
{
}

int AHeightMapCS::Binding()
{
    if (nullptr == m_HeightMap)
        return E_FAIL;

    m_HeightMap->Binding_CS_UAV(0);
    m_RaycastOut->Binding_CS_SRV(20);

    m_Const.iArr[0] = m_HeightMap->GetWidth();
    m_Const.iArr[1] = m_HeightMap->GetHeight();

    m_Const.v2Arr[0] = m_BrushScale;

    if (nullptr != m_BrushTex)
    {
        m_BrushTex->Binding_CS_SRV(0);
        m_Const.bTex[0] = 1;
    }
    else
        m_Const.bTex[0] = 0;

    return S_OK;
}

void AHeightMapCS::CalcGroupCount()
{
    m_GroupX = (m_HeightMap->GetWidth() / m_ThreadPerGroupX) + !!(m_HeightMap->GetWidth() % m_ThreadPerGroupX);
    m_GroupY = (m_HeightMap->GetHeight() / m_ThreadPerGroupY) + !!(m_HeightMap->GetHeight() % m_ThreadPerGroupY);
    m_GroupZ = 1;
}

void AHeightMapCS::Clear()
{
    m_HeightMap->Clear_CS_UAV();
    m_RaycastOut->Clear_CS_SRV();
}