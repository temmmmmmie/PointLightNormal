#include "pch.h"
#include "AssetMgr.h"

#include "assets.h"

void AssetMgr::Init()
{
	CreateEngineMesh();

	CreateEngineGraphicShader();

	CreateEngineMaterial();

	CreateEngineSprite();

	CreateEngineFlipbook();
}

Ptr<AMeshData> AssetMgr::LoadFBX(const wstring& _strPath)
{
	wstring strFileName = path(_strPath).stem();

	wstring strName = L"MeshData\\";
	strName += strFileName + L".mdat";

	Ptr<AMeshData> pMeshData = Load<AMeshData>(strName, strName);

	if (nullptr != pMeshData)
		return pMeshData;

	pMeshData = AMeshData::LoadFromFBX(_strPath);
	pMeshData->SetKey(strName);
	pMeshData->SetRelativePath(strName);

	m_mapAsset[(UINT)ASSET_TYPE::MESHDATA].insert(make_pair(strName, pMeshData.Get()));

	// meshdata 를 실제파일로 저장
	pMeshData->Save(PathMgr::GetInst()->GetContentPath() + strName);

	return pMeshData;
}

void AssetMgr::CreateEngineMesh()
{
	vector<Vtx> vecVtx;
	vector<UINT> vecIdx;
	Vtx v;

	// =========
	// PointMesh
	// =========
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vUV = Vec2(0.f, 0.f);

	UINT i = 0;

	Ptr<AMesh> pMesh = new AMesh(true);
	pMesh->Create(&v, 1, &i, 1);
	AddAsset(L"PointMesh", pMesh.Get());

	// ========
	// RectMesh
	// ========
	// 0 -- 1
	// |  \ |
	// 3 -- 2
	Vtx arrVtx[4] = {};
	arrVtx[0].vPos = Vec3(-0.5f, 0.5f, 0.f);
	arrVtx[0].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrVtx[0].vUV = Vec2(0.f, 0.f);
	
	arrVtx[1].vPos = Vec3(0.5f, 0.5f, 0.f);
	arrVtx[1].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrVtx[1].vUV = Vec2(1.f, 0.f);

	arrVtx[2].vPos = Vec3(0.5f, -0.5f, 0.f);
	arrVtx[2].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrVtx[2].vUV = Vec2(1.f, 1.f);

	arrVtx[3].vPos = Vec3(-0.5f, -0.5f, 0.f);
	arrVtx[3].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrVtx[3].vUV = Vec2(0.f, 1.f);

	for (int i = 0; i < 4; ++i)
	{
		arrVtx[i].vTangent = Vec3(1.f, 0.f, 0.f);
		arrVtx[i].vNormal = Vec3(0.f, 0.f, -1.f);
		arrVtx[i].vBinormal = Vec3(0.f, -1.f, 0.f);
	}

	UINT arrIdx[6] = {};
	arrIdx[0] = 0;
	arrIdx[1] = 1;
	arrIdx[2] = 2;

	arrIdx[3] = 0;
	arrIdx[4] = 2;
	arrIdx[5] = 3;

	pMesh = new AMesh(true);
	pMesh->Create(arrVtx, 4, arrIdx, 6);
	AddAsset(L"RectMesh", pMesh.Get());



	// ==================
	// RectMesh_LineStrip
	// ==================
	vecIdx.clear();
	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);
	vecIdx.push_back(3);
	vecIdx.push_back(0);

	pMesh = new AMesh(true);
	pMesh->Create(arrVtx, 4, vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"RectMesh_LineStrip", pMesh.Get());

	// ==========
	// CircleMesh
	// ==========
	vecVtx.clear();
	vecIdx.clear();

	float Radius = 0.5f;
	UINT Poly = 40;
	float Theta = XM_2PI / (float)Poly;

	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vUV = Vec2(0.5f, 0.5f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	vecVtx.push_back(v);

	for (int i = 0; i < Poly + 1; ++i)
	{
		v.vPos = Vec3(Radius * cosf((float)i * Theta), Radius * sinf((float)i * Theta), 0.f);
		v.vUV = Vec2(v.vPos.x + 0.5f, 1.f - (v.vPos.y + 0.5f));
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		vecVtx.push_back(v);
	}

	for (int i = 0; i < Poly; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	pMesh = new AMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"CircleMesh", pMesh.Get());

	// ====================
	// CircleMesh_LineStrip
	// ====================
	vecIdx.clear();
	for (int i = 0; i < Poly + 1; ++i)
	{
		vecIdx.push_back(i + 1);
	}

	pMesh = new AMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"CircleMesh_LineStrip", pMesh.Get());

	// ========
	// CubeMesh
	// ========
	vecVtx.clear();
	vecIdx.clear();

	// 24 개의 정점이 필요(각 면 마다 방향벡터가 극단적으로 나뉘기 때문)
	Vtx arrCube[24] = {};

	// 윗면
	arrCube[0].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[0].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[0].vUV = Vec2(0.f, 0.f);
	arrCube[0].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[0].vNormal = Vec3(0.f, 1.f, 0.f);
	arrCube[0].vBinormal = Vec3(0.f, 0.f, -1.f);

	arrCube[1].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[1].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[1].vUV = Vec2(1.f, 0.f);
	arrCube[1].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[1].vNormal = Vec3(0.f, 1.f, 0.f);
	arrCube[1].vBinormal = Vec3(0.f, 0.f, -1.f);

	arrCube[2].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[2].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[2].vUV = Vec2(1.f, 1.f);
	arrCube[2].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[2].vNormal = Vec3(0.f, 1.f, 0.f);
	arrCube[2].vBinormal = Vec3(0.f, 0.f, -1.f);

	arrCube[3].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[3].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[3].vUV = Vec2(0.f, 1.f);
	arrCube[3].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[3].vNormal = Vec3(0.f, 1.f, 0.f);
	arrCube[3].vBinormal = Vec3(0.f, 0.f, -1.f);


	// 아랫 면	
	arrCube[4].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[4].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[4].vUV = Vec2(0.f, 0.f);
	arrCube[4].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[4].vNormal = Vec3(0.f, -1.f, 0.f);
	arrCube[4].vBinormal = Vec3(0.f, 0.f, 1.f);

	arrCube[5].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[5].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[5].vUV = Vec2(1.f, 0.f);
	arrCube[5].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[5].vNormal = Vec3(0.f, -1.f, 0.f);
	arrCube[5].vBinormal = Vec3(0.f, 0.f, 1.f);

	arrCube[6].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[6].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[6].vUV = Vec2(1.f, 1.f);
	arrCube[6].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[6].vNormal = Vec3(0.f, -1.f, 0.f);
	arrCube[6].vBinormal = Vec3(0.f, 0.f, 1.f);

	arrCube[7].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[7].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[7].vUV = Vec2(0.f, 1.f);
	arrCube[7].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[7].vNormal = Vec3(0.f, -1.f, 0.f);
	arrCube[7].vBinormal = Vec3(0.f, 0.f, 1.f);

	// 왼쪽 면
	arrCube[8].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[8].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[8].vUV = Vec2(0.f, 0.f);
	arrCube[8].vTangent = Vec3(0.f, 0.f, -1.f);
	arrCube[8].vNormal = Vec3(-1.f, 0.f, 0.f);
	arrCube[8].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[9].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[9].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[9].vUV = Vec2(1.f, 0.f);
	arrCube[9].vTangent = Vec3(0.f, 0.f, -1.f);
	arrCube[9].vNormal = Vec3(-1.f, 0.f, 0.f);
	arrCube[9].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[10].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[10].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[10].vUV = Vec2(1.f, 1.f);
	arrCube[10].vTangent = Vec3(0.f, 0.f, -1.f);
	arrCube[10].vNormal = Vec3(-1.f, 0.f, 0.f);
	arrCube[10].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[11].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[11].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[11].vUV = Vec2(0.f, 1.f);
	arrCube[11].vTangent = Vec3(0.f, 0.f, -1.f);
	arrCube[11].vNormal = Vec3(-1.f, 0.f, 0.f);
	arrCube[11].vBinormal = Vec3(0.f, -1.f, 0.f);

	// 오른쪽 면
	arrCube[12].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[12].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[12].vUV = Vec2(0.f, 0.f);
	arrCube[12].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[12].vNormal = Vec3(1.f, 0.f, 0.f);
	arrCube[12].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[13].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[13].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[13].vUV = Vec2(1.f, 0.f);
	arrCube[13].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[13].vNormal = Vec3(1.f, 0.f, 0.f);
	arrCube[13].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[14].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[14].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[14].vUV = Vec2(1.f, 1.f);
	arrCube[14].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[14].vNormal = Vec3(1.f, 0.f, 0.f);
	arrCube[14].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[15].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[15].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[15].vUV = Vec2(0.f, 1.f);
	arrCube[15].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[15].vNormal = Vec3(1.f, 0.f, 0.f);
	arrCube[15].vBinormal = Vec3(0.f, -1.f, 0.f);

	// 뒷 면
	arrCube[16].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[16].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[16].vUV = Vec2(0.f, 0.f);
	arrCube[16].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[16].vNormal = Vec3(0.f, 0.f, 1.f);
	arrCube[16].vBinormal = Vec3(0.f, -1.f, 1.f);

	arrCube[17].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[17].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[17].vUV = Vec2(1.f, 0.f);
	arrCube[17].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[17].vNormal = Vec3(0.f, 0.f, 1.f);
	arrCube[17].vBinormal = Vec3(0.f, -1.f, 1.f);

	arrCube[18].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[18].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[18].vUV = Vec2(1.f, 1.f);
	arrCube[18].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[18].vNormal = Vec3(0.f, 0.f, 1.f);
	arrCube[18].vBinormal = Vec3(0.f, -1.f, 1.f);

	arrCube[19].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[19].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[19].vUV = Vec2(0.f, 1.f);
	arrCube[19].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[19].vNormal = Vec3(0.f, 0.f, 1.f);
	arrCube[19].vBinormal = Vec3(0.f, -1.f, 1.f);

	// 앞 면
	arrCube[20].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[20].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[20].vUV = Vec2(0.f, 0.f);
	arrCube[20].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[20].vNormal = Vec3(0.f, 0.f, -1.f);
	arrCube[20].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[21].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[21].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[21].vUV = Vec2(1.f, 0.f);
	arrCube[21].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[21].vNormal = Vec3(0.f, 0.f, -1.f);
	arrCube[21].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[22].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[22].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[22].vUV = Vec2(1.f, 1.f);
	arrCube[22].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[22].vNormal = Vec3(0.f, 0.f, -1.f);
	arrCube[22].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[23].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[23].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[23].vUV = Vec2(0.f, 1.f);
	arrCube[23].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[23].vNormal = Vec3(0.f, 0.f, -1.f);
	arrCube[23].vBinormal = Vec3(0.f, -1.f, 0.f);

	// 인덱스
	for (int i = 0; i < 12; i += 2)
	{
		vecIdx.push_back(i * 2);
		vecIdx.push_back(i * 2 + 1);
		vecIdx.push_back(i * 2 + 2);

		vecIdx.push_back(i * 2);
		vecIdx.push_back(i * 2 + 2);
		vecIdx.push_back(i * 2 + 3);
	}

	pMesh = new AMesh;
	pMesh->Create(arrCube, 24, vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"CubeMesh", pMesh.Get());



	vecIdx.clear();
	vecIdx.push_back(0); vecIdx.push_back(1); vecIdx.push_back(2); vecIdx.push_back(3); vecIdx.push_back(0);
	vecIdx.push_back(7); vecIdx.push_back(4); vecIdx.push_back(3); vecIdx.push_back(2); vecIdx.push_back(5);
	vecIdx.push_back(4); vecIdx.push_back(5); vecIdx.push_back(6); vecIdx.push_back(1); vecIdx.push_back(0);
	vecIdx.push_back(7); vecIdx.push_back(6);

	pMesh = new AMesh;
	pMesh->Create(arrCube, 24, vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"CubeMesh_LineStrip", pMesh.Get());



	// ==========
	// SphereMesh
	// ==========
	{
		vector<Vtx> vecVtx;
		vector<UINT> vecIdx;
		Vtx v;

		float fRadius = 0.5f;

		// Top
		v.vPos = Vec3(0.f, fRadius, 0.f);
		v.vUV = Vec2(0.5f, 0.f);
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		v.vNormal = v.vPos;
		v.vNormal.Normalize();
		v.vTangent = Vec3(1.f, 0.f, 0.f);
		v.vBinormal = Vec3(0.f, 0.f, -1.f);
		vecVtx.push_back(v);

		// Body
		UINT iStackCount = 50; // 가로 분할 개수
		UINT iSliceCount = 50; // 세로 분할 개수

		float fStackAngle = XM_PI / iStackCount;
		float fSliceAngle = XM_2PI / iSliceCount;

		float fUVXStep = 1.f / (float)iSliceCount;
		float fUVYStep = 1.f / (float)iStackCount;

		for (UINT i = 1; i < iStackCount; ++i)
		{
			float phi = i * fStackAngle;

			for (UINT j = 0; j <= iSliceCount; ++j)
			{
				float theta = j * fSliceAngle;

				v.vPos = Vec3(fRadius * sinf(i * fStackAngle) * cosf(j * fSliceAngle)
					, fRadius * cosf(i * fStackAngle)
					, fRadius * sinf(i * fStackAngle) * sinf(j * fSliceAngle));

				v.vUV = Vec2(fUVXStep * j, fUVYStep * i);
				v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
				v.vNormal = v.vPos;
				v.vNormal.Normalize();

				v.vTangent.x = -fRadius * sinf(phi) * sinf(theta);
				v.vTangent.y = 0.f;
				v.vTangent.z = fRadius * sinf(phi) * cosf(theta);
				v.vTangent.Normalize();

				v.vNormal.Cross(v.vTangent, v.vBinormal);
				v.vBinormal.Normalize();

				vecVtx.push_back(v);
			}
		}

		// Bottom
		v.vPos = Vec3(0.f, -fRadius, 0.f);
		v.vUV = Vec2(0.5f, 1.f);
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		v.vNormal = v.vPos;
		v.vNormal.Normalize();

		v.vTangent = Vec3(1.f, 0.f, 0.f);
		v.vBinormal = Vec3(0.f, 0.f, -1.f);
		vecVtx.push_back(v);

		// 인덱스
		// 북극점
		for (UINT i = 0; i < iSliceCount; ++i)
		{
			vecIdx.push_back(0);
			vecIdx.push_back(i + 2);
			vecIdx.push_back(i + 1);
		}

		// 몸통
		for (UINT i = 0; i < iStackCount - 2; ++i)
		{
			for (UINT j = 0; j < iSliceCount; ++j)
			{
				// + 
				// | \
				// +--+
				vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
				vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
				vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j)+1);

				// +--+
				//  \ |
				//    +
				vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
				vecIdx.push_back((iSliceCount + 1) * (i)+(j + 1) + 1);
				vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
			}
		}

		// 남극점
		UINT iBottomIdx = (UINT)vecVtx.size() - 1;
		for (UINT i = 0; i < iSliceCount; ++i)
		{
			vecIdx.push_back(iBottomIdx);
			vecIdx.push_back(iBottomIdx - (i + 2));
			vecIdx.push_back(iBottomIdx - (i + 1));
		}

		pMesh = new AMesh;
		pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
		AddAsset(L"SphereMesh", pMesh.Get());
		vecVtx.clear();
		vecIdx.clear();
	}
}

void AssetMgr::CreateEngineGraphicShader()
{
	// ===========
	// Std2DShader
	// ===========
	Ptr<AGraphicShader> pShader = new AGraphicShader(true);
	pShader->CreateVertexShader(L"HLSL//std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(L"HLSL//std2d.fx", "PS_Std2D");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::DEFAULT);

	// ShaderParam 설정
	pShader->AddShaderParam(SHADER_PARAM::INT, 0, L"TestParam", false, 1);
	pShader->AddShaderParam(SHADER_PARAM::TEXTURE, 0, L"OutputColor", true, 0);

	AddAsset(L"Std2DShader", pShader.Get());

	// ================
	// Std2D_AlphaBlend
	// ================
	pShader = new AGraphicShader(true);
	pShader->CreateVertexShader(L"HLSL//std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(L"HLSL//std2d.fx", "PS_Std2D_AlphaBlend");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	AddAsset(L"Std2D_AlphaBlend", pShader.Get());

	// ===================
	// Monochrome(흑백효과)
	// ===================
	pShader = new AGraphicShader(true);
	pShader->CreateVertexShader(L"HLSL//std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(L"HLSL//std2d.fx", "PS_Monochrome");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	AddAsset(L"MonochromeShader", pShader.Get());

	// =================================
	// Monochrome(흑백효과) - PostProcess
	// =================================
	pShader = new AGraphicShader(true);
	pShader->CreateVertexShader(L"HLSL//postprocess.fx", "VS_Monochrome");
	pShader->CreatePixelShader(L"HLSL//postprocess.fx", "PS_Monochrome");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	AddAsset(L"MonochromePPShader", pShader.Get());

	// ===========
	// DebugShader
	// ===========
	pShader = new AGraphicShader(true);
	pShader->CreateVertexShader(L"HLSL//debug.fx", "VS_Debug");
	pShader->CreatePixelShader(L"HLSL//debug.fx", "PS_Debug");
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	AddAsset(L"DebugShader", pShader.Get());

	// ===========
	// Std3DShader
	// ===========
	pShader = new AGraphicShader(true);
	pShader->CreateVertexShader(L"HLSL//std3d.fx", "VS_Std3D");
	pShader->CreatePixelShader(L"HLSL//std3d.fx", "PS_Std3D");
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetBSType(BS_TYPE::DEFAULT);

	pShader->AddShaderParam(SHADER_PARAM::TEXTURE, 0, L"Albedo", 0, 0);
	pShader->AddShaderParam(SHADER_PARAM::TEXTURE, 1, L"Normal", 0, 0);

	AddAsset(L"Std3DShader", pShader.Get());

	// ====================
	// Std3D_DeferredShader
	// ====================
	pShader = new AGraphicShader(true);
	pShader->CreateVertexShader(L"HLSL//std3d_deferred.fx", "VS_Std3D_Deferred");
	pShader->CreatePixelShader(L"HLSL//std3d_deferred.fx", "PS_Std3D_Deferred");
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetBSType(BS_TYPE::DEFAULT);

	pShader->AddShaderParam(SHADER_PARAM::TEXTURE, 0, L"Albedo", 0, 0);
	pShader->AddShaderParam(SHADER_PARAM::TEXTURE, 1, L"Normal", 0, 0);

	AddAsset(L"Std3D_DeferredShader", pShader.Get());

	// ===============
	// VolueMeshShader
	// ===============
	pShader = new AGraphicShader(true);
	pShader->CreateVertexShader(L"HLSL//volumemesh.fx", "VS_VolumeMesh");
		
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::VOLUME_CHECK);

	AddAsset(L"VolumeMeshShader", pShader.Get());


	// ===============
	// ShadowMapShader
	// ===============
	pShader = new AGraphicShader(true);
	pShader->CreateVertexShader(L"HLSL//shadowmap.fx", "VS_ShadowMap");
	pShader->CreatePixelShader(L"HLSL//shadowmap.fx", "PS_ShadowMap");

	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::LESS);

	AddAsset(L"ShadowMapShader", pShader.Get());

	// ===============
	// ShadowCubeMapShader
	// ===============
	pShader = new AGraphicShader(true);
	pShader->CreateVertexShader(L"HLSL//ShadowCubeMap.fx", "VS_ShadowCubeMap");
	pShader->CreatePixelShader(L"HLSL//ShadowCubeMap.fx", "PS_ShadowCubeMap");

	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);

	AddAsset(L"ShadowCubeMapShader", pShader.Get());
}

void AssetMgr::CreateEngineMaterial()
{
	// =========
	// Std2DMtrl
	// =========
	Ptr<AMaterial> pMtrl = new AMaterial(true);
	pMtrl->SetShader((AGraphicShader*)Find(ASSET_TYPE::GRAPHICSHADER, L"Std2DShader").Get());
	pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_MASK);
	AddAsset(L"Std2DMtrl", pMtrl.Get());

	// ================
	// Std2D_AlphaBlend
	// ================
	pMtrl = new AMaterial(true);
	pMtrl->SetShader((AGraphicShader*)Find(ASSET_TYPE::GRAPHICSHADER, L"Std2D_AlphaBlend").Get());
	pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_TRANSPARENT);
	AddAsset(L"Std2D_AlphaBlendMtrl", pMtrl.Get());

	// ==============
	// MonochromeMtrl
	// ==============
	pMtrl = new AMaterial(true);
	pMtrl->SetShader((AGraphicShader*)Find(ASSET_TYPE::GRAPHICSHADER, L"MonochormeShader").Get());
	pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_OPAQUE);
	AddAsset(L"MonochromeMtrl", pMtrl.Get());

	// ================
	// MonochromePPMtrl
	// ================
	pMtrl = new AMaterial(true);
	pMtrl->SetShader((AGraphicShader*)Find(ASSET_TYPE::GRAPHICSHADER, L"MonochromePPShader").Get());
	pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_POSTPROCESS);
	AddAsset(L"MonochromePPMtrl", pMtrl.Get());

	// ==============
	// DebugShapeMtrl
	// ==============
	pMtrl = new AMaterial(true);
	pMtrl->SetShader((AGraphicShader*)Find(ASSET_TYPE::GRAPHICSHADER, L"DebugShader").Get());
	pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_DEBUG);
	AddAsset(L"DebugShapeMtrl", pMtrl.Get());

	// =========
	// Std3DMtrl
	// =========
	pMtrl = new AMaterial(true);
	pMtrl->SetShader((AGraphicShader*)Find(ASSET_TYPE::GRAPHICSHADER, L"Std3DShader").Get());
	pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_OPAQUE);
	AddAsset(L"Std3DMtrl", pMtrl.Get());

	// ==================
	// Std3D_DeferredMtrl
	// ==================
	pMtrl = new AMaterial(true);
	pMtrl->SetShader((AGraphicShader*)Find(ASSET_TYPE::GRAPHICSHADER, L"Std3D_DeferredShader").Get());
	pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_DEFERRED);
	AddAsset(L"Std3D_DeferredMtrl", pMtrl.Get());	

	// ==============
	// VolumeMeshMtrl
	// ==============
	pMtrl = new AMaterial(true);
	pMtrl->SetShader((AGraphicShader*)Find(ASSET_TYPE::GRAPHICSHADER, L"VolumeMeshShader").Get());
	pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_DEBUG);
	AddAsset(L"VolumeMeshMtrl", pMtrl.Get());

	// ==============
	// ShadowMapMtrl
	// ==============
	pMtrl = new AMaterial(true);
	pMtrl->SetShader((AGraphicShader*)Find(ASSET_TYPE::GRAPHICSHADER, L"ShadowMapShader").Get());
	pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_DEBUG);
	AddAsset(L"ShadowMapMtrl", pMtrl.Get());	

	// ==============
	// ShadowCubeMapMtrl
	// ==============
	pMtrl = new AMaterial(true);
	pMtrl->SetShader((AGraphicShader*)Find(ASSET_TYPE::GRAPHICSHADER, L"ShadowCubeMapShader").Get());
	pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_DEBUG);
	AddAsset(L"ShadowCubeMapMtrl", pMtrl.Get());

}


void AssetMgr::CreateEngineSprite()
{
	Ptr<ATexture> pAtlas = Load<ATexture>(L"ExplosionAtlas", L"Texture//Ex.png");

	int i = 0;
	for (UINT Row = 0; Row < 9; ++Row)
	{
		for (UINT Col = 0; Col < 10; ++Col, ++i)
		{
			wchar_t szBuffer[256] = {};
			swprintf_s(szBuffer, 256, L"Explosion_%02d", i);

			Ptr<ASprite> pSprite = new ASprite;
			pSprite->SetAtlas(pAtlas);
			pSprite->SetLeftTop(Vec2(Col * 320.f, Row * 240.f));
			pSprite->SetSlice(Vec2(320.f, 240.f));
			pSprite->SetOffset(Vec2(-60.f, 0.f));
			AddAsset(szBuffer, pSprite.Get());
		}
	}

	pAtlas = Load<ATexture>(L"TileAtlas", L"Texture//TILE.bmp");

	i = 0;
	for (UINT Row = 0; Row < 6; ++Row)
	{
		for (UINT Col = 0; Col < 8; ++Col, ++i)
		{
			wchar_t szBuffer[256] = {};
			swprintf_s(szBuffer, 256, L"TILE_%02d", i);

			Ptr<ASprite> pSprite = new ASprite;
			pSprite->SetAtlas(pAtlas);
			pSprite->SetLeftTop(Vec2(Col * 64.f, Row * 64.f));
			pSprite->SetSlice(Vec2(64.f, 64.f));
			AddAsset(szBuffer, pSprite.Get());
		}
	}
}

void AssetMgr::CreateEngineFlipbook()
{
	Ptr<AFlipbook> pFlipbook = new AFlipbook;

	int i = 0;
	for (UINT Row = 0; Row < 9; ++Row)
	{
		for (UINT Col = 0; Col < 10; ++Col, ++i)
		{
			wchar_t szBuffer[256] = {};
			swprintf_s(szBuffer, 256, L"Explosion_%02d", i);

			Ptr<ASprite> pSprite = Find<ASprite>(szBuffer);
			pFlipbook->AddSprite(pSprite);
		}
	}

	pFlipbook->SetPanelSize(Vec2(640.f, 480.f));

	AddAsset(L"Explosion", pFlipbook.Get());
}
