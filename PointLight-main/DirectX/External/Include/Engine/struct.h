#pragma once

struct Vertex
{
	Vec3	vPos;
	Vec4	vColor;
	Vec2	vUV;

	Vec3	vTangent;
	Vec3    vNormal;
	Vec3	vBinormal;

	// Skinning 용 데이터
	Vec4	vWeights;  // Bone 가중치
	Vec4	vIndices;  // Bone 인덱스
};
typedef Vertex Vtx;


struct tTaskInfo
{
	TASK_TYPE Type;
	DWORD_PTR Param0;
	DWORD_PTR Param1;
	DWORD_PTR Param2;
};

struct tDebugShapeInfo
{
	DEBUG_SHAPE		ShapeType;
	Vec4			Color;
	Vec3			WorldPos;
	Vec3			Scale;
	Vec3			Rotation;
	Matrix			matWorld;
	bool			DepthTest;
	float			Age;
	float			Life; 
};

struct Pixel
{
	BYTE b, g, r, a;
};

struct tRay
{
	Vec3	vStart;
	Vec3	vDir;
};


// ============
// Animation 3D
// ============
struct tFrameTrans
{
	Vec4	vTranslate;
	Vec4	vScale;
	Vec4	qRot;
};

struct tMTKeyFrame
{
	double	dTime;
	int		iFrame;
	Vec3	vTranslate;
	Vec3	vScale;
	Vec4	qRot;
};


struct tMTBone
{
	wstring				strBoneName;
	int					iDepth;
	int					iParentIndx;
	Matrix				matOffset;	// Inverse 행렬( Skin 정점을 -> 기본상태로 되돌림)
	Matrix				matBone;
	vector<tMTKeyFrame>	vecKeyFrame;
};

struct tMTAnimClip
{
	wstring			strAnimName;
	int				iStartFrame;
	int				iEndFrame;
	int				iFrameLength;

	double			dStartTime;
	double			dEndTime;
	double			dTimeLength;
	float			fUpdateTime; // 이거 안씀

	FbxTime::EMode	eMode;
};


// ===============
// 광원 관련 구조체
// ===============
struct tLightInfo
{
	LIGHT_TYPE	Type;		// 광원 타입
	Vec3		LightColor;	// 빛의 색상
	Vec3		Ambient;	// 빛으로 인해서 발생하는 최소한의 빛(환경광)
	Vec3		WorldPos;	// 광원의 위치
	Vec3		Direction;	// 광원의 빛이 향하는 방향
	float		Radius;		// 광원의 영향 범위
	float		Angle;		// 광원을 발사하는 각도범위
};


// =================
// 파티클 관련 구조체
// =================
struct tParticle
{
	Vec3	WorldPos;
	Vec3	WorldScale;
	Vec4	Color;

	Vec3	Velocity;

	float	Mass;
	float	Age;			// 파티클 나이(생성 이후 시간)
	float	Life;			// 파티클 수명(최대 유지시간)
	float	NormalizedAge;  // 파티클의 나이를 수명 대비 정규화한 값(Age/Life)

	int		Active;			// 활성화 상태
};







// ========================
// 상수버퍼에 대응하는 구조체
// ========================
struct tTransform
{
	Matrix	matWorld;
	Matrix	matView;
	Matrix	matProj;

	Matrix	matWorldInv;
	Matrix	matViewInv;
	Matrix	matProjInv;

	Matrix  matWV;
	Matrix  matWVP;
};
extern tTransform g_Trans; // 전방선언


struct tMtrlConst
{
	Vec4	DiffCoef;
	Vec4	SpecCoef;
	Vec4	AmbCoef;
	Vec4	EmissCoef;

	int		iArr[4];
	float	fArr[4];
	Vec2	v2Arr[4];
	Vec4	v4Arr[4];
	Matrix  matArr[2];

	int		bTex[TEX_END];	

	// 3D Animation 정보
	int		arrAnimData[4];
	Matrix	ViewProj;
};


struct tGlobalData
{
	Vec2  RenderResolution;
	UINT  Light2DCount;
	UINT  Light3DCount;
	float DeltaTime;
	float Time;
	float EngineDT;
	float EngineTime;
};
extern tGlobalData g_Global;

