#include "StartScreen.h"
#include <conio.h>
#include <cmath>

StartScreen::StartScreen() : screenActive(true), selectedOption(0), mouseHover(false), mousePos(0, 0) {
    animationTime = GetTickCount();
    InitializeButtons();
}

void StartScreen::InitializeButtons() {
    buttons.clear();
    
    int buttonWidth = 250;
    int buttonHeight = 60;
    int centerX = WINDOW_WIDTH / 2 - buttonWidth / 2;
    int startY = WINDOW_HEIGHT / 2 + 50;
    int spacing = 80;
    
    // 开始游戏按钮
    buttons.emplace_back(centerX, startY, buttonWidth, buttonHeight, 
                        _T("START GAME"), RGB(0, 150, 0), RGB(0, 200, 0));
    
    // 游戏说明按钮
    buttons.emplace_back(centerX, startY + spacing, buttonWidth, buttonHeight, 
                        _T("HOW TO PLAY"), RGB(60, 60, 80), RGB(100, 100, 120));
    
    // 退出按钮
    buttons.emplace_back(centerX, startY + spacing * 2, buttonWidth, buttonHeight, 
                        _T("EXIT GAME"), RGB(150, 0, 0), RGB(200, 0, 0));
}

bool StartScreen::ShowStartScreen() {
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
    setbkcolor(RGB(10, 10, 20));
    
    BeginBatchDraw();
    
    bool needsRedraw = true;
    DWORD lastTime = GetTickCount();
    
    while (screenActive) {
        DWORD currentTime = GetTickCount();
        
        int input = HandleInput();
        
        if (input == 1) { // Enter 或鼠标点击
            if (selectedOption == 0) { // 开始游戏
                screenActive = false;
                EndBatchDraw();
                return true; 
            } else if (selectedOption == 1) { // 游戏说明
                ShowHowToPlay();
                needsRedraw = true;
            } else if (selectedOption == 2) { // 退出
                screenActive = false;
                EndBatchDraw();
                return false; 
            }
        }
        else if (input == 2) { // 上箭头
            selectedOption = (selectedOption - 1 + (int)buttons.size()) % (int)buttons.size();
            needsRedraw = true;
        }
        else if (input == 3) { // 下箭头
            selectedOption = (selectedOption + 1) % (int)buttons.size();
            needsRedraw = true;
        }
        else if (input == 4) { // ESC 退出
            screenActive = false;
            EndBatchDraw();
            return false;
        }
        
        UpdateMouseHover();
        
        if (needsRedraw || (currentTime - lastTime >= 50)) { 
            UpdateAnimation();
            RenderStartScreen();
            FlushBatchDraw();
            needsRedraw = false;
            lastTime = currentTime;
        }
        
        Sleep(10);
    }
    
    EndBatchDraw();
    return false;
}

void StartScreen::ShowHowToPlay() {
    cleardevice();
    
    // 背景
    setfillcolor(RGB(20, 20, 40));
    solidrectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // 标题
    settextcolor(RGB(255, 215, 0));
    settextstyle(36, 0, _T("Arial"));
    int titleWidth = textwidth(_T("HOW TO PLAY"));
    outtextxy((WINDOW_WIDTH - titleWidth) / 2, 50, _T("HOW TO PLAY"));
    
    // 游戏说明内容
    settextcolor(WHITE);
    settextstyle(18, 0, _T("Arial"));
    
    int startY = 120;
    int lineHeight = 25;
    int currentY = startY;
    
    // 修复：移除换行符，分别定义每行文本
    const TCHAR* instructions[] = {
        _T("CONTROLS:"),
        _T("  Use WASD keys to move your character"),
        _T("  Press Q to use your character's special skill"),
        _T("  Character attacks automatically"),
        _T(""),
        _T("GAMEPLAY:"),
        _T("  Defeat enemies to gain experience and level up"),
        _T("  Collect experience gems dropped by enemies"),
        _T("  Collect power-up items for temporary boosts"),
        _T("  Survive as long as possible!"),
        _T(""),
        _T("CHARACTER TYPES:"),
        _T("  Warrior - High health, berserker rage skill"),
        _T("  Mage - Magic attacks, fireball skill"),
        _T("  Archer - Fast attacks, multi-shot skill"),
        _T("  Assassin - High speed, shadow strike skill"),
        _T(""),
        _T("POWER-UP ITEMS:"),
        _T("  Speed Boost - Increases movement speed"),
        _T("  Damage Up - Increases attack damage"),
        _T("  Fire Rate Up - Increases attack speed"),
        _T("  Shield - Temporary damage protection"),
        _T("  EXP Boost - Increases experience gain"),
        _T("  Multi Shot - Fires multiple projectiles")
    };
    
    int instructionCount = sizeof(instructions) / sizeof(instructions[0]);
    
    for (int i = 0; i < instructionCount; i++) {
        if (_tcscmp(instructions[i], _T("")) == 0) {
            currentY += lineHeight / 2;
            continue;
        }
        
        // 检查是否是标题行
        if (_tcsstr(instructions[i], _T("CONTROLS:")) != nullptr || 
            _tcsstr(instructions[i], _T("GAMEPLAY:")) != nullptr ||
            _tcsstr(instructions[i], _T("CHARACTER TYPES:")) != nullptr || 
            _tcsstr(instructions[i], _T("POWER-UP ITEMS:")) != nullptr) {
            settextcolor(RGB(255, 215, 0));
            settextstyle(20, 0, _T("Arial"));
        } else {
            settextcolor(RGB(200, 200, 200));
            settextstyle(16, 0, _T("Arial"));
        }
        
        outtextxy(50, currentY, instructions[i]);
        currentY += lineHeight;
    }
    
    // 返回提示
    settextcolor(RGB(255, 255, 100));
    settextstyle(18, 0, _T("Arial"));
    outtextxy(50, WINDOW_HEIGHT - 50, _T("Press any key to return to main menu"));
    
    FlushBatchDraw();
    _getch();
}

void StartScreen::UpdateMouseHover() {
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(GetHWnd(), &cursorPos);
    
    mousePos = Vector2((float)cursorPos.x, (float)cursorPos.y);
    
    for (size_t i = 0; i < buttons.size(); i++) {
        buttons[i].isHovered = buttons[i].Contains(mousePos.x, mousePos.y);
        
        if (buttons[i].isHovered) {
            selectedOption = (int)i;
        }
    }
}

void StartScreen::UpdateAnimation() {
    animationTime = GetTickCount();
}

void StartScreen::RenderStartScreen() {
    cleardevice();
    
    RenderBackground();
    RenderTitle();
    RenderButtons();
    
    // 版本信息
    settextcolor(RGB(100, 100, 100));
    settextstyle(14, 0, _T("Arial"));
    outtextxy(WINDOW_WIDTH - 120, WINDOW_HEIGHT - 30, _T("Version 1.0.0"));
    
    // 控制提示
    settextcolor(RGB(150, 150, 150));
    settextstyle(16, 0, _T("Arial"));
    outtextxy(30, WINDOW_HEIGHT - 80, _T("Use mouse or arrow keys + ENTER to select"));
    outtextxy(30, WINDOW_HEIGHT - 60, _T("Press ESC to exit"));
}

void StartScreen::RenderBackground() {
    // 绘制渐变背景
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        float ratio = (float)y / WINDOW_HEIGHT;
        int r = (int)(10 + ratio * 30);
        int g = (int)(10 + ratio * 20);
        int b = (int)(20 + ratio * 60);
        
        setlinecolor(RGB(r, g, b));
        line(0, y, WINDOW_WIDTH, y);
    }
    
    // 绘制动态星星
    srand(12345); // 固定种子确保星星位置一致
    DWORD time = GetTickCount();
    
    for (int i = 0; i < 80; i++) {
        float x = (float)(rand() % WINDOW_WIDTH);
        float y = (float)(rand() % WINDOW_HEIGHT);
        
        // 添加闪烁效果
        float twinkle = (float)sin((time * 0.002f) + i) * 0.5f + 0.5f;
        int brightness = (int)(100 + 155 * twinkle);
        
        setfillcolor(RGB(brightness, brightness, brightness));
        solidcircle((int)x, (int)y, 1 + (i % 2));
    }
    
    // 绘制游戏元素装饰
    DrawGameDecorations();
}

void StartScreen::DrawGameDecorations() {
    DWORD time = GetTickCount();
    float angle = time * 0.001f;
    
    // 绘制旋转的装饰圆环
    Vector2 center(WINDOW_WIDTH * 0.8f, WINDOW_HEIGHT * 0.3f);
    float radius = 80.0f;
    
    setlinecolor(RGB(100, 100, 255));
    setlinestyle(PS_SOLID, 2);
    
    for (int i = 0; i < 8; i++) {
        float a = angle + i * 3.14159f / 4;
        Vector2 pos = center + Vector2((float)cos(a) * radius, (float)sin(a) * radius);
        solidcircle((int)pos.x, (int)pos.y, 3);
    }
    
    // 绘制左侧装饰
    Vector2 leftCenter(WINDOW_WIDTH * 0.2f, WINDOW_HEIGHT * 0.7f);
    setlinecolor(RGB(255, 100, 100));
    
    for (int i = 0; i < 6; i++) {
        float a = -angle + i * 3.14159f / 3;
        Vector2 pos = leftCenter + Vector2((float)cos(a) * 60, (float)sin(a) * 60);
        solidcircle((int)pos.x, (int)pos.y, 2);
    }
}

void StartScreen::RenderTitle() {
    // 主标题阴影
    settextcolor(RGB(50, 50, 50));
    settextstyle(60, 0, _T("Arial"));
    int titleWidth = textwidth(_T("VAMPIRE SURVIVORS"));
    outtextxy((WINDOW_WIDTH - titleWidth) / 2 + 4, 120 + 4, _T("VAMPIRE SURVIVORS"));
    
    // 主标题
    settextcolor(RGB(255, 215, 0));
    outtextxy((WINDOW_WIDTH - titleWidth) / 2, 120, _T("VAMPIRE SURVIVORS"));
    
    // 副标题
    settextcolor(RGB(200, 200, 200));
    settextstyle(24, 0, _T("Arial"));
    int subtitleWidth = textwidth(_T("Endless Survival Adventure"));
    outtextxy((WINDOW_WIDTH - subtitleWidth) / 2, 190, _T("Endless Survival Adventure"));
    
    // 添加脉动效果
    DWORD time = GetTickCount();
    float pulse = (float)sin(time * 0.003f) * 0.3f + 0.7f;
    int pulseAlpha = (int)(200 * pulse);
    
    settextcolor(RGB(pulseAlpha, pulseAlpha, 100));
    settextstyle(18, 0, _T("Arial"));
    int mottoWidth = textwidth(_T("Survive the Night, Become the Legend"));
    outtextxy((WINDOW_WIDTH - mottoWidth) / 2, 220, _T("Survive the Night, Become the Legend"));
}

void StartScreen::RenderButtons() {
    for (size_t i = 0; i < buttons.size(); i++) {
        const Button& button = buttons[i];
        
        // 确定按钮颜色
        COLORREF buttonColor = button.normalColor;
        COLORREF textColor = WHITE;
        
        if ((int)i == selectedOption) {
            buttonColor = button.hoverColor;
            textColor = RGB(255, 255, 200);
            
            // 添加选中发光效果
            setlinecolor(RGB(255, 255, 100));
            setlinestyle(PS_SOLID, 4);
            rectangle(button.x - 6, button.y - 6, 
                     button.x + button.width + 6, button.y + button.height + 6);
            
            // 内层发光
            setlinecolor(RGB(255, 255, 200));
            setlinestyle(PS_SOLID, 2);
            rectangle(button.x - 3, button.y - 3, 
                     button.x + button.width + 3, button.y + button.height + 3);
        }
        
        // 绘制按钮背景
        setfillcolor(buttonColor);
        setlinecolor(RGB(150, 150, 150));
        setlinestyle(PS_SOLID, 2);
        fillrectangle(button.x, button.y, button.x + button.width, button.y + button.height);
        rectangle(button.x, button.y, button.x + button.width, button.y + button.height);
        
        // 绘制按钮文字
        settextcolor(textColor);
        settextstyle(22, 0, _T("Arial"));
        
        // 计算文字居中位置
        int textWidth = textwidth(button.text);
        int textHeight = textheight(button.text);
        int textX = button.x + (button.width - textWidth) / 2;
        int textY = button.y + (button.height - textHeight) / 2;
        
        // 如果选中，添加文字阴影
        if ((int)i == selectedOption) {
            settextcolor(RGB(80, 80, 80));
            outtextxy(textX + 2, textY + 2, button.text);
            settextcolor(textColor);
        }
        
        outtextxy(textX, textY, button.text);
        
        // 为不同按钮添加图标
        settextcolor(textColor);
        settextstyle(18, 0, _T("Arial"));
        if (i == 0) { // 开始游戏
            outtextxy(button.x + 15, textY, _T(">"));
        } else if (i == 1) { // 游戏说明
            outtextxy(button.x + 15, textY, _T("?"));
        } else if (i == 2) { // 退出
            outtextxy(button.x + 15, textY, _T("X"));
        }
    }
}

int StartScreen::HandleInput() {
    // 处理鼠标点击
    static bool mousePressed = false;
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
        if (!mousePressed) {
            mousePressed = true;
            
            // 检查是否点击了某个按钮
            for (size_t i = 0; i < buttons.size(); i++) {
                if (buttons[i].Contains(mousePos.x, mousePos.y)) {
                    selectedOption = (int)i;
                    return 1; // 模拟按下Enter
                }
            }
        }
    } else {
        mousePressed = false;
    }
    
    // 处理键盘输入
    if (_kbhit()) {
        int ch = _getch();
        switch (ch) {
        case 13: return 1; // Enter
        case 27: return 4; // ESC
        case 224: // Arrow keys
            ch = _getch();
            switch (ch) {
            case 72: return 2; // Up
            case 80: return 3; // Down
            }
            break;
        }
    }
    
    return 0;
}