#include "pch.h"
#include "Inspector.h"

#include <Engine/LevelMgr.h>
#include <Engine/Level.h>
#include <Engine/GameObject.h>
#include <Engine/components.h>

#include "TransformUI.h"
#include "Collider2DUI.h"
#include "CameraUI.h"
#include "MeshRenderUI.h"
#include "Light2DUI.h"
#include "Light3DUI.h"

#include "MeshUI.h"
#include "MeshDataUI.h"
#include "MaterialUI.h"
#include "TextureUI.h"
#include "SoundUI.h"
#include "SpriteUI.h"
#include "FlipbookUI.h"
#include "PrefabUI.h"
#include "LevelUI.h"
#include "GraphicShaderUI.h"
#include "ComputeShaderUI.h"
#include "ScriptUI.h"

#define ADDCHILD_COMPONENTUI(ClassType, ComponentType, Size)	m_arrComUI[(UINT)ComponentType] = new ClassType;\
																m_arrComUI[(UINT)ComponentType]->SetChildSize(Size);\
																AddChildUI(m_arrComUI[(UINT)ComponentType]);

#define ADDCHILD_ASSETUI(ClassType, AssetType)					m_arrAssetUI[(UINT)AssetType] = new ClassType;\
																m_arrAssetUI[(UINT)AssetType]->SetActive(false);\
																AddChildUI(m_arrAssetUI[(UINT)AssetType]);


Inspector::Inspector()
	: EditorUI("Inspector")
	, m_arrComUI{}
	, m_arrAssetUI{}
{
	// 자식UI로 ComponentUI 추가
	ADDCHILD_COMPONENTUI(TransformUI, COMPONENT_TYPE::TRANSFORM, Vec2(0.f, 150.f));
	ADDCHILD_COMPONENTUI(Collider2DUI, COMPONENT_TYPE::COLLIDER2D, Vec2(0.f, 120.f));
	ADDCHILD_COMPONENTUI(Light2DUI, COMPONENT_TYPE::LIGHT2D, Vec2(0.f, 100.f));
	ADDCHILD_COMPONENTUI(Light3DUI, COMPONENT_TYPE::LIGHT3D, Vec2(0.f, 120.f));
	ADDCHILD_COMPONENTUI(CameraUI, COMPONENT_TYPE::CAMERA, Vec2(0.f, 220.f));
	ADDCHILD_COMPONENTUI(MeshRenderUI, COMPONENT_TYPE::MESHRENDER, Vec2(0.f, 100.f));
		
		
	// 자식UI로 AssetUI 추가
	ADDCHILD_ASSETUI(MeshUI, ASSET_TYPE::MESH);
	ADDCHILD_ASSETUI(MeshDataUI, ASSET_TYPE::MESHDATA);
	ADDCHILD_ASSETUI(MaterialUI, ASSET_TYPE::MATERIAL);
	ADDCHILD_ASSETUI(TextureUI, ASSET_TYPE::TEXTURE);
	ADDCHILD_ASSETUI(SoundUI, ASSET_TYPE::SOUND);
	ADDCHILD_ASSETUI(SpriteUI, ASSET_TYPE::SPRITE);
	ADDCHILD_ASSETUI(FlipbookUI, ASSET_TYPE::FLIPBOOK);
	ADDCHILD_ASSETUI(PrefabUI, ASSET_TYPE::PREFAB);
	ADDCHILD_ASSETUI(LevelUI, ASSET_TYPE::LEVEL);
	ADDCHILD_ASSETUI(GraphicShaderUI, ASSET_TYPE::GRAPHICSHADER);
	ADDCHILD_ASSETUI(ComputeShaderUI, ASSET_TYPE::COMPUTESHADER);

	SetTargetObject(nullptr);
}

Inspector::~Inspector()
{
}

void Inspector::Tick_UI()
{
	if (nullptr == m_TargetObject)
		return;

	// ===============
	// GameObject Info
	// ===============
	wstring Name = m_TargetObject->GetName();
	string strName = string(Name.begin(), Name.end());

	ImGui::Text("ObjectName");
	ImGui::SameLine(100);
	ImGui::Text(strName.c_str());

	char szBuff[50] = {};
	_itoa_s(m_TargetObject->GetLayerIdx(), szBuff, 50, 10);
	ImGui::Text("Layer Index");
	ImGui::SameLine(100);
	ImGui::Text(szBuff);

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Separator();
}

void Inspector::SetTargetObject(Ptr<GameObject> _TargetObject)
{
	if (nullptr != m_TargetAsset)
	{
		m_arrAssetUI[(UINT)m_TargetAsset->GetType()]->SetActive(false);		
	}

	m_TargetObject = _TargetObject;
	
	// ComponentUI 에 TargetObject 를 알려줌
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr == m_arrComUI[i])
			continue;

		m_arrComUI[i]->SetTargetObject(_TargetObject);
	}	

	if (nullptr == m_TargetObject)
		return;

	// Object 가 보유한 Script 를 확인
	const vector<Ptr<CScript>>&  vecScripts = m_TargetObject->GetScripts();

	// Object 가 보유한 Script 보다 ScriptUI 의 개수가 모자라면,
	if (m_vecScriptUI.size() < vecScripts.size())
	{
		// 모자란 개수만큼 ScriptUI 를 보충한다.
		int AddCount = vecScripts.size() - m_vecScriptUI.size();

		for (int i = 0; i < AddCount; ++i)
		{
			ScriptUI* pScriptUI = new ScriptUI;
			pScriptUI->SetChildSize(Vec2(0.f, 150.f));
			AddChildUI(pScriptUI);

			m_vecScriptUI.push_back(pScriptUI);

			int idx = m_vecScriptUI.size() - 1;
			char szNum[50] = {};
			_itoa_s(idx, szNum, 50, 10);
			pScriptUI->SetSubName(szNum);
		}		
	}

	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		m_vecScriptUI[i]->SetScript(vecScripts[i].Get());
	}
}

void Inspector::SetTargetAsset(Ptr<Asset> _TargetAsset)
{
	m_TargetAsset = _TargetAsset;

	// 오브젝트 관련 정보(ComponentUI) 전부 Off 시킨다.
	SetTargetObject(nullptr);

	// 타겟팅 된 에셋에 알맞은 AssetUI 를 활성화 시킨다.
	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	{	
		if (i == (UINT)m_TargetAsset->GetType())
		{
			m_arrAssetUI[i]->SetActive(true);
			m_arrAssetUI[i]->SetTargetAsset(_TargetAsset);
		}
		else
			m_arrAssetUI[i]->SetActive(false);
	}
}


