#include "VictoryScreen.h"
#include "ImageManager.h"
#include <conio.h>
#include <cmath>
#include <cstdlib>  // 添加 rand() 和 srand() 支持

VictoryScreen::VictoryScreen(int score, int level, int enemies, float time) 
    : screenActive(true), selectedOption(0), finalScore(score), finalLevel(level), 
      enemiesDefeated(enemies), survivalTime(time), mousePos(0.0f, 0.0f) {  // 修复初始化
    animationTime = GetTickCount();
    InitializeButtons();
}

void VictoryScreen::InitializeButtons() {
    buttons.clear();
    
    int buttonWidth = 280;
    int buttonHeight = 60;
    int centerX = WINDOW_WIDTH / 2 - buttonWidth / 2;
    int startY = WINDOW_HEIGHT / 2 + 120;
    int spacing = 80;
    
    // 返回主菜单按钮
    buttons.emplace_back(centerX, startY, buttonWidth, buttonHeight, 
                        _T("RETURN TO MAIN MENU"), VictoryChoice::RETURN_TO_MENU,
                        RGB(0, 120, 200), RGB(0, 160, 255));
    
    // 退出游戏按钮
    buttons.emplace_back(centerX, startY + spacing, buttonWidth, buttonHeight, 
                        _T("EXIT GAME"), VictoryChoice::EXIT_GAME,
                        RGB(150, 60, 60), RGB(200, 80, 80));
}

VictoryChoice VictoryScreen::ShowVictoryScreen() {
    BeginBatchDraw();
    
    bool needsRedraw = true;
    DWORD lastTime = GetTickCount();
    
    while (screenActive) {
        DWORD currentTime = GetTickCount();
        animationTime = currentTime;
        
        UpdateMouseHover();
        
        int input = HandleInput();
        
        if (input == 1) { // Enter 或鼠标点击
            VictoryChoice choice = buttons[selectedOption].choice;
            screenActive = false;
            EndBatchDraw();
            return choice;
        }
        else if (input == 2) { // 上箭头
            selectedOption = (selectedOption - 1 + (int)buttons.size()) % (int)buttons.size();
            needsRedraw = true;
        }
        else if (input == 3) { // 下箭头
            selectedOption = (selectedOption + 1) % (int)buttons.size();
            needsRedraw = true;
        }
        else if (input == 4) { // ESC - 默认返回主菜单
            screenActive = false;
            EndBatchDraw();
            return VictoryChoice::RETURN_TO_MENU;
        }
        
        if (needsRedraw || (currentTime - lastTime >= 50)) {
            RenderVictoryScreen();
            FlushBatchDraw();
            needsRedraw = false;
            lastTime = currentTime;
        }
        
        Sleep(10);
    }
    
    EndBatchDraw();
    return VictoryChoice::RETURN_TO_MENU;
}

void VictoryScreen::UpdateMouseHover() {
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

void VictoryScreen::RenderVictoryScreen() {
    RenderBackground();
    DrawVictoryEffects();
    RenderTitle();
    RenderStats();
    RenderButtons();
    
    // 控制提示
    settextcolor(RGB(200, 200, 200));
    settextstyle(16, 0, _T("Arial"));
    outtextxy(30, WINDOW_HEIGHT - 60, _T("Use mouse or arrow keys + ENTER to select"));
    outtextxy(30, WINDOW_HEIGHT - 40, _T("Press ESC to return to main menu"));
}

void VictoryScreen::RenderBackground() {
    // 使用背景图片
    ImageManager* imgManager = ImageManager::GetInstance();
    imgManager->DrawScaledBackground(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // 添加胜利专用的金色渐变遮罩
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        float ratio = (float)y / WINDOW_HEIGHT;
        int alpha = (int)(60 + ratio * 40); // 更亮的遮罩
        int r = alpha;
        int g = (int)(alpha * 0.8f);
        int b = (int)(alpha * 0.3f);
        
        setlinecolor(RGB(r, g, b));
        line(0, y, WINDOW_WIDTH, y);
    }
}

void VictoryScreen::DrawVictoryEffects() {
    DWORD time = animationTime;
    
    // 绘制旋转的胜利光环
    for (int ring = 0; ring < 3; ring++) {
        float baseRadius = 120.0f + ring * 40.0f;
        float angle = (time * 0.001f * (ring + 1));
        int particleCount = 8 + ring * 4;
        
        for (int i = 0; i < particleCount; i++) {
            float particleAngle = angle + (i * 2.0f * 3.14159f / particleCount);
            float radius = baseRadius + (float)sin(time * 0.003f + i) * 20.0f;  // 修复类型转换
            
            int x = WINDOW_WIDTH / 2 + (int)((float)cos(particleAngle) * radius);  // 修复类型转换
            int y = WINDOW_HEIGHT / 2 - 50 + (int)((float)sin(particleAngle) * radius * 0.6f);  // 修复类型转换
            
            // 根据环层设置不同颜色
            COLORREF color;
            int size;
            switch (ring) {
                case 0: 
                    color = RGB(255, 215, 0); // 金色
                    size = 4;
                    break;
                case 1: 
                    color = RGB(255, 255, 100); // 亮黄色
                    size = 3;
                    break;
                case 2: 
                    color = RGB(255, 255, 200); // 淡黄色
                    size = 2;
                    break;
                default:
                    color = RGB(255, 255, 255);
                    size = 2;
                    break;
            }
            
            setfillcolor(color);
            solidcircle(x, y, size);
        }
    }
    
    // 绘制随机闪烁的星星效果
    srand(12345); // 固定种子确保位置一致
    for (int i = 0; i < 30; i++) {
        float x = (float)(rand() % WINDOW_WIDTH);
        float y = (float)(rand() % (WINDOW_HEIGHT / 2)); // 只在上半部分
        
        float twinkle = (float)sin((time * 0.005f) + i * 0.5f) * 0.5f + 0.5f;
        if (twinkle > 0.7f) { // 只显示亮度较高的星星
            int brightness = (int)(200 + 55 * twinkle);
            setfillcolor(RGB(brightness, brightness, brightness));
            solidcircle((int)x, (int)y, 1 + (i % 3));
        }
    }
}

void VictoryScreen::RenderTitle() {
    // 胜利标题 - 多层阴影效果
    settextstyle(72, 0, _T("Arial"));
    int titleWidth = textwidth(_T("VICTORY!"));
    int titleX = (WINDOW_WIDTH - titleWidth) / 2;
    int titleY = WINDOW_HEIGHT / 2 - 180;
    
    // 外层阴影
    settextcolor(RGB(20, 20, 20));
    outtextxy(titleX + 6, titleY + 6, _T("VICTORY!"));
    
    // 中层阴影
    settextcolor(RGB(100, 80, 20));
    outtextxy(titleX + 3, titleY + 3, _T("VICTORY!"));
    
    // 主标题 - 渐变金色效果
    settextcolor(RGB(255, 215, 0));
    outtextxy(titleX, titleY, _T("VICTORY!"));
    
    // 添加发光效果
    settextcolor(RGB(255, 255, 150));
    settextstyle(70, 0, _T("Arial"));
    outtextxy(titleX + 1, titleY + 1, _T("VICTORY!"));
    
    // 副标题
    settextcolor(RGB(255, 255, 255));
    settextstyle(28, 0, _T("Arial"));
    TCHAR subtitle[100];
    int minutes = (int)survivalTime / 60;
    int seconds = (int)survivalTime % 60;
    _stprintf_s(subtitle, _T("You survived %02d:%02d and became a legend!"), minutes, seconds);
    int subtitleWidth = textwidth(subtitle);
    outtextxy((WINDOW_WIDTH - subtitleWidth) / 2, titleY + 80, subtitle);
    
    // 额外的励志文字
    settextcolor(RGB(200, 200, 255));
    settextstyle(20, 0, _T("Arial"));
    const TCHAR* motivationText = _T("The night is conquered, the legend is born!");  // 修改为 const TCHAR*
    int motivationWidth = textwidth(motivationText);
    outtextxy((WINDOW_WIDTH - motivationWidth) / 2, titleY + 115, motivationText);
}

void VictoryScreen::RenderStats() {
    // 统计信息面板
    int panelX = WINDOW_WIDTH / 2 - 200;
    int panelY = WINDOW_HEIGHT / 2 - 60;
    int panelWidth = 400;
    int panelHeight = 140;
    
    // 面板背景
    setfillcolor(RGB(0, 0, 0));
    setlinecolor(RGB(255, 215, 0));
    setlinestyle(PS_SOLID, 3);
    fillrectangle(panelX, panelY, panelX + panelWidth, panelY + panelHeight);
    rectangle(panelX, panelY, panelX + panelWidth, panelY + panelHeight);
    
    // 内边框
    setlinecolor(RGB(255, 255, 150));
    setlinestyle(PS_SOLID, 1);
    rectangle(panelX + 5, panelY + 5, panelX + panelWidth - 5, panelY + panelHeight - 5);
    
    // 统计标题
    settextcolor(RGB(255, 215, 0));
    settextstyle(24, 0, _T("Arial"));
    const TCHAR* statsTitle = _T("FINAL STATISTICS");  // 修改为 const TCHAR*
    int titleWidth = textwidth(statsTitle);
    outtextxy(panelX + (panelWidth - titleWidth) / 2, panelY + 10, statsTitle);
    
    // 统计数据
    settextcolor(RGB(255, 255, 255));
    settextstyle(18, 0, _T("Arial"));
    
    int statY = panelY + 45;
    int lineHeight = 22;
    
    TCHAR statText[100];
    
    // 最终分数
    _stprintf_s(statText, _T("Final Score: %d"), finalScore);
    outtextxy(panelX + 20, statY, statText);
    
    // 最终等级
    _stprintf_s(statText, _T("Final Level: %d"), finalLevel);
    outtextxy(panelX + 220, statY, statText);
    
    statY += lineHeight;
    
    // 击败敌人数
    _stprintf_s(statText, _T("Enemies Defeated: %d"), enemiesDefeated);
    outtextxy(panelX + 20, statY, statText);
    
    // 生存时间
    int minutes = (int)survivalTime / 60;
    int seconds = (int)survivalTime % 60;
    _stprintf_s(statText, _T("Time: %02d:%02d"), minutes, seconds);
    outtextxy(panelX + 220, statY, statText);
    
    statY += lineHeight;
    
    // 额外成就信息
    settextcolor(RGB(255, 215, 0));
    settextstyle(16, 0, _T("Arial"));
    const TCHAR* achievement = _T("Achievement Unlocked: Vampire Slayer!");  // 修改为 const TCHAR*
    int achievementWidth = textwidth(achievement);
    outtextxy(panelX + (panelWidth - achievementWidth) / 2, statY + 10, achievement);
}

void VictoryScreen::RenderButtons() {
    for (size_t i = 0; i < buttons.size(); i++) {
        const VictoryButton& button = buttons[i];
        
        // 确定按钮颜色和效果
        COLORREF buttonColor = button.normalColor;
        COLORREF textColor = WHITE;
        
        if ((int)i == selectedOption) {
            buttonColor = button.hoverColor;
            textColor = RGB(255, 255, 200);
            
            // 添加选中发光效果
            setlinecolor(RGB(255, 255, 100));
            setlinestyle(PS_SOLID, 6);
            rectangle(button.x - 8, button.y - 8, 
                     button.x + button.width + 8, button.y + button.height + 8);
            
            // 中层发光
            setlinecolor(RGB(255, 255, 200));
            setlinestyle(PS_SOLID, 4);
            rectangle(button.x - 5, button.y - 5, 
                     button.x + button.width + 5, button.y + button.height + 5);
            
            // 内层发光
            setlinecolor(RGB(255, 255, 255));
            setlinestyle(PS_SOLID, 2);
            rectangle(button.x - 2, button.y - 2, 
                     button.x + button.width + 2, button.y + button.height + 2);
        }
        
        // 绘制按钮背景
        setfillcolor(buttonColor);
        setlinecolor(RGB(200, 200, 200));
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
        
        // 为不同按钮添加图标（使用基本字符避免编码问题）
        settextcolor(textColor);
        settextstyle(20, 0, _T("Arial"));
        if (button.choice == VictoryChoice::RETURN_TO_MENU) {
            outtextxy(button.x + 15, textY - 2, _T("<<"));  // 使用简单字符替代特殊符号
        } else if (button.choice == VictoryChoice::EXIT_GAME) {
            outtextxy(button.x + 15, textY - 2, _T("X"));   // 使用简单字符替代特殊符号
        }
    }
}

int VictoryScreen::HandleInput() {
    // 处理鼠标点击
    static bool mousePressed = false;
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
        if (!mousePressed) {
            mousePressed = true;
            
            // 检查是否点击了某个按钮
            for (size_t i = 0; i < buttons.size(); i++) {
                if (buttons[i].Contains(mousePos.x, mousePos.y)) {
                    selectedOption = (int)i;
                    
                    // 等待鼠标释放后再返回
                    while (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                        Sleep(10);
                    }
                    
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