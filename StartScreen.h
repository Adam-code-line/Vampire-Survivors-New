#pragma once
#include "GameConfig.h"
#include "Vector2.h"
#include <graphics.h>
#include <windows.h>
#include <tchar.h>
#include <vector>

class StartScreen {
private:
    bool screenActive;
    int selectedOption;
    bool mouseHover;
    Vector2 mousePos;
    
    // 按钮定义
    struct Button {
        int x, y, width, height;
        LPCTSTR text;
        COLORREF normalColor, hoverColor;
        bool isHovered;
        
        Button(int _x, int _y, int _w, int _h, LPCTSTR _text, 
               COLORREF _normal = RGB(60, 60, 80), COLORREF _hover = RGB(100, 100, 120))
            : x(_x), y(_y), width(_w), height(_h), text(_text), 
              normalColor(_normal), hoverColor(_hover), isHovered(false) {}
        
        bool Contains(float mouseX, float mouseY) const {
            return mouseX >= x && mouseX <= x + width && 
                   mouseY >= y && mouseY <= y + height;
        }
    };
    
    std::vector<Button> buttons;
    DWORD animationTime;

public:
    StartScreen();
    bool ShowStartScreen();

private:
    void InitializeButtons();
    void RenderStartScreen();
    void RenderTitle();
    void RenderBackground();
    void RenderButtons();
    void UpdateAnimation();
    int HandleInput();
    void UpdateMouseHover();
    void ShowHowToPlay();        // 添加这个声明
    void DrawGameDecorations();  // 添加这个声明
};
