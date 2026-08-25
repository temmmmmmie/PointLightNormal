#pragma once

#include "Component.h"


#include "AMesh.h"
#include "AMaterial.h"


struct tMtrlSet
{
	Ptr<AMaterial>  SharedMtrl;    // 공유 메테리얼
	Ptr<AMaterial>  DynamicMtrl;   // 공유 메테리얼의 복사본    
	Ptr<AMaterial>  CurMtrl;       // 현재 사용 할 메테리얼
};

class CRenderComponent
	: public Component
{
private:
	Ptr<AMesh>          m_Mesh;

	//Ptr<AMaterial>		m_SharedMtrl;	
	//Ptr<AMaterial>		m_DynamicMtrl;
	//Ptr<AMaterial>		m_CurMtrl;
	vector<tMtrlSet>    m_vecMtrls; // 메시의 서브셋(인덱스버퍼) 에 각각 대응하는 재질들

	bool				m_FrustumCulling;
	float				m_BoundingBox;

public:
	Ptr<AMesh> GetMesh() { return m_Mesh; }
	void SetMesh(Ptr<AMesh> _Mesh);

	UINT GetMaterialCount() { return (UINT)m_vecMtrls.size(); }

	void SetMaterial(Ptr<AMaterial> _Mtrl, UINT _idx);
	Ptr<AMaterial> GetMaterial(UINT _Idx);
	Ptr<AMaterial> GetSharedMaterial(UINT _Idx);
	Ptr<AMaterial> GetDynamicMaterial(UINT _Idx);

	void SetFrustumCulling(bool _Set) { m_FrustumCulling = _Set; }
	bool IsFrustumCulling() { return m_FrustumCulling; }

	void SetBoundingBox(float _Radius) { m_BoundingBox = _Radius; }
	float GetBoundingBox() { return m_BoundingBox; }

	


public:
	virtual void Render() = 0;
	virtual void Render_ShadowMap(Ptr<AMaterial> _ShadowMapMtrl);
	virtual void SaveToLevelFile(FILE* _File) override;
	virtual void LoadFromLevelFile(FILE* _File) override;
	virtual CRenderComponent* Clone() = 0;public:
	CRenderComponent(COMPONENT_TYPE _Type);
	CRenderComponent(const CRenderComponent& _Origin);
	virtual ~CRenderComponent();
};

