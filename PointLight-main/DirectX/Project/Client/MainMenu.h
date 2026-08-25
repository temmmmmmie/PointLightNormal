#pragma once

#include "EditorUI.h"

#include <Engine/Level.h>

class MainMenu :
    public EditorUI
{
private:
    Ptr<ALevel>  m_CopyLevel;

public:
    void File();
    void Level();
    void GameObjectMenu();
    void Asset();
    void Editor();
    void RenderTarget();

private:
    wstring GetAssetName(ASSET_TYPE _Type, const wstring& _Name);

public:
    static void SaveLevel(class ALevel* Level, const wstring& _RelativePath);
    static class ALevel* LoadLevel(const wstring& _RelativePath);

    static void SaveGameObject(class GameObject* _Object, FILE* _File);
    static class GameObject* LoadGameObject(FILE* _File);


public:
    virtual void Tick() override;
    virtual void Tick_UI() override;

public:
    MainMenu();
    virtual ~MainMenu();
};

