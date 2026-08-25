#include "pch.h"
#include "GameObject.h"

#include "LevelMgr.h"
#include "Level.h"
#include "Layer.h"

GameObject::GameObject()
	: m_Parent(nullptr)
	, m_LayerIdx(-1)
	, m_IsDead(false)
{
}

GameObject::GameObject(const GameObject& _Origin)
	: Entity(_Origin)
    , m_LayerIdx(-1)
    , m_IsDead(false)
	, m_Parent(nullptr)
{
	// 컴포넌트를 복사받는다.
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr == _Origin.m_arrCom[i])
			continue;

		AddComponent(_Origin.m_arrCom[i]->Clone());
	}

	// 스크립트를 복사받는다.
	for (size_t i = 0; i < _Origin.m_vecScript.size(); ++i)
	{
		AddComponent(_Origin.m_vecScript[i]->Clone());
	}

	// 자식 오브젝트 복사
	for (size_t i = 0; i < _Origin.m_vecChild.size(); ++i)
	{
		GameObject* pChild = _Origin.m_vecChild[i]->Clone();
		AddChild(pChild);
		pChild->m_LayerIdx = _Origin.m_vecChild[i]->m_LayerIdx;
	}	
}

GameObject::~GameObject()
{
}

void GameObject::AddComponent(Component* _Component)
{
	if (nullptr == _Component)
		return;

	COMPONENT_TYPE Type = _Component->GetType();

	if (COMPONENT_TYPE::SCRIPT != Type)
	{
		m_arrCom[(UINT)Type] = _Component;

		// 입력으로 들어오는 컴포넌트가 RenderComponent 중 하나다
		CRenderComponent* pRenderCom = dynamic_cast<CRenderComponent*>(_Component);
		if (pRenderCom)
		{
			// 하나의 게임오브젝트는 1개의 RenderCompnent 만 가질 수 있다.
			assert(!m_RenderCom.Get());

			m_RenderCom = pRenderCom;
		}
	}

	else
	{
		CScript* pScript = dynamic_cast<CScript*>(_Component);

		assert(pScript);

		m_vecScript.push_back(pScript);
	}

	_Component->SetOwner(this);
}

void GameObject::Begin()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if(m_arrCom[i].Get())
			m_arrCom[i]->Begin();
	}

	// 대본(Script) 수행
	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		m_vecScript[i]->Begin();
	}

	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->Begin();
	}
}

void GameObject::Tick()
{
	// 대본(Script) 수행
	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		m_vecScript[i]->Tick();
	}

	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->Tick();
	}
}

void GameObject::FinalTick()
{
	// Component
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (m_arrCom[i].Get())
			m_arrCom[i]->FinalTick();
	}

	// RegisterLayer
	RegisterToLayer();

	// Child Object
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->FinalTick();
	}

	// Transform 변경점 해제
	Transform()->Clear();
}


bool GameObject::IsAncestor(GameObject* _Other)
{
	GameObject* pAncestor = m_Parent;

	while (nullptr != pAncestor)
	{
		if (pAncestor == _Other)
			return true;

		pAncestor = pAncestor->m_Parent;
	}

	return false;
}

void GameObject::RegisterToLayer()
{
	Ptr<Layer> pLayer = LevelMgr::GetInst()->GetCurrentLevel()->GetLayer(m_LayerIdx);
	pLayer->RegisterObject(this);
}

void GameObject::DeregisterAsParent()
{
	if (nullptr == m_Parent)
	{
		Ptr<Layer> pLayer = LevelMgr::GetInst()->GetCurrentLevel()->GetLayer(m_LayerIdx);
		pLayer->DeregisterAsParent(this);
	}
}

void GameObject::DisconnectWithParent()
{
	if (nullptr == m_Parent)
		return;

	Transform()->SetChange();

	vector<Ptr<GameObject>>::iterator iter = m_Parent->m_vecChild.begin();
	for (; iter != m_Parent->m_vecChild.end(); ++iter)
	{
		if ((*iter) == this)
		{
			m_Parent->m_vecChild.erase(iter);
			m_Parent = nullptr;
			return;
		}
	}

	// 자식이 가리키고 있는 부모가, 해당 자식을 가리키고 있지 않았다.
	assert(nullptr);
}

void GameObject::RegisterToLayerAsParent()
{
	Ptr<Layer> pLayer = LevelMgr::GetInst()->GetCurrentLevel()->GetLayer(m_LayerIdx);
	pLayer->RegisterObjectAsParent(this);
}

void GameObject::AddChild(Ptr<GameObject> _Child, int _NextLayerIdx)
{
	// 최상위 부모 오브젝트인 경우
	if (nullptr == _Child->GetParent() )
	{		
		// 레벨 안에 속해있던 오브젝트였다면
		if (_Child->m_LayerIdx != -1)
		{			
			// 자신이 등록되어있는 레이어에서, 최상위 부모였던것을 취소한다.
			_Child->DeregisterAsParent();
		}		
	}

	// 최상위 부모 오브젝트가 아닌 경우
	else
	{
		// 기존 부모와의 관계를 끊는다.
		_Child->DisconnectWithParent();
	}
	
	
	// 레벨 외부에 있던 오브젝트가 자식으로 들어오는경우
	if (-1 == _Child->m_LayerIdx)
	{
		// 부모역할을 할 오브젝트와 같은 레이어 소속으로 잡아준다.
		_Child->m_LayerIdx = m_LayerIdx;

		// 현재 레벨의 상태가 Play 중이었다면
		if (nullptr != LevelMgr::GetInst()->GetCurrentLevel()
			&& LevelMgr::GetInst()->GetCurrentLevel()->GetState() == LEVEL_STATE::PLAY)
		{
			_Child->Begin();
		}
	}


	// 새로운 부모와 자식을 서로 연결해 준다.
	m_vecChild.push_back(_Child);
	_Child->m_Parent = this;

	// 자식으로 추가되면서, Layer 소속도 바꾼다.
	assert(-1 <= _NextLayerIdx && _NextLayerIdx < MAX_LAYER);
	if (_NextLayerIdx != -1)
		_Child->m_LayerIdx = _NextLayerIdx;

	// Transform 행렬을 재 계산되도록 세팅해준다.
	_Child->Transform()->SetChange();
}

void GameObject::Render()
{ 
	Transform()->Binding();

	m_RenderCom->Render();
}

void GameObject::Render_ShadowMap(Ptr<AMaterial> _ShadowMapMtrl)
{
	Transform()->Binding();

	m_RenderCom->Render_ShadowMap(_ShadowMapMtrl);
}
