#pragma once
#include "Character.h"
#include "GameConfig.h"
#include "Vector2.h"
#include <graphics.h>
#include <vector>
#include <windows.h>
#include <tchar.h>

class MainMenu {
private:
    std::vector<CharacterData> characters;
    int selectedCharacter;
    bool menuActive;
    Vector2 mousePos;
    
    // 角色卡片定义
    struct CharacterCard {
        int x, y, width, height;
        bool isHovered;
        
        CharacterCard(int _x, int _y, int _w, int _h)
            : x(_x), y(_y), width(_w), height(_h), isHovered(false) {}
        
        bool Contains(float mouseX, float mouseY) const {
            return mouseX >= x && mouseX <= x + width && 
                   mouseY >= y && mouseY <= y + height;
        }
    };
    
    std::vector<CharacterCard> characterCards;

public:
    MainMenu();
    CharacterType ShowMenu();

private:
    void InitializeCharacters();
    void InitializeCharacterCards();
    void RenderMenu();
    void RenderCharacterSelection();
    void RenderCharacterInfo(const CharacterData& character, int x, int y);
    void UpdateMouseHover();
    int HandleInput();
};
