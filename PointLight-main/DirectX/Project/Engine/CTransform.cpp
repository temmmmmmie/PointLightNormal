#include "pch.h"
#include "CTransform.h"

#include "Device.h"
#include "ConstBuffer.h"

#include "GameObject.h"

CTransform::CTransform()
	: Component(COMPONENT_TYPE::TRANSFORM)
	, m_IndependentScale(false)
	, m_Changed(true)
{
}

CTransform::~CTransform()
{
}


CTransform& CTransform::operator=(const CTransform& _Other)
{
	Component::operator=(_Other);

	m_RelativePos = _Other.m_RelativePos;
	m_RelativeScale = _Other.m_RelativeScale;
	m_RelativeRot = _Other.m_RelativeRot;

	m_IndependentScale = _Other.m_IndependentScale;
	m_Changed = true;	

	return *this;
}

void CTransform::Binding()
{
	g_Trans.matWorld = m_matWorld;
	g_Trans.matWorldInv = m_matWorldInv;
	g_Trans.matWV = g_Trans.matWorld * g_Trans.matView;
	g_Trans.matWVP = g_Trans.matWV * g_Trans.matProj;

	Device::GetInst()->GetConstBuffer(CB_TYPE::TRANFORM)->SetData(&g_Trans);
	Device::GetInst()->GetConstBuffer(CB_TYPE::TRANFORM)->Binding();
}
void CTransform::BindingWithOutWorld()
{
	g_Trans.matWV = g_Trans.matWorld * g_Trans.matView;
	g_Trans.matWVP = g_Trans.matWV * g_Trans.matProj;

	Device::GetInst()->GetConstBuffer(CB_TYPE::TRANFORM)->SetData(&g_Trans);
	Device::GetInst()->GetConstBuffer(CB_TYPE::TRANFORM)->Binding();
}

void CTransform::FinalTick()
{
	if (!m_Changed && (nullptr == GetOwner()->GetParent() || !GetOwner()->GetParent()->Transform()->IsChanged()) )
	{
		return;
	} 
	else
		m_Changed = true;

	m_matWorld = XMMatrixIdentity(); // 단위행렬로 초기화

	Matrix matScale = XMMatrixScaling(m_RelativeScale.x, m_RelativeScale.y, m_RelativeScale.z);
	Matrix matTranslation = XMMatrixTranslation(m_RelativePos.x, m_RelativePos.y, m_RelativePos.z);
	Matrix matRot = XMMatrixRotationX(m_RelativeRot.x) * XMMatrixRotationY(m_RelativeRot.y) * XMMatrixRotationZ(m_RelativeRot.z);

	// 월드행렬 = 크기 x 회전(자전) x 이동 
	m_matWorld = matScale * matRot * matTranslation;

	// 방향벡터 구하기
	// 기저벡터
	m_LocalDir[(UINT)DIR::RIGHT] = XMVector3TransformNormal(Vec3::Right, matRot);
	m_LocalDir[(UINT)DIR::UP]	 = XMVector3TransformNormal(Vec3::Up, matRot);
	m_LocalDir[(UINT)DIR::FRONT] = XMVector3TransformNormal(Vec3::Front, matRot);

	for (int i = 0; i < 3; ++i)
	{
		m_WorldDir[i] = m_LocalDir[i].Normalize();
	}

	// 부모 오브젝트가 있다면
	Ptr<GameObject> pParentObj = GetOwner()->GetParent();
	if (nullptr != pParentObj)
	{
		// 부모의 상태까지 적용한 최종 월드행렬을 계산해준다.
		// 월드행렬 = 크기 x 회전(자전) x 이동 x (부모 : 크' x 자' x 이')
		const Matrix& matParentWorld = pParentObj->Transform()->GetmatWorld();

		if (m_IndependentScale)
		{
			// 월드행렬 = (크기 x 회전(자전) x 이동) x (부모크기-1)  x (부모 : 크' x 자' x 이')
			Vec3 ParentScale = pParentObj->Transform()->GetRelativeScale();
			Matrix ParentScaleInv = XMMatrixInverse(nullptr, XMMatrixScaling(ParentScale.x, ParentScale.y, ParentScale.z));
			m_matWorld = m_matWorld * ParentScaleInv* matParentWorld;
		}
		else
			m_matWorld *= matParentWorld;

		// 최종 월드행렬 기준으로 방향벡터(월드방향벡터)를 구한다.
		for (int i = 0; i < 3; ++i)
		{
			// (Vec3, 0) x Matrix
			m_WorldDir[(UINT)DIR::RIGHT]	= XMVector3TransformNormal(Vec3::Right, m_matWorld);
			m_WorldDir[(UINT)DIR::UP]		= XMVector3TransformNormal(Vec3::Up,	m_matWorld);
			m_WorldDir[(UINT)DIR::FRONT]	= XMVector3TransformNormal(Vec3::Front, m_matWorld);

			for (int i = 0; i < 3; ++i)
			{
				m_WorldDir[i].Normalize();
			}
		}
	}

	m_matWorldInv = XMMatrixInverse(nullptr, m_matWorld);
}

void CTransform::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_RelativePos, sizeof(Vec3), 1, _File);
	fwrite(&m_RelativeScale, sizeof(Vec3), 1, _File);
	fwrite(&m_RelativeRot, sizeof(Vec3), 1, _File);
	fwrite(&m_IndependentScale, sizeof(bool), 1, _File);
}

void CTransform::LoadFromLevelFile(FILE* _File)
{
	fread(&m_RelativePos, sizeof(Vec3), 1, _File);
	fread(&m_RelativeScale, sizeof(Vec3), 1, _File);
	fread(&m_RelativeRot, sizeof(Vec3), 1, _File);
	fread(&m_IndependentScale, sizeof(bool), 1, _File);
}
