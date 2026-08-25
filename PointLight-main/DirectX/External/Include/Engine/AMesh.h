#pragma once
#include "Asset.h"

#include "FBXLoader.h"
#include "StructuredBuffer.h"

struct tIndexInfo
{
    ComPtr<ID3D11Buffer>    IB;
    D3D11_BUFFER_DESC       IBDesc;
    UINT				    IdxCount;
    UINT*                   IdxSysMem;
};

class AMesh :
    public Asset
{
private:
    ComPtr<ID3D11Buffer>    m_VB;
    D3D11_BUFFER_DESC       m_VBDesc;
    UINT                    m_VtxCount;
    Vtx*                    m_VtxSys;

    // 하나의 버텍스버퍼에 여러개의 인덱스버퍼가 연결
    vector<tIndexInfo>	    m_vecIdxInfo;

    // Animation3D 정보
    vector<tMTAnimClip>	    m_vecAnimClip;
    vector<tMTBone>		    m_vecBones;

    Ptr<StructuredBuffer>   m_BoneFrameData;   // 전체 본 프레임 정보(크기, 이동, 회전) (프레임 개수만큼)
    Ptr<StructuredBuffer>   m_BoneInverse;	   // 각 뼈의 offset 행렬(각 뼈의 위치를 되돌리는 행렬) (1행 짜리)


public:
    static AMesh* CreateFromContainer(FBXLoader& _loader);
    int Create(Vtx* _VtxSysMem, UINT _VtxCount, UINT* _IdxSysMem, UINT _IdxCount);
    void Binding(UINT _SubSet);
    void Render(UINT _SubSet);
    void Render_Particle(int _Count);

public:
    Vtx* GetVtxSysMem() { return m_VtxSys; }
    UINT GetVtxCount() { return m_VtxCount; }

    const UINT* GetIdxSysMem(UINT _SubsetIdx) const { return m_vecIdxInfo[_SubsetIdx].IdxSysMem; }
    UINT GetSubsetCount() { return (UINT)m_vecIdxInfo.size(); }

    const vector<tMTBone>& GetBones() { return m_vecBones; }
    UINT GetBoneCount() { return (UINT)m_vecBones.size(); }
    const vector<tMTAnimClip>& GetAnimClip() { return m_vecAnimClip; }
    bool IsAnimMesh() { return !m_vecAnimClip.empty(); }
    Ptr<StructuredBuffer> GetBoneFrameDataBuffer() { return m_BoneFrameData; } // 전체 본 프레임 정보
    Ptr<StructuredBuffer> GetBoneInverseBuffer() { return  m_BoneInverse; }	   // 각 Bone 의 Inverse 행렬

public:
    virtual int Save(const wstring& _FilePath) override;
    virtual int Load(const wstring& _FilePath) override;

    CLONE_DISABLE(AMesh);
public:
    AMesh(bool _EngineAsset = false);
    virtual ~AMesh();
};

