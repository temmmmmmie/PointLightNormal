#pragma once


// 상수버퍼가 대응하는 레지스터 번호
enum class CB_TYPE
{
	TRANFORM, // b0
	MATERIAL, // b1
	GLOBAL,	  // b2

	END,
};

enum class ASSET_TYPE
{
	MESH,
	MESHDATA,
	MATERIAL,
	TEXTURE,
	SOUND,
	SPRITE,
	FLIPBOOK,
	PREFAB,
	LEVEL,
	GRAPHICSHADER,
	COMPUTESHADER, // GPGPU, Nvidia - CUDA, DirectX - ComputeShader
	END,
};
extern const char* ASSET_TYPE_STR[(UINT)ASSET_TYPE::END];
extern const wchar_t* ASSET_TYPE_WSTR[(UINT)ASSET_TYPE::END];

enum class COMPONENT_TYPE
{
	CAMERA,				// 카메라 역할을 수행	
	LIGHT2D,			// 2D 광원
	LIGHT3D,			// 3D 광원
	ANIMATOR3D,			// 3D Animation

	////////////////////////////////////////////////
	// Rendering 기능 컴포넌트
	MESHRENDER,			// 기본적인 렌더링 컴포넌트
	SPRITE_RENDER,
	FLIPBOOK_RENDER,    // 애니메이션 렌더링 기능
	TILE_RENDER,
	PARTICLE_RENDER,
	SKYBOX,
	DECAL,
	LANDSCAPE,
	////////////////////////////////////////////////

	TRANSFORM,			// 위치, 크기, 회전 등 물체의 위치상태를 관리하는 컴포넌트
	COLLIDER2D,			// 2D 충돌체 
	COLLIDER3D,			// 3D 충돌체

	END,

	SCRIPT,				// GameObject 의 역할, 대본, 스크립트
};
extern const char* COMPONENT_TYPE_STR[(UINT)COMPONENT_TYPE::END];
extern const wchar_t* COMPONENT_TYPE_WSTR[(UINT)COMPONENT_TYPE::END];

enum class DIR
{
	RIGHT,
	UP,
	FRONT,

	END,
};


enum class RS_TYPE
{
	CULL_BACK,	// Default, 뒷면(CCW) 컬링
	CULL_FRONT, // 앞면(CW) 컬링,
	CULL_NONE,  // 컬링하지 않음
	WIRE_FRAME, // 정점을 잇는 선만 그리기

	END
};

enum class DS_TYPE
{
	LESS,				// 작아야 성공, 깊이 기록 O
	LESS_EQUAL,			// 깊이가 작거나 같아야 성공
	
	NO_TEST,			// 깊이검사 수행 X, 깊이 기록 O
	NO_WRITE,			// 깊이검사 수행 O, 깊이 기록 X
	NO_TEST_NO_WRITE,	// 깊이검사 수행 X, 깊이 기록 X

	VOLUME_GREATER,  		
	VOLUME_LESS,		

	VOLUME_CHECK,

	VOLUME_EQUAL,

	END,
};

enum class BS_TYPE
{
	DEFAULT,		// Src 1		Dest 0			Add
	ALPHABLEND,		// Src SrcA		Dest (1-SrcA)	Add
	ONE_ONE,		// Src 1		Dest 1			Add

	DECAL,			// SV_Target0 - AlphaBlend, SV_Target1 = OneOneBlend

	END,
};


enum SCALAR_PARAM
{
	INT_0,
	INT_1,
	INT_2,
	INT_3,

	FLOAT_0,
	FLOAT_1,
	FLOAT_2,
	FLOAT_3,

	VEC2_0,
	VEC2_1,
	VEC2_2,
	VEC2_3,

	VEC4_0,
	VEC4_1,
	VEC4_2,
	VEC4_3,

	MAT_0,
	MAT_1,
};

enum TEX_PARAM
{
	TEX_0,
	TEX_1,
	TEX_2,
	TEX_3,
	TEX_4,
	TEX_5,

	TEXCUBE_0,
	TEXCUBE_1,

	TEX_END,
};

// 렌더링 시점 분류용
enum class RENDER_DOMAIN
{
	DOMAIN_DEFERRED,		// 지연렌더링
	DOMAIN_DECAL,			// 데칼 렌더링

	DOMAIN_LIGHT,			

	DOMAIN_OPAQUE,			// 불투명
	DOMAIN_MASK,			// 불투명 or 투명 (Alpha 1 or 0)
	DOMAIN_TRANSPARENT,		// 투명 ~ 반투명 (Alpha 0 ~ 1)
	DOMAIN_PARTICLE,		// 투명 ~ 반투명 (Alpha 0 ~ 1)
	DOMAIN_POSTPROCESS,		// 최종 장면 후처리

	DOMAIN_DEBUG,			// 렌더링이 다 끝나고 디버깅 단계에 사용되는 재질
	NONE,					// 미정의(렌더링 되지 않음)
};


// Task
enum class TASK_TYPE
{
	CREATE_OBJECT,	// 0: Layer Index, 1 : GameObject Adress
	DELETE_OBJECT,
	ADD_CHILD,
	CHANGE_LEVEL_STATE,
	CHANGE_LEVEL,

	END,
};


// DebugShape
enum class DEBUG_SHAPE
{
	RECT,
	CIRCLE,
	LINE,

	CUBE,
	SPHERE,
};

enum class LEVEL_STATE
{
	PLAY,
	PAUSE,
	STOP,
};

// 광원의 종류
enum class LIGHT_TYPE
{
	DIRECTIONAL, // 방향성        월드 전체에 빛을 주는 객체 - 태양광, 달, 전역광원, 
	POINT,		 // 점광원		 한 점으로부터 퍼져나가는 빛 - 전구, 가로등, 횃불, 모닥불
	SPOT,		 // 스포트라이트  한 방향으로 빛을 모아서 쏘는 광원 - 무대 장치(스포트라이트), 손전등
};

enum class MRT_TYPE
{
	SWAPCHAIN,
	DEFERRED,
	DECAL,
	LIGHT,

	END,
};
