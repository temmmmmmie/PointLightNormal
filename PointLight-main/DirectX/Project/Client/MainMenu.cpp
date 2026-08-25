#include "pch.h"
#include "MainMenu.h"

#include <Engine/PathMgr.h>

#include <Engine/RenderMgr.h>

#include <Engine/LevelMgr.h>
#include <Engine/Level.h>
#include <Engine/Layer.h>
#include <Engine/GameObject.h>
#include <Engine/assets.h>

#include <Source/ScriptMgr.h>

#include "EditorMgr.h"
#include "Inspector.h"
#include "ContentUI.h"

MainMenu::MainMenu()
	: EditorUI("MainMenu")
{
}

MainMenu::~MainMenu()
{
}

void MainMenu::Tick()
{
	if (ImGui::BeginMainMenuBar())
	{
		File();

		Level();

		GameObjectMenu();

		Asset();

		Editor();

		RenderTarget();

		ImGui::EndMainMenuBar();
	}
}

void MainMenu::Tick_UI()
{
}

void MainMenu::File()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Level Save"))
		{
			SaveLevel(LevelMgr::GetInst()->GetCurrentLevel().Get(), L"Level\\Test.lv");
		}

		if (ImGui::MenuItem("Level Load"))
		{
			ALevel* pLoadedLevel = LoadLevel(L"Level\\Test.lv");
			ChangeLevel(pLoadedLevel, LEVEL_STATE::STOP);
		}

		ImGui::EndMenu();
	}
}

void MainMenu::Level()
{
	if (ImGui::BeginMenu("Level"))
	{
		Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurrentLevel();

		bool IsPlay = pCurLevel.Get() && LEVEL_STATE::PLAY == pCurLevel->GetState();
		bool IsPause = pCurLevel.Get() && LEVEL_STATE::PAUSE == pCurLevel->GetState();
		bool IsStop = pCurLevel.Get() && LEVEL_STATE::STOP == pCurLevel->GetState();

		if (ImGui::MenuItem("Play", nullptr, nullptr, !IsPlay))
		{
			// 현재 레벨을 복사해둔다.
			if (IsStop)
			{
				m_CopyLevel = pCurLevel->Clone();
			}			

			ChangeLevelState(LEVEL_STATE::PLAY);
		}

		if (ImGui::MenuItem("Pause", nullptr, nullptr, IsPlay))
		{
			ChangeLevelState(LEVEL_STATE::PAUSE);
		}

		if (ImGui::MenuItem("Stop", nullptr, nullptr, !IsStop))
		{
			ChangeLevel(m_CopyLevel.Get(), LEVEL_STATE::STOP);
			FIND_UI(Inspector, "Inspector")->SetTargetObject(nullptr);
		}

		ImGui::EndMenu();
	}
}

void MainMenu::GameObjectMenu()
{
	if (ImGui::BeginMenu("GameObject"))
	{
		if (ImGui::BeginMenu("Add Script"))
		{			
			vector<wstring> vecScriptNames;
			ScriptMgr::GetScriptInfo(vecScriptNames);

			for (size_t i = 0; i < vecScriptNames.size(); ++i)
			{				
				if (ImGui::MenuItem(EditorUI::GetString(vecScriptNames[i]).c_str()))
				{					
					Ptr<GameObject> Object = FIND_UI(Inspector, "Inspector")->GetTargetObject();
					if (nullptr != Object)
					{						
						Object->AddComponent(ScriptMgr::GetScript(vecScriptNames[i]));
						FIND_UI(Inspector, "Inspector")->SetTargetObject(Object);
					}
				}
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}
}

void MainMenu::Asset()
{
	if (ImGui::BeginMenu("Asset"))
	{
		if (ImGui::MenuItem("Create Empty Material"))
		{
			// 재질 생성 및 AssetMgr 등록
			Ptr<AMaterial> pMtrl = new AMaterial;
			wstring Name = GetAssetName(ASSET_TYPE::MATERIAL, L"EmptyMaterial");
			AssetMgr::GetInst()->AddAsset(Name, pMtrl.Get());

			// ContentUI 트리 재구성
			FIND_UI(ContentUI, "ContentUI")->RenewContent();
		}

		ImGui::EndMenu();
	}
}

void MainMenu::Editor()
{
	if (ImGui::BeginMenu("Editor"))
	{
		ImGui::EndMenu();
	}
}

void MainMenu::RenderTarget()
{
	if (ImGui::BeginMenu("RenderTarget"))
	{
		vector<Ptr<ATexture>> vecTargets;
		RenderMgr::GetInst()->GetRenderTargets(vecTargets);

		Ptr<ATexture> pSelectedTarget = RenderMgr::GetInst()->GetSelectedTarget();

		for (size_t i = 0; i < vecTargets.size(); ++i)
		{
			bool bSelected = pSelectedTarget == vecTargets[i];

			if (ImGui::MenuItem(GetString(vecTargets[i]->GetKey()).c_str(), nullptr, bSelected))
			{
				if(bSelected)
					RenderMgr::GetInst()->SetSelectedTarget(nullptr);
				else
					RenderMgr::GetInst()->SetSelectedTarget(vecTargets[i]);
			}
		}

		ImGui::EndMenu();
	}
}

wstring MainMenu::GetAssetName(ASSET_TYPE _Type, const wstring& _Name)
{
	int i = 0;
	while (true)
	{
		wchar_t Num[50] = {};
		swprintf_s(Num, 50, L"_%d", i);

		wstring Ext;

		switch (_Type)
		{
		case ASSET_TYPE::MESH:
			Ext = L".mesh";
			break;
		case ASSET_TYPE::MESHDATA:
			Ext = L".mdat";
			break;
		case ASSET_TYPE::MATERIAL:
			Ext = L".mtrl";
			break;
		case ASSET_TYPE::TEXTURE:
		case ASSET_TYPE::SOUND:
		case ASSET_TYPE::GRAPHICSHADER:
		case ASSET_TYPE::COMPUTESHADER:
			assert(nullptr);
			break;
		case ASSET_TYPE::SPRITE:
			Ext = L".sprite";
			break;
		case ASSET_TYPE::FLIPBOOK:
			Ext = L".flip";
			break;
		case ASSET_TYPE::PREFAB:
			Ext = L".pref";
			break;
		case ASSET_TYPE::LEVEL:
			Ext = L".lv";
			break;
		}

		wstring AssetName = wstring(_Name + Num + Ext);
		if (nullptr == AssetMgr::GetInst()->Find(_Type, AssetName))
		{
			return AssetName;
		}

		i++;
	}
}

void MainMenu::SaveLevel(ALevel* Level, const wstring& _RelativePath)
{
	assert(Level);

	wstring Filepath = PathMgr::GetInst()->GetContentPath() + _RelativePath;

	FILE* pFile = nullptr;

	_wfopen_s(&pFile, Filepath.c_str(), L"wb");

	// 레벨의 이름 저장
	SaveWString(Level->GetName(), pFile);

	// 레벨안에 Layer 정보 저장
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// 레이어 이름 저장
		SaveWString(Level->GetLayer(i)->GetName(), pFile);

		// 레이어에 속한 GameObject 정보 저장
		const vector<Ptr<GameObject>>& vecParents = Level->GetLayer(i)->GetParentObjects();

		// 오브젝트 개수 저장
		UINT ObjCount = (UINT)vecParents.size();
		fwrite(&ObjCount, sizeof(UINT), 1, pFile);

		for (size_t j = 0; j < vecParents.size(); ++j)
		{
			SaveGameObject(vecParents[j].Get(), pFile);
		}
	}


	fclose(pFile);
}

ALevel* MainMenu::LoadLevel(const wstring& _RelativePath)
{
	wstring Filepath = PathMgr::GetInst()->GetContentPath() + _RelativePath;

	FILE* pFile = nullptr;

	_wfopen_s(&pFile, Filepath.c_str(), L"rb");

	// 레벨 생성
	ALevel* pLevel = new ALevel;

	// 레벨의 이름
	wstring LevelName;
	LoadWString(LevelName, pFile);
	pLevel->SetName(LevelName);

	// 레벨안에 Layer 정보
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// 레이어 이름
		wstring LayerName;
		LoadWString(LayerName, pFile);
		pLevel->GetLayer(i)->SetName(LayerName);

		// 레이어에 속한 최상위 부모 GameObject
		// 오브젝트 개수
		UINT ObjCount = 0;
		fread(&ObjCount, sizeof(UINT), 1, pFile);

		for (size_t j = 0; j < ObjCount; ++j)
		{
			GameObject* pObject = LoadGameObject(pFile);
			pLevel->GetLayer(i)->AddObject(pObject, false);
		}
	}

	fclose(pFile);

	return pLevel;
}

void MainMenu::SaveGameObject(GameObject* _Object, FILE* _File)
{
	// GameObject 이름
	SaveWString(_Object->GetName(), _File);

	// GameObject 컴포넌트 정보
	UINT ComType = 0;
	for (ComType; ComType < (UINT)COMPONENT_TYPE::END; ++ComType)
	{
		Component* pComponent = _Object->GetComponent((COMPONENT_TYPE)ComType);
		if (nullptr == pComponent)
			continue;

		// 컴포넌트 타입 저장
		fwrite(&ComType, sizeof(UINT), 1, _File);

		// 컴포넌트 정보 저장
		pComponent->SaveToLevelFile(_File);
	}
		
	// END 저장
	fwrite(&ComType, sizeof(UINT), 1, _File);


	// GameObject 스크립트 정보
	UINT ScriptCount = _Object->GetScripts().size();
	fwrite(&ScriptCount, sizeof(UINT), 1, _File);

	for (UINT i = 0; i < ScriptCount; ++i)
	{
		// Script 이름 저장
		wstring ScriptName = ScriptMgr::GetScriptName(_Object->GetScripts()[i].Get());
		SaveWString(ScriptName, _File);

		// Script 정보 저장
		_Object->GetScripts()[i]->SaveToLevelFile(_File);
	}

	// GameObject 의 자식 오브젝트
	UINT ChildCount = _Object->GetChild().size();
	fwrite(&ChildCount, sizeof(UINT), 1, _File);

	for (UINT i = 0; i < ChildCount; ++i)
	{
		// 자식 오브젝트의 소속 레이어 정보
		int Idx = _Object->GetChild()[i]->GetLayerIdx();
		fwrite(&Idx, sizeof(int), 1, _File);
		
		SaveGameObject(_Object->GetChild()[i].Get(), _File);
	}
}

GameObject* MainMenu::LoadGameObject(FILE* _File)
{
	GameObject* pObject = new GameObject;

	// GameObject 이름
	wstring ObjName;
	LoadWString(ObjName, _File);
	pObject->SetName(ObjName);

	// GameObject 컴포넌트 정보
	COMPONENT_TYPE Type = COMPONENT_TYPE::END;
	while(true)
	{
		// 컴포넌트 타입
		fread(&Type, sizeof(UINT), 1, _File);
		if (Type == COMPONENT_TYPE::END)
			break;

		Component* pComponent = nullptr;

		switch (Type)
		{
		case COMPONENT_TYPE::CAMERA:
			pComponent = new CCamera;
			break;
		case COMPONENT_TYPE::LIGHT2D:
			pComponent = new CLight2D;
			break;
		case COMPONENT_TYPE::LIGHT3D:
			//pComponent = new CLight3D;
			break;
		case COMPONENT_TYPE::MESHRENDER:
			pComponent = new CMeshRender;
			break;
		case COMPONENT_TYPE::SPRITE_RENDER:
			pComponent = new CSpriteRender;
			break;
		case COMPONENT_TYPE::FLIPBOOK_RENDER:
			pComponent = new CFlipbookRender;
			break;
		case COMPONENT_TYPE::TILE_RENDER:
			pComponent = new CTileRender;
			break;
		case COMPONENT_TYPE::PARTICLE_RENDER:
			pComponent = new CParticleRender;
			break;
		case COMPONENT_TYPE::SKYBOX:			
		case COMPONENT_TYPE::DECAL:
		case COMPONENT_TYPE::LANDSCAPE:
		case COMPONENT_TYPE::TRANSFORM:
			pComponent = new CTransform;
			break;
		case COMPONENT_TYPE::COLLIDER2D:
			pComponent = new CCollider2D;
			break;
		case COMPONENT_TYPE::COLLIDER3D:
			//pComponent = new CCollider3D;
			break;
		}

		// 컴포넌트 정보 불러오기
		pComponent->LoadFromLevelFile(_File);

		// 오브젝트에 추가
		pObject->AddComponent(pComponent);
	}

	// GameObject 스크립트 정보
	UINT ScriptCount = 0;
	fread(&ScriptCount, sizeof(UINT), 1, _File);

	for (UINT i = 0; i < ScriptCount; ++i)
	{
		// Script 이름
		wstring ScriptName;
		LoadWString(ScriptName, _File);

		CScript* pScript = ScriptMgr::GetScript(ScriptName);
		pScript->LoadFromLevelFile(_File);

		pObject->AddComponent(pScript);
	}

	// GameObject 의 자식 오브젝트
	UINT ChildCount = 0;
	fread(&ChildCount, sizeof(UINT), 1, _File);

	for (UINT i = 0; i < ChildCount; ++i)
	{
		// 자식 오브젝트의 소속 레이어 정보
		int Idx = -1;
		fread(&Idx, sizeof(int), 1, _File);

		GameObject* pChildObj = LoadGameObject(_File);
		pObject->AddChild(pChildObj, Idx);
	}

	return pObject;
}
