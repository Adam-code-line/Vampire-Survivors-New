#include "MainMenu.h"
#include <conio.h>
#include <iterator>

MainMenu::MainMenu() : selectedCharacter(0), menuActive(true), mousePos(0, 0) {
    InitializeCharacters();
    InitializeCharacterCards();
}

void MainMenu::InitializeCharacters() {
    characters.clear();

    // 战士
    characters.emplace_back(
        CharacterType::WARRIOR,
        "Warrior",
        "Strong melee fighter with high health\nSkill: Berserker Rage - 360° attack",
        120.0f, 150, 1.0f, 30, SkillType::BERSERKER_RAGE, 8.0f
    );

    // 法师
    characters.emplace_back(
        CharacterType::MAGE,
        "Mage",
        "Powerful spellcaster with magic attacks\nSkill: Fireball - Triple fire projectiles",
        100.0f, 80, 1.5f, 20, SkillType::FIREBALL, 6.0f
    );

    // 弓箭手
    characters.emplace_back(
        CharacterType::ARCHER,
        "Archer",
        "Ranged fighter with high attack speed\nSkill: Multi-Shot - 8-direction arrows",
        140.0f, 100, 2.0f, 25, SkillType::MULTI_SHOT, 7.0f
    );

    // 刺客
    characters.emplace_back(
        CharacterType::ASSASSIN,
        "Assassin",
        "Fast and agile with teleport ability\nSkill: Shadow Strike - Teleport attack",
        160.0f, 90, 1.8f, 35, SkillType::SHADOW_STRIKE, 10.0f
    );
}

void MainMenu::InitializeCharacterCards() {
    characterCards.clear();
    
    int cardWidth = 250;
    int cardHeight = 350;
    int spacing = 20;
    int totalWidth = (int)characters.size() * cardWidth + ((int)characters.size() - 1) * spacing;
    int startX = (WINDOW_WIDTH - totalWidth) / 2;
    int startY = 180;

    for (size_t i = 0; i < characters.size(); i++) {
        int x = startX + (int)i * (cardWidth + spacing);
        int y = startY;
        characterCards.emplace_back(x, y, cardWidth, cardHeight);
    }
}

CharacterType MainMenu::ShowMenu() {
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
    setbkcolor(RGB(20, 20, 40));
    
    // 开始双缓冲绘制
    BeginBatchDraw();
    
    bool needsRedraw = true;
    DWORD lastTime = GetTickCount();
    
    while (menuActive) {
        DWORD currentTime = GetTickCount();
        
        // 更新鼠标悬停状态
        UpdateMouseHover();
        
        // 处理输入
        int input = HandleInput();
        
        if (input == 1) { // Enter pressed 或鼠标点击
            menuActive = false;
            break;
        }
        else if (input == 2) { // Left arrow
            selectedCharacter = (selectedCharacter - 1 + (int)characters.size()) % (int)characters.size();
            needsRedraw = true;
        }
        else if (input == 3) { // Right arrow
            selectedCharacter = (selectedCharacter + 1) % (int)characters.size();
            needsRedraw = true;
        }
        else if (input == 4) { // ESC 退出
            menuActive = false;
            // 返回默认角色或处理退出逻辑
            break;
        }
        
        // 检查是否需要重绘
        if (needsRedraw || (currentTime - lastTime >= 50)) { // 约20FPS
            RenderMenu();
            FlushBatchDraw();
            needsRedraw = false;
            lastTime = currentTime;
        }
        
        Sleep(10);
    }
    
    // 结束双缓冲绘制
    EndBatchDraw();
    
    return characters[selectedCharacter].type;
}

void MainMenu::UpdateMouseHover() {
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(GetHWnd(), &cursorPos);
    
    mousePos = Vector2((float)cursorPos.x, (float)cursorPos.y);
    
    // 更新每个角色卡片的悬停状态
    for (size_t i = 0; i < characterCards.size(); i++) {
        characterCards[i].isHovered = characterCards[i].Contains(mousePos.x, mousePos.y);
        
        // 如果鼠标悬停在某个卡片上，更新选中的角色
        if (characterCards[i].isHovered) {
            selectedCharacter = (int)i;
        }
    }
}

void MainMenu::RenderMenu() {
    cleardevice();

    // 绘制背景渐变
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        float ratio = (float)y / WINDOW_HEIGHT;
        int r = (int)(20 + ratio * 20);
        int g = (int)(20 + ratio * 30);
        int b = (int)(40 + ratio * 40);
        
        setlinecolor(RGB(r, g, b));
        line(0, y, WINDOW_WIDTH, y);
    }

    // 标题阴影
    settextcolor(RGB(50, 50, 50));
    settextstyle(48, 0, _T("Arial"));
    int titleWidth = textwidth(_T("VAMPIRE SURVIVORS"));
    outtextxy((WINDOW_WIDTH - titleWidth) / 2 + 3, 53, _T("VAMPIRE SURVIVORS"));

    // 标题
    settextcolor(RGB(255, 215, 0));
    settextstyle(48, 0, _T("Arial"));
    outtextxy((WINDOW_WIDTH - titleWidth) / 2, 50, _T("VAMPIRE SURVIVORS"));

    // 副标题
    settextcolor(RGB(200, 200, 200));
    settextstyle(24, 0, _T("Arial"));
    int subtitleWidth = textwidth(_T("Select Your Character"));
    outtextxy((WINDOW_WIDTH - subtitleWidth) / 2, 120, _T("Select Your Character"));

    RenderCharacterSelection();

    // 控制说明
    settextcolor(RGB(255, 255, 100));
    settextstyle(16, 0, _T("Arial"));
    outtextxy(50, WINDOW_HEIGHT - 80, _T("Use LEFT/RIGHT arrows or MOUSE to select"));
    outtextxy(50, WINDOW_HEIGHT - 60, _T("Press ENTER or CLICK to confirm"));
    outtextxy(50, WINDOW_HEIGHT - 40, _T("Press ESC to exit"));
}

void MainMenu::RenderCharacterSelection() {
    for (size_t i = 0; i < characters.size(); i++) {
        const CharacterCard& card = characterCards[i];
        int x = card.x;
        int y = card.y;

        // 选中或悬停效果
        if ((int)i == selectedCharacter) {
            // 外层发光效果
            setlinecolor(RGB(255, 255, 0));
            setlinestyle(PS_SOLID, 6);
            rectangle(x - 10, y - 10, x + card.width + 10, y + card.height + 10);
            
            // 中层发光
            setlinecolor(RGB(255, 255, 100));
            setlinestyle(PS_SOLID, 4);
            rectangle(x - 6, y - 6, x + card.width + 6, y + card.height + 6);
            
            // 内层发光
            setlinecolor(RGB(255, 255, 200));
            setlinestyle(PS_SOLID, 2);
            rectangle(x - 3, y - 3, x + card.width + 3, y + card.height + 3);
        }
        else if (card.isHovered) {
            // 悬停效果（较弱的高亮）
            setlinecolor(RGB(150, 150, 255));
            setlinestyle(PS_SOLID, 3);
            rectangle(x - 5, y - 5, x + card.width + 5, y + card.height + 5);
        }

        // 角色卡片背景
        COLORREF cardColor;
        if ((int)i == selectedCharacter) {
            cardColor = RGB(100, 100, 140);
        } else if (card.isHovered) {
            cardColor = RGB(80, 80, 120);
        } else {
            cardColor = RGB(60, 60, 80);
        }
        
        setfillcolor(cardColor);
        setlinecolor(RGB(150, 150, 150));
        setlinestyle(PS_SOLID, 1);
        fillrectangle(x, y, x + card.width, y + card.height);
        rectangle(x, y, x + card.width, y + card.height);

        RenderCharacterInfo(characters[i], x, y);
    }
}

void MainMenu::RenderCharacterInfo(const CharacterData& character, int x, int y) {
    // 角色名称
    settextcolor(WHITE);
    settextstyle(24, 0, _T("Arial"));

    // 转换string到TCHAR
    TCHAR nameText[50];
    MultiByteToWideChar(CP_ACP, 0, character.name.c_str(), -1, nameText, 50);
    
    // 居中显示角色名称
    int nameWidth = textwidth(nameText);
    outtextxy(x + (250 - nameWidth) / 2, y + 10, nameText);

    // 角色图标 (用颜色区分)
    COLORREF characterColor;
    TCHAR iconChar;
    switch (character.type) {
    case CharacterType::WARRIOR: 
        characterColor = RGB(220, 50, 50); 
        iconChar = _T('W');
        break;
    case CharacterType::MAGE: 
        characterColor = RGB(50, 50, 220); 
        iconChar = _T('M');
        break;
    case CharacterType::ARCHER: 
        characterColor = RGB(50, 220, 50); 
        iconChar = _T('A');
        break;
    case CharacterType::ASSASSIN: 
        characterColor = RGB(150, 50, 150); 
        iconChar = _T('S');
        break;
    default:
        characterColor = WHITE;
        iconChar = _T('?');
        break;
    }

    // 角色图标背景
    setfillcolor(characterColor);
    solidcircle(x + 125, y + 80, 40);
    
    // 角色图标边框
    setlinecolor(WHITE);
    setlinestyle(PS_SOLID, 3);
    circle(x + 125, y + 80, 40);
    
    // 角色图标字母
    settextcolor(WHITE);
    settextstyle(30, 0, _T("Arial"));
    TCHAR iconStr[2] = { iconChar, _T('\0') };
    int iconWidth = textwidth(iconStr);
    int iconHeight = textheight(iconStr);
    outtextxy(x + 125 - iconWidth/2, y + 80 - iconHeight/2, iconStr);

    // 属性信息
    settextcolor(RGB(200, 200, 200));
    settextstyle(14, 0, _T("Arial"));

    TCHAR statText[100];
    _stprintf_s(statText, _T("Speed: %.0f"), character.speed);
    outtextxy(x + 10, y + 140, statText);

    _stprintf_s(statText, _T("Health: %d"), character.health);
    outtextxy(x + 10, y + 160, statText);

    _stprintf_s(statText, _T("Attack Speed: %.1f"), character.attackSpeed);
    outtextxy(x + 10, y + 180, statText);

    _stprintf_s(statText, _T("Damage: %d"), character.damage);
    outtextxy(x + 10, y + 200, statText);

    // 技能信息
    settextcolor(RGB(255, 215, 0));
    settextstyle(14, 0, _T("Arial"));

    std::string skillName;
    switch (character.skill) {
    case SkillType::BERSERKER_RAGE: skillName = "Berserker Rage"; break;
    case SkillType::FIREBALL: skillName = "Fireball"; break;
    case SkillType::MULTI_SHOT: skillName = "Multi-Shot"; break;
    case SkillType::SHADOW_STRIKE: skillName = "Shadow Strike"; break;
    default: skillName = "Unknown"; break;
    }

    TCHAR skillText[50];
    MultiByteToWideChar(CP_ACP, 0, skillName.c_str(), -1, skillText, 50);
    outtextxy(x + 10, y + 230, _T("Skill:"));
    outtextxy(x + 10, y + 250, skillText);

    _stprintf_s(statText, _T("Cooldown: %.1fs"), character.skillCooldown);
    outtextxy(x + 10, y + 270, statText);

    // 修复：使用简单的索引比较而不是std::distance
    size_t characterIndex = 0;
    for (size_t j = 0; j < characters.size(); j++) {
        if (&characters[j] == &character) {
            characterIndex = j;
            break;
        }
    }
    
    if (selectedCharacter == (int)characterIndex) {
        settextcolor(RGB(150, 150, 255));
        settextstyle(12, 0, _T("Arial"));
        outtextxy(x + 10, y + 295, _T("Click to select"));
    }
}

int MainMenu::HandleInput() {
    // 处理鼠标点击
    static bool mousePressed = false;
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
        if (!mousePressed) {
            mousePressed = true;
            
            // 检查是否点击了某个角色卡片
            for (size_t i = 0; i < characterCards.size(); i++) {
                if (characterCards[i].Contains(mousePos.x, mousePos.y)) {
                    selectedCharacter = (int)i;
                    return 1; // 模拟按下Enter，直接确认选择
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
            case 75: return 2; // Left
            case 77: return 3; // Right
            }
            break;
        }
    }
    return 0;
}