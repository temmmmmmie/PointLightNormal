#pragma once
#include "Asset.h"

#include "AMaterial.h"

class AMeshData :
	public Asset
{
private:
	Ptr<AMesh>				m_Mesh;
	vector<Ptr<AMaterial>>	m_vecMtrl;

public:
	Ptr<AMesh> GetMesh() const { return m_Mesh; };
	static AMeshData* LoadFromFBX(const wstring& _RelativePath);

	GameObject* Instantiate();
	virtual int Save(const wstring& _FilePath) override;
	virtual int Load(const wstring& _strFilePath) override;

public:
	CLONE(AMeshData);
	AMeshData(bool _Engine = false);
	virtual ~AMeshData();
};

