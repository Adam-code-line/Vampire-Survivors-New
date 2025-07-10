#pragma once
#include "GameConfig.h"
#include "Vector2.h"
#include <graphics.h>
#include <windows.h>
#include <tchar.h>
#include <vector>
#include <cmath>  // 添加数学函数支持

enum class VictoryChoice {
    RETURN_TO_MENU,
    EXIT_GAME,
    NONE
};

class VictoryScreen {
private:
    bool screenActive;
    int selectedOption;
    Vector2 mousePos;
    int finalScore;
    int finalLevel;
    int enemiesDefeated;
    float survivalTime;
    DWORD animationTime;
    
    // 按钮定义
    struct VictoryButton {
        int x, y, width, height;
        const TCHAR* text;  // 修改为 const TCHAR*
        COLORREF normalColor, hoverColor;
        bool isHovered;
        VictoryChoice choice;
        
        VictoryButton(int _x, int _y, int _w, int _h, const TCHAR* _text, VictoryChoice _choice,
                     COLORREF _normal = RGB(60, 60, 80), COLORREF _hover = RGB(100, 100, 120))
            : x(_x), y(_y), width(_w), height(_h), text(_text), choice(_choice),
              normalColor(_normal), hoverColor(_hover), isHovered(false) {}
        
        bool Contains(float mouseX, float mouseY) const {
            return mouseX >= x && mouseX <= x + width && 
                   mouseY >= y && mouseY <= y + height;
        }
    };
    
    std::vector<VictoryButton> buttons;

public:
    VictoryScreen(int score, int level, int enemies, float time);
    VictoryChoice ShowVictoryScreen();

private:
    void InitializeButtons();
    void RenderVictoryScreen();
    void RenderBackground();
    void RenderTitle();
    void RenderStats();
    void RenderButtons();
    void UpdateMouseHover();
    void DrawVictoryEffects();
    int HandleInput();
};
