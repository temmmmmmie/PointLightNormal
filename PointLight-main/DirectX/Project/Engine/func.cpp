#include "pch.h"
#include "func.h"

#include "RenderMgr.h"
#include "TaskMgr.h"

void SpawnGameObject(int LayerIdx, GameObject* _Object)
{
	tTaskInfo info = {};
	info.Type = TASK_TYPE::CREATE_OBJECT;
	info.Param0 = LayerIdx;
	info.Param1 = (DWORD_PTR)_Object;
	TaskMgr::GetInst()->AddTask(info);
}

void DestroyGameObject(GameObject* _Object)
{
	tTaskInfo info = {};
	info.Type = TASK_TYPE::DELETE_OBJECT;
	info.Param0 = (DWORD_PTR)_Object;
	TaskMgr::GetInst()->AddTask(info);
}

#include "LevelMgr.h"
void ChangeLevelState(LEVEL_STATE _NextState)
{
	tTaskInfo info = {};

	info.Type = TASK_TYPE::CHANGE_LEVEL_STATE;
	info.Param0 = (DWORD_PTR)_NextState;

	TaskMgr::GetInst()->AddTask(info);
}

void ChangeLevel(ALevel* pLevel, LEVEL_STATE _NextState)
{
	tTaskInfo info = {};

	info.Type = TASK_TYPE::CHANGE_LEVEL;
	info.Param0 = (DWORD_PTR)pLevel;
	info.Param1 = (DWORD_PTR)_NextState;

	TaskMgr::GetInst()->AddTask(info);
}

void AddChild(GameObject* _Src, GameObject* _Dest)
{
	tTaskInfo info = {};

	info.Type = TASK_TYPE::ADD_CHILD;
	info.Param0 = (DWORD_PTR)_Src;
	info.Param1 = (DWORD_PTR)_Dest;

	TaskMgr::GetInst()->AddTask(info);
}

const char* GetString(ASSET_TYPE _Type)
{
	return ASSET_TYPE_STR[(UINT)_Type];
}

const wchar_t* GetWString(ASSET_TYPE _Type)
{
	return ASSET_TYPE_WSTR[(UINT)_Type];
}

const char* GetString(COMPONENT_TYPE _Type)
{
	return COMPONENT_TYPE_STR[(UINT)_Type];
}

const wchar_t* GetWString(COMPONENT_TYPE _Type)
{
	return COMPONENT_TYPE_WSTR[(UINT)_Type];
}

void DrawDebugRect(Vec3 _WorldPos, Vec3 _WorldScale, Vec3 _WolrdRot, Vec4 _Color, bool _DepthTest, float _Duration)
{
	tDebugShapeInfo info = {};

	info.ShapeType = DEBUG_SHAPE::RECT;
	info.Color = _Color;
	info.DepthTest = _DepthTest;
	info.WorldPos = _WorldPos;
	info.Scale = _WorldScale;
	info.Rotation = _WolrdRot;
	info.Age = 0.f;
	info.Life = _Duration;
	info.matWorld = XMMatrixIdentity();

	RenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugRect(const Matrix& _matWorld, Vec4 _Color, bool _DepthTest, float _Duration)
{
	tDebugShapeInfo info = {};

	info.ShapeType = DEBUG_SHAPE::RECT;
	info.Color = _Color;
	info.DepthTest = _DepthTest;
	info.Age = 0.f;
	info.Life = _Duration;
	info.matWorld = _matWorld;

	RenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugCircle(Vec3 _WorldPos, float _Radius, Vec4 _Color, bool _DepthTest, float _Duration)
{
	tDebugShapeInfo info = {};

	info.ShapeType = DEBUG_SHAPE::CIRCLE;
	info.Color = _Color;
	info.DepthTest = _DepthTest;
	info.Age = 0.f;
	info.Life = _Duration;
	info.WorldPos = _WorldPos;
	info.Scale = Vec3(_Radius * 2.f, _Radius * 2.f, 1.f);
	info.Rotation = Vec3(0.f, 0.f, 0.f);
	info.matWorld = XMMatrixIdentity();

	RenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugCube(Vec3 _WorldPos, Vec3 _WorldScale, Vec3 _WolrdRot, Vec4 _Color, bool _DepthTest, float _Duration)
{
	tDebugShapeInfo info = {};

	info.ShapeType = DEBUG_SHAPE::CUBE;
	info.Color = _Color;
	info.DepthTest = _DepthTest;
	info.WorldPos = _WorldPos;
	info.Scale = _WorldScale;
	info.Rotation = _WolrdRot;
	info.Age = 0.f;
	info.Life = _Duration;
	info.matWorld = XMMatrixIdentity();

	RenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugCube(const Matrix& _matWorld, Vec4 _Color, bool _DepthTest, float _Duration)
{
	tDebugShapeInfo info = {};

	info.ShapeType = DEBUG_SHAPE::CUBE;
	info.Color = _Color;
	info.DepthTest = _DepthTest;
	info.Age = 0.f;
	info.Life = _Duration;
	info.matWorld = _matWorld;

	RenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugSphere(Vec3 _CenerPos, float _Radius, Vec4 _Color, bool _DepthTest, float _Duration)
{
	tDebugShapeInfo info = {};

	info.ShapeType = DEBUG_SHAPE::SPHERE;
	info.Color = _Color;
	info.DepthTest = _DepthTest;
	info.Age = 0.f;
	info.Life = _Duration;
	info.WorldPos = _CenerPos;
	info.Scale = Vec3(_Radius * 2.f, _Radius * 2.f, _Radius * 2.f);
	info.Rotation = Vec3(0.f, 0.f, 0.f);
	info.matWorld = XMMatrixIdentity();

	RenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void SaveString(const string& _String, FILE* _File)
{
	// 문자열 길이 저장
	int Len = _String.length();
	fwrite(&Len, sizeof(int), 1, _File);

	// 문자열 내용 저장
	fwrite(_String.c_str(), sizeof(char), Len, _File);
}

void SaveWString(const wstring& _String, FILE* _File)
{
	// 문자열 길이 저장
	int Len = _String.length();
	fwrite(&Len, sizeof(int), 1, _File);

	// 문자열 내용 저장
	fwrite(_String.c_str(), sizeof(wchar_t), Len, _File);
}

void LoadWString(string& _String, FILE* _File)
{
	// 문자열 길이
	int Len = 0;
	fread(&Len, sizeof(int), 1, _File);

	// 문자열 내용
	char szBuff[256] = {};
	fread(szBuff, sizeof(char), Len, _File);
	_String = szBuff;
}

void LoadWString(wstring& _String, FILE* _File)
{
	// 문자열 길이
	int Len = 0;
	fread(&Len, sizeof(int), 1, _File);

	// 문자열 내용
	wchar_t szBuff[256] = {};
	fread(szBuff, sizeof(wchar_t), Len, _File);
	_String = szBuff;
}

#include "Asset.h"
void SaveAssetRef(const class Asset* _Asset, FILE* _File)
{
	// 포인터가 null 인지 아닌지 저장
	bool Exist = _Asset;
	fwrite(&Exist, sizeof(bool), 1, _File);

	if (nullptr != _Asset)
	{
		wstring Key = _Asset->GetKey();
		wstring Path = _Asset->GetRelativePath();

		SaveWString(Key, _File);
		SaveWString(Path, _File);
	}
}


Matrix GetMatrixFromFbxMatrix(FbxAMatrix& _mat)
{
	Matrix mat;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			mat.m[i][j] = (float)_mat.Get(i, j);
		}
	}
	return mat;
}