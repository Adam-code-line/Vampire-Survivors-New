#pragma once
#include "Character.h"
#include "GameConfig.h"
#include <graphics.h>
#include <vector>
#include <windows.h>
#include <tchar.h>

class MainMenu {
private:
    std::vector<CharacterData> characters;
    int selectedCharacter;
    bool menuActive;

public:
    MainMenu();
    CharacterType ShowMenu();

private:
    void InitializeCharacters();
    void RenderMenu();
    void RenderCharacterSelection();
    void RenderCharacterInfo(const CharacterData& character, int x, int y);
    int HandleInput();
};
