#pragma once
#include "Engine\Entity.h"
#include "ImGui/imgui.h"

class EditorUI :
    public Entity
{
private:
    string                  m_UIName;
    string                  m_SubName;
    bool                    m_Active;

    EditorUI*               m_ParentUI;
    vector<Ptr<EditorUI>>   m_vecChildUI;

    Vec2                    m_ChildSize;    // 자식UI 타입일 경우, 부모내에서 차지하는 영역을 나타냄
    bool                    m_UseSeperator; // 자식UI 타입일 경우, 다음 자식과의 구분을 짓는 구분선을 사용한다.

public:
    virtual void Tick();
    virtual void Tick_UI() = 0;

public:
    GET_REF(string, UIName);
    GET_SET_REF(string, SubName);

    bool IsActive() { return m_Active; }
    void SetActive(bool _Active) 
    { 
        if (m_Active == _Active)
            return;

        m_Active = _Active; 

        if (m_Active)
            Activate();
        else
            Deactivate();
    }

    void AddChildUI(EditorUI* _ChildUI);

    EditorUI* GetParentUI() { return m_ParentUI; }
    const vector<Ptr<EditorUI>>& GetChiildUI() { return m_vecChildUI; }

    Vec2 GetChidlSize() { return m_ChildSize; }
    void SetChildSize(Vec2 _Size) { m_ChildSize = _Size; }

    void UseSeperator(bool _Use) { m_UseSeperator = _Use; }

protected:
    virtual void Activate() {};
    virtual void Deactivate() {};

    string GetString(const wstring& _WStr) { return string(_WStr.begin(), _WStr.end()); }
    wstring GetWString(const string& _Str) { return wstring(_Str.begin(), _Str.end()); }

    CLONE_DISABLE(EditorUI);
public:
    EditorUI(const string& _UIName);
    virtual ~EditorUI();
};


// Delegate 맴버함수 타입 재정의
typedef void(EditorUI::* DELEGATE_0)(void);
typedef void(EditorUI::* DELEGATE_1)(DWORD_PTR);
typedef void(EditorUI::* DELEGATE_2)(DWORD_PTR, DWORD_PTR);