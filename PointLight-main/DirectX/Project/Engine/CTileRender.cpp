#include "pch.h"
#include "CTileRender.h"

#include "AssetMgr.h"
#include "CTransform.h"

CTileRender::CTileRender()
	: CRenderComponent(COMPONENT_TYPE::TILE_RENDER)
	, m_Col(1)
	, m_Row(1)
	, m_TileSize(1.f, 1.f)	
{
	SetMesh(FIND(AMesh, L"RectMesh"));
	
	CreateMaterial();

	// 타일정보를 전달받을 버퍼 생성
	m_Buffer = new StructuredBuffer;
}

CTileRender::CTileRender(const CTileRender& _Origin)
	: CRenderComponent(_Origin)
	, m_Col(_Origin.m_Col)
	, m_Row(_Origin.m_Row)
	, m_TileSize(_Origin.m_TileSize)
	, m_Buffer(nullptr)
	, m_Atlas(_Origin.m_Atlas)
	, m_vecTileInfo(_Origin.m_vecTileInfo)
{
	m_Buffer = new StructuredBuffer;
}

CTileRender::~CTileRender()
{
}


void CTileRender::FinalTick()
{
	// 타일맵의 크기는 타일 1개 사이즈 x 타일 행렬 수
	Transform()->SetRelativeScale(Vec3(m_TileSize.x, m_TileSize.y, 1.f) * Vec3((float)m_Col, (float)m_Row, 1.f));
	
	DrawDebugRect(Transform()->GetmatWorld(), Vec4(0.f, 1.f, 0.f, 1.f), false);
}

void CTileRender::Render()
{
	GetMaterial(0)->SetScalar(INT_0, m_Col);
	GetMaterial(0)->SetScalar(INT_1, m_Row);			
	GetMaterial(0)->SetTexture(TEX_0, m_Atlas);
	GetMaterial(0)->Binding();

	// 타일 정보를 구조화버퍼에 담아서 바인딩 해준다.
	SetBuffer();

	// 렌더링 시작
	GetMesh()->Render(0);

	// 바인딩했던 버퍼를 다시 클리어해준다.
	m_Buffer->Clear();
}



void CTileRender::CreateMaterial()
{
	wstring MtrlName = L"TileMtrl";
	wstring ShaderName = L"TileShader";

	Ptr<AMaterial> pMtrl = AssetMgr::GetInst()->Find<AMaterial>(MtrlName);

	if (nullptr == pMtrl)
	{
		Ptr<AGraphicShader> pShader = AssetMgr::GetInst()->Find<AGraphicShader>(ShaderName);

		if (nullptr == pShader)
		{
			pShader = new AGraphicShader;
			pShader->SetName(ShaderName);
			pShader->CreateVertexShader(L"HLSL\\tile.fx", "VS_Tile");
			pShader->CreatePixelShader(L"HLSL\\tile.fx", "PS_Tile");
			pShader->SetRSType(RS_TYPE::CULL_NONE);
			pShader->SetBSType(BS_TYPE::DEFAULT);
			pShader->SetDSType(DS_TYPE::LESS);
			AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());
		}

		pMtrl = new AMaterial;
		pMtrl->SetName(MtrlName);
		pMtrl->SetShader(pShader);
		pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_OPAQUE);
		AssetMgr::GetInst()->AddAsset(pMtrl->GetName(), pMtrl.Get());
	}

	SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(MtrlName), 0);
}

void CTileRender::SetBuffer()
{
	if (m_Buffer->GetElementCount() < m_vecTileInfo.size())
	{
		m_Buffer->Create(sizeof(tTileInfo), m_Col * m_Row, SRV_ONLY, true);
	}

	m_Buffer->SetData(m_vecTileInfo.data(), sizeof(tTileInfo) * m_vecTileInfo.size());
	m_Buffer->Binding(18);
}


int CTileRender::SetSprite(UINT _Col, UINT _Row, Ptr<ASprite> _Sprite)
{
	if (_Sprite->GetAtlas() == nullptr || _Sprite->GetAtlas() != m_Atlas)
		return E_FAIL;

	if (m_Col < _Col || m_Row < _Row)
		return E_FAIL;

	int Idx = m_Col * _Row + _Col;

	m_vecTileInfo[Idx].vLeftTop = _Sprite->GetLeftTop();
	m_vecTileInfo[Idx].vSlice = _Sprite->GetSlice();

	return S_OK;
}


void CTileRender::SaveToLevelFile(FILE* _File)
{
	CRenderComponent::SaveToLevelFile(_File);

	fwrite(&m_Col, sizeof(UINT), 1, _File);
	fwrite(&m_Row, sizeof(UINT), 1, _File);
	fwrite(&m_TileSize, sizeof(Vec2), 1, _File);
	SaveAssetRef(m_Atlas.Get(), _File);

	// 타일 개수
	UINT TileCount = (UINT)m_vecTileInfo.size();
	fwrite(&TileCount, sizeof(UINT), 1, _File);
		
	// 각 타일의 정보
	fwrite(m_vecTileInfo.data(), sizeof(tTileInfo), m_vecTileInfo.size(), _File);
}

void CTileRender::LoadFromLevelFile(FILE* _File)
{
	CRenderComponent::LoadFromLevelFile(_File);

	fread(&m_Col, sizeof(UINT), 1, _File);
	fread(&m_Row, sizeof(UINT), 1, _File);
	fread(&m_TileSize, sizeof(Vec2), 1, _File);

	m_Atlas = LoadAssetRef<ATexture>(_File);

	// 타일 개수
	UINT TileCount = 0;
	fread(&TileCount, sizeof(UINT), 1, _File);

	// 각 타일의 정보
	m_vecTileInfo.resize(TileCount);
	fread(m_vecTileInfo.data(), sizeof(tTileInfo), TileCount, _File);
}