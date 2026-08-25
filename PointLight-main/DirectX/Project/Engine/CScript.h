#pragma once
#include "Component.h"

#include "TimeMgr.h"
#include "KeyMgr.h"
#include "Device.h"
#include "PathMgr.h"
#include "AssetMgr.h"


enum class SCRIPT_PARAM
{
    INT,
    FLOAT,
    VEC2,
    VEC4,
    MATRIX,

    TEXTURE,
    PREFAB,
    MATERIAL,
};

struct tScriptParam
{
    SCRIPT_PARAM    Param;
    void*           Data;    // 실제 데이터의 시작 주소
    wstring         Desc;
    bool            IsInput; // UI 표시방법 (단순 입력, 드래그)
    float           Step;    // UI 에서 값의 이동 단위
};


class CScript :
    public Component
{
private:
    const UINT           m_Type;
    vector<tScriptParam> m_vecScriptParam;

public:
    UINT GetScriptType() { return m_Type; }

protected:
    void AddScriptParam(SCRIPT_PARAM _Type, void* _Data, const wstring& _Desc, bool _IsInput = true, float _Step = 1.f) { m_vecScriptParam.push_back(tScriptParam{ _Type , _Data, _Desc, _IsInput, _Step}); }

public:
    const vector<tScriptParam>& GetScriptParam() { return m_vecScriptParam; }

public:
    virtual void Tick() = 0;
    virtual void FinalTick() final;
   
    virtual CScript* Clone() = 0;

public:
    class CRenderComponent* GetRenderComponent();

public:
    CScript(UINT _Type);
    virtual ~CScript();
};

