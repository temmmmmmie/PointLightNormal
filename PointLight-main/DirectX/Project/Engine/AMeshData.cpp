#include "pch.h"
#include "AMeshData.h"

#include "PathMgr.h"
#include "AssetMgr.h"

#include "GameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"
#include "CAnimator3D.h"

#include "FBXLoader.h"

AMeshData::AMeshData(bool _Engine)
	: Asset(ASSET_TYPE::MESHDATA)
{
}

AMeshData::~AMeshData()
{
}

GameObject* AMeshData::Instantiate()
{
	GameObject* pNewObj = new GameObject;
	pNewObj->AddComponent(new CTransform);
	pNewObj->AddComponent(new CMeshRender);

	pNewObj->MeshRender()->SetMesh(m_Mesh);

	for (UINT i = 0; i < m_vecMtrl.size(); ++i)
	{
		pNewObj->MeshRender()->SetMaterial(m_vecMtrl[i], i);
	}

	// Animation 파트 추가
	if (false == m_Mesh->IsAnimMesh())
		return pNewObj;

	CAnimator3D* pAnimator = new CAnimator3D;
	pNewObj->AddComponent(pAnimator);

	pAnimator->SetBones(m_Mesh->GetBones());
	pAnimator->SetAnimClip(m_Mesh->GetAnimClip());

	return pNewObj;
}

AMeshData* AMeshData::LoadFromFBX(const wstring& _RelativePath)
{
	wstring strFullPath = PathMgr::GetInst()->GetContentPath();
	strFullPath += _RelativePath;

	FBXLoader loader;
	loader.init();
	loader.LoadFbx(strFullPath);

	// 메쉬 가져오기
	Ptr<AMesh> pMesh = nullptr;
	pMesh = AMesh::CreateFromContainer(loader);

	// AssetMgr 에 메쉬 등록
	if (nullptr != pMesh)
	{
		wstring strMeshKey = L"mesh\\";
		strMeshKey += path(strFullPath).stem();
		strMeshKey += L".mesh";

		if (nullptr == AssetMgr::GetInst()->Find<AMesh>(strMeshKey))
		{
			AssetMgr::GetInst()->AddAsset(strMeshKey, pMesh.Get());
		}

		// 메시를 실제 파일로 저장
		pMesh->Save(PathMgr::GetInst()->GetContentPath() + strMeshKey);
	}

	vector<Ptr<AMaterial>> vecMtrl;

	// 메테리얼 가져오기
	for (UINT i = 0; i < loader.GetContainer(0).vecMtrl.size(); ++i)
	{
		// 예외처리 (material 이름이 입력 안되어있을 수도 있다.)
		Ptr<AMaterial> pMtrl = AssetMgr::GetInst()->Find<AMaterial>(loader.GetContainer(0).vecMtrl[i].strMtrlName);

		if (nullptr == pMtrl)
			continue;

		vecMtrl.push_back(pMtrl);
	}

	AMeshData* pMeshData = new AMeshData(true);
	pMeshData->m_Mesh = pMesh;
	pMeshData->m_vecMtrl = vecMtrl;

	return pMeshData;
}


int AMeshData::Save(const wstring& _FilePath)
{
	wstring RelativePath = PathMgr::GetInst()->GetRelativePath(_FilePath);
	SetRelativePath(RelativePath);

	wstring strFilePath = PathMgr::GetInst()->GetContentPath() + RelativePath;

	FILE* pFile = nullptr;
	errno_t err = _wfopen_s(&pFile, strFilePath.c_str(), L"wb");
	assert(pFile);

	// Mesh Key 저장	
	// Mesh Data 저장
	SaveAssetRef(m_Mesh.Get(), pFile);

	// material 정보 저장
	UINT iMtrlCount = (UINT)m_vecMtrl.size();
	fwrite(&iMtrlCount, sizeof(UINT), 1, pFile);

	UINT i = 0;
	wstring strMtrlPath = PathMgr::GetInst()->GetContentPath();
	strMtrlPath += L"Material\\";

	for (; i < iMtrlCount; ++i)
	{
		if (nullptr == m_vecMtrl[i])
			continue;

		// Material 인덱스, Key, Path 저장
		fwrite(&i, sizeof(UINT), 1, pFile);
		SaveAssetRef(m_vecMtrl[i].Get(), pFile);
	}

	i = -1; // 마감 값
	fwrite(&i, sizeof(UINT), 1, pFile);

	fclose(pFile);

	return S_OK;
}

int AMeshData::Load(const wstring& _FilePath)
{
	FILE* pFile = NULL;
	_wfopen_s(&pFile, _FilePath.c_str(), L"rb");

	if (nullptr == pFile)
		return E_FAIL;

	// Mesh Load
	m_Mesh = LoadAssetRef<AMesh>(pFile);
	assert(m_Mesh.Get());

	// material 정보 읽기
	UINT iMtrlCount = 0;
	fread(&iMtrlCount, sizeof(UINT), 1, pFile);

	m_vecMtrl.resize(iMtrlCount);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		UINT idx = -1;
		fread(&idx, 4, 1, pFile);
		if (idx == -1)
			break;

		wstring strKey, strPath;
		m_vecMtrl[i] = LoadAssetRef<AMaterial>(pFile);
	}

	fclose(pFile);

	return S_OK;
}