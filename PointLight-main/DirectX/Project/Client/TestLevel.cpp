#include "pch.h"

#include <Engine/LevelMgr.h>
#include <Engine/Level.h>
#include <Engine/layer.h>
#include <Engine/GameObject.h>
#include <Engine/components.h>

#include <Source/Scripts/CCameraMoveScript.h>
#include <Source/Scripts/CPlayerScript.h>
#include <Source/Scripts/CMonsterScript.h>
#include <Source/Scripts/CMissileScript.h>

#include <Engine/AssetMgr.h>
#include <Engine/CollisionMgr.h>

void CreateTestLevel()
{
	// 레벨 생성 및 초기오브젝트 설정
	ALevel* pLevel = new ALevel;
	ChangeLevel(pLevel, LEVEL_STATE::STOP);

	pLevel->SetName(L"TestLevel");
	pLevel->GetLayer(0)->SetName(L"Default");
	pLevel->GetLayer(1)->SetName(L"Background");
	pLevel->GetLayer(2)->SetName(L"Tile");
	pLevel->GetLayer(3)->SetName(L"Player");
	pLevel->GetLayer(4)->SetName(L"Enemy");
	pLevel->GetLayer(31)->SetName(L"UI");


	Ptr<GameObject> pObject = nullptr;

	// Camera 오브젝트
	pObject = new GameObject;
	pObject->SetName(L"MainCamera");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCamera);
	pObject->AddComponent(new CCameraMoveScript);

	pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, -100.f));

	pObject->Camera()->SetOrthoScale(1.f);
	pObject->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pObject->Camera()->LayerCheckAll(); // 모든 레이어를 다 볼수 있도록 함
	pObject->Camera()->SetPriority(0); // 메인카메라로 설정

	pLevel->AddObject(0, pObject.Get());

	// SkyBox
	// 3D 기준 배경
	pObject = new GameObject;
	pObject->SetName(L"SkyBox");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CSkyBox);

	pObject->SkyBox()->SetSkyBoxType(SKYBOX_TYPE::SPHERE);
	pObject->SkyBox()->SetSkyBoxTexture(LOAD(ATexture, L"Texture\\skybox\\Sky02.jpg"));
	//pObject->SkyBox()->SetSkyBoxTexture(LOAD(ATexture, L"Texture\\skybox\\SkyWater.dds"));

	pLevel->AddObject(0, pObject.Get());


	//// 광원 설치
	//pObject = new GameObject;
	//pObject->SetName(L"DirLight");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CLight3D);

	//pObject->Transform()->SetRelativePos(Vec3(-1000.f, 1000.f, 500.f));
	//pObject->Transform()->SetRelativeRot(Vec3(XM_PI / 4.f, XM_PI / 2.f, 0.f));

	//pObject->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	//pObject->Light3D()->SetRadius(300.f);

	//pObject->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	//pObject->Light3D()->SetAmbient(Vec3(0.15f, 0.15f,0.15f));	
	//
	//pLevel->AddObject(0, pObject.Get());

	 //광원 설치
	pObject = new GameObject;
	pObject->SetName(L"PointLight");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CLight3D);

	pObject->Transform()->SetRelativePos(Vec3(0.f, 70.f, 0.f));
	pObject->Transform()->SetRelativeRot(Vec3(0, 0.f, 0.f));

	pObject->Light3D()->SetLightType(LIGHT_TYPE::POINT);
	pObject->Light3D()->SetRadius(1000.f);

	pObject->Light3D()->SetLightColor(Vec3(1.f, 0.8f, 0.7f));
	pObject->Light3D()->SetAmbient(Vec3(0.15f, 0.15f, 0.15f));

	pLevel->AddObject(0, pObject.Get());



	{
	// Player 오브젝트
	pObject = new GameObject;
	pObject->SetName(L"Player");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CPlayerScript);

	pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 224.f));
	pObject->Transform()->SetRelativeScale(Vec3(1300.f, 1300.f, 200.f));
	pObject->Transform()->SetRelativeRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	LOAD(ATexture, L"Texture\\LandScapeTexture\\gl1_ground_II_normal.TGA");

	pObject->MeshRender()->SetMesh(FIND(AMesh, L"RectMesh"));
	pObject->MeshRender()->SetMaterial(FIND(AMaterial, L"Std3D_DeferredMtrl"), 0);
	pObject->MeshRender()->GetMaterial(0)->SetTexture(TEX_0, LOAD(ATexture, L"Texture\\LandScapeTexture\\gl1_ground_II_albedo.TGA"));
	pObject->MeshRender()->SetBoundingBox(750.f);

	pLevel->AddObject(3, pObject.Get());

}

	//// Decal 오브젝트
	//Ptr<GameObject> pDecalObj = new GameObject;
	//pDecalObj->SetName(L"Decal");
	//pDecalObj->AddComponent(new CTransform);
	//pDecalObj->AddComponent(new CDecal);

	//pDecalObj->Transform()->SetRelativePos(Vec3(0.f, -250.f, 500.f));
	//pDecalObj->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 200.f));
	//pDecalObj->GetRenderCom()->SetBoundingBox(500.f);

	////pDecalObj->Decal()->SetSpecifiedObject(pObject);
	////pDecalObj->Decal()->SetDecalMode(DECAL_MODE::EMISSIVE);
	//pDecalObj->Decal()->SetDecalMode(DECAL_MODE::COLOR);
	//pDecalObj->Decal()->SetDecalTex(LOAD(ATexture, L"Texture\\MagicCircle.png"));

	//pLevel->AddObject(3, pDecalObj.Get());

	//// Tessellation Test Object
	//Ptr<AGraphicShader> pTessTestShader = new AGraphicShader;
	//pTessTestShader->CreateVertexShader(L"HLSL\\TessTest.fx", "VS_TessTest");
	//pTessTestShader->CreateHullShader(L"HLSL\\TessTest.fx", "HS_TessTest");
	//pTessTestShader->CreateDomainShader(L"HLSL\\TessTest.fx", "DS_TessTest");
	//pTessTestShader->CreatePixelShader(L"HLSL\\TessTest.fx", "PS_TessTest");
	//pTessTestShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	//pTessTestShader->SetRSType(RS_TYPE::WIRE_FRAME);



	//Ptr<AMaterial> pTessTestMtrl = new AMaterial;
	//pTessTestMtrl->SetShader(pTessTestShader);
	//pTessTestMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_OPAQUE);


	//pObject = new GameObject;
	//pObject->SetName(L"Tess Test");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);

	//pObject->MeshRender()->SetMesh(FIND(AMesh, L"RectMesh"));
	//pObject->MeshRender()->SetMaterial(pTessTestMtrl, 0);

	//pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 1000.f));
	//pObject->Transform()->SetRelativeScale(Vec3(300.f, 300.f, 0.f));

	//pLevel->AddObject(3, pObject.Get());


	//// LandScape 오브젝트
	//pObject = new GameObject;
	//pObject->SetName(L"LandScape");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CLandScape);

	//pObject->Transform()->SetRelativePos(Vec3(0.f, -1000.f, 0.f));
	//pObject->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1000.f));

	////pObject->LandScape()->SetHeightMap(LOAD(ATexture, L"Texture\\HeightMap\\HeightMap_01.jpg"));
	//pObject->LandScape()->CreateHeightMap(1024, 1024);
	//pObject->LandScape()->SetFace(10, 10);
	//pObject->LandScape()->SetFrustumCulling(false);

	//pLevel->AddObject(3, pObject.Get());	

	// Particle Object
	//pObject = new GameObject;
	//pObject->SetName(L"Particle");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CParticleRender);

	//pObject->Transform()->SetRelativePos(Vec3(-300.f, 0.f, 200.f));
	//pObject->ParticleRender()->SetParticleTexture(LOAD(ATexture, L"Texture\\particle\\CartoonSmoke.png", L"Texture\\particle\\Bubbles99px.png"));

	//pLevel->AddObject(0, pObject.Get());

	
	// ============
	// FBX Loading
	// ============	
	{
		Ptr<AMeshData> pMeshData = nullptr;
		GameObject* pObj = nullptr;

		pMeshData = AssetMgr::GetInst()->LoadFBX(L"FBX\\Crow.fbx");

		pObj = pMeshData->Instantiate();
		pObj->SetName(L"Crow");
		pObj->Transform()->SetRelativePos(Vec3(0.f, 0.f, 300.f));
		pObj->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 200.f));
		pObj->GetRenderCom()->SetFrustumCulling(false);
				
		pLevel->AddObject(0, pObj, false);

		pMeshData = AssetMgr::GetInst()->LoadFBX(L"FBX\\CampFire.fbx");

		pObj = pMeshData->Instantiate();
		pObj->SetName(L"CampFire");
		pObj->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
		pObj->Transform()->SetRelativeScale(Vec3(80.f, 80.f, 80.f));
		pObj->Transform()->SetRelativeRot(Vec3(0.f, XM_PI / 2.5f, 0.f));
		pObj->GetRenderCom()->SetFrustumCulling(false);

		pLevel->AddObject(0, pObj, false);

		pMeshData = AssetMgr::GetInst()->LoadFBX(L"FBX\\RealCrow.fbx");

		pObj = pMeshData->Instantiate();
		pObj->SetName(L"RealCrow");
		pObj->Transform()->SetRelativePos(Vec3(-174.f, -269.f, 193.f));
		pObj->Transform()->SetRelativeScale(Vec3(120.f, 120.f, 120.f));
		pObj->Transform()->SetRelativeRot(Vec3(0.f, - XM_PI / 4, 0.f));
		pObj->GetRenderCom()->SetFrustumCulling(false);

		pLevel->AddObject(0, pObj, false);

		pObj = pMeshData->Instantiate();
		pObj->SetName(L"RealCrow2");
		pObj->Transform()->SetRelativePos(Vec3(174.f, -172.f, 247.f));
		pObj->Transform()->SetRelativeScale(Vec3(120.f, 120.f, 120.f));
		pObj->Transform()->SetRelativeRot(Vec3(0.f, XM_PI / 4, 0.f));
		pObj->GetRenderCom()->SetFrustumCulling(false);


		pMeshData = AssetMgr::GetInst()->LoadFBX(L"FBX\\Stone01.fbx");
		pLevel->AddObject(0, pObj, false);

		pObj = pMeshData->Instantiate();
		pObj->SetName(L"Stone01");
		pObj->Transform()->SetRelativePos(Vec3(-265.f, -79.f, 1329.f));
		pObj->Transform()->SetRelativeScale(Vec3(5.f, 6.f, 5.f));
		pObj->Transform()->SetRelativeRot(Vec3(0.f, XM_PI / 20    , 0.f));
		pObj->GetRenderCom()->SetFrustumCulling(false);

		pLevel->AddObject(0, pObj, false);
	}
}
