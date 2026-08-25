#pragma once

void SpawnGameObject(int LayerIdx, class GameObject* _Object);
void DestroyGameObject(class GameObject* _Object);
void ChangeLevelState(LEVEL_STATE _NextState);
void ChangeLevel(class ALevel* pLevel, LEVEL_STATE _NextState);
void AddChild(GameObject* _Src, GameObject* _Dest);


const char* GetString(ASSET_TYPE _Type);
const wchar_t* GetWString(ASSET_TYPE _Type);
const char* GetString(COMPONENT_TYPE _Type);
const wchar_t* GetWString(COMPONENT_TYPE _Type);


void DrawDebugRect(Vec3 _WorldPos, Vec3 _WorldScale, Vec3 _WolrdRot, Vec4 _Color, bool _DepthTest, float _Duration = 0.f);
void DrawDebugRect(const Matrix& _matWorld, Vec4 _Color, bool _DepthTest, float _Duration = 0.f);
void DrawDebugCircle(Vec3 _WorldPos, float _Radius, Vec4 _Color, bool _DepthTest, float _Duration = 0.f);


void DrawDebugCube(Vec3 _WorldPos, Vec3 _WorldScale, Vec3 _WolrdRot, Vec4 _Color, bool _DepthTest, float _Duration = 0.f);
void DrawDebugCube(const Matrix& _matWorld, Vec4 _Color, bool _DepthTest, float _Duration = 0.f);
void DrawDebugSphere(Vec3 _CenerPos, float _Radius, Vec4 _Color, bool _DepthTest, float _Duration = 0.f);


void SaveString(const string& _String, FILE* _File);
void SaveWString(const wstring& _String, FILE* _File);

void LoadWString(string& _String, FILE* _File);
void LoadWString(wstring& _String, FILE* _File);


void SaveAssetRef(const class Asset* _Asset, FILE* _File);

#include "AssetMgr.h"
template<typename T>
T* LoadAssetRef(FILE* _File)
{
	// 포인터가 null 인지 아닌지 확인
	bool Exist = false;
	fread(&Exist, sizeof(bool), 1, _File);

	if (false == Exist)
		return nullptr;
	
	wstring Key, Path;

	LoadWString(Key, _File);
	LoadWString(Path, _File);	

	return AssetMgr::GetInst()->Load<T>(Key, Path).Get();
}


Matrix GetMatrixFromFbxMatrix(FbxAMatrix& _mat);
