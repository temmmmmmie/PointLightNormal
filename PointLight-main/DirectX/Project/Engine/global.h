#pragma once

// Windows 라이브러리
#include <Windows.h>

// DirectX11 라이브러리 링크
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
using namespace DirectX;
using namespace DirectX::PackedVector;


#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxguid")

// DiretXTex 라이브러리 링크
#include <DirectxTex/DirectXTex.h>

#ifndef _DEBUG
#pragma comment(lib, "DirectxTex/DirectXTex")
#else
#pragma comment(lib, "DirectxTex/DirectXTex_debug")
#endif

// ComPtr
#include <wrl.h>
using namespace Microsoft::WRL;

// SimpleMath
#include "SimpleMath.h"
using namespace DirectX::SimpleMath;


// Std 라이브러리
#include <string>
#include <vector>
#include <list>
#include <map>
#include <filesystem>
#include <typeinfo>

using std::string;
using std::wstring;
using std::vector;
using std::list;
using std::map;
using std::make_pair;
using namespace std::filesystem;

// FMOD
#include <FMOD/fmod.h>
#include <FMOD/fmod.hpp>
#include <FMOD/fmod_codec.h>

#ifdef _DEBUG
#pragma comment(lib, "FMOD/fmodL64_vc.lib")
#else
#pragma comment(lib, "FMOD/fmod64_vc.lib")
#endif


// Fbx Loader
#include <FBXLoader/fbxsdk.h>
#ifdef _DEBUG
#pragma comment(lib, "FBXLoader/x64/debug/libfbxsdk-md.lib")
#else
#pragma comment(lib, "FBXLoader/x64/release/libfbxsdk-md.lib")
#endif

// Engine 헤더
#include "singleton.h"
#include "define.h"
#include "enum.h"
#include "struct.h"
#include "Ptr.h"
#include "func.h"
