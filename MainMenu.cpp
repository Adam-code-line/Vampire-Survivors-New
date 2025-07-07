#include "MainMenu.h"
#include <conio.h>

MainMenu::MainMenu() : selectedCharacter(0), menuActive(true) {
    InitializeCharacters();
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

CharacterType MainMenu::ShowMenu() {
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
    setbkcolor(RGB(20, 20, 40));
    
    // 开始双缓冲绘制
    BeginBatchDraw();
    
    bool needsRedraw = true;
    int lastSelectedCharacter = -1;
    DWORD lastTime = GetTickCount();
    
    while (menuActive) {
        DWORD currentTime = GetTickCount();
        
        // 处理输入
        int input = HandleInput();
        
        if (input == 1) { // Enter pressed
            menuActive = false;
            break;
        }
        else if (input == 2) { // Left arrow
            selectedCharacter = (selectedCharacter - 1 + characters.size()) % characters.size();
            needsRedraw = true;
        }
        else if (input == 3) { // Right arrow
            selectedCharacter = (selectedCharacter + 1) % characters.size();
            needsRedraw = true;
        }
        
        // 检查是否需要重绘（选择改变或超过刷新间隔）
        if (needsRedraw || (currentTime - lastTime >= 16)) { // 约60FPS
            RenderMenu();
            FlushBatchDraw(); // 刷新到屏幕
            needsRedraw = false;
            lastTime = currentTime;
        }
        
        Sleep(1); // 避免100%CPU占用
    }
    
    // 结束双缓冲绘制
    EndBatchDraw();
    
    return characters[selectedCharacter].type;
}

void MainMenu::RenderMenu() {
    cleardevice();

    // 标题
    settextcolor(WHITE);
    settextstyle(48, 0, _T("Arial"));
    outtextxy(WINDOW_WIDTH / 2 - 200, 50, _T("VAMPIRE SURVIVORS"));

    settextstyle(24, 0, _T("Arial"));
    outtextxy(WINDOW_WIDTH / 2 - 100, 120, _T("Select Your Character"));

    RenderCharacterSelection();

    // 控制说明
    settextcolor(YELLOW);
    settextstyle(16, 0, _T("Arial"));
    outtextxy(WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT - 60, _T("Use LEFT/RIGHT arrows to select"));
    outtextxy(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT - 40, _T("Press ENTER to confirm"));
}

void MainMenu::RenderCharacterSelection() {
    int cardWidth = 250;
    int cardHeight = 350;
    int spacing = 20;
    int totalWidth = characters.size() * cardWidth + (characters.size() - 1) * spacing;
    int startX = (WINDOW_WIDTH - totalWidth) / 2;
    int startY = 180;

    for (size_t i = 0; i < characters.size(); i++) {
        int x = startX + i * (cardWidth + spacing);
        int y = startY;

        // 选中框 - 使用更明显的高亮效果
        if (i == selectedCharacter) {
            setlinecolor(YELLOW);
            setlinestyle(PS_SOLID, 4); // 更粗的边框
            rectangle(x - 8, y - 8, x + cardWidth + 8, y + cardHeight + 8);
            
            // 添加发光效果
            setlinecolor(RGB(255, 255, 100));
            setlinestyle(PS_SOLID, 2);
            rectangle(x - 6, y - 6, x + cardWidth + 6, y + cardHeight + 6);
        }

        // 角色卡片背景
        COLORREF cardColor = (i == selectedCharacter) ? RGB(80, 80, 100) : RGB(60, 60, 80);
        setfillcolor(cardColor);
        setlinecolor(WHITE);
        setlinestyle(PS_SOLID, 1);
        fillrectangle(x, y, x + cardWidth, y + cardHeight);

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
    outtextxy(x + 10, y + 10, nameText);

    // 角色图标 (用颜色区分)
    COLORREF characterColor;
    switch (character.type) {
    case CharacterType::WARRIOR: characterColor = RED; break;
    case CharacterType::MAGE: characterColor = BLUE; break;
    case CharacterType::ARCHER: characterColor = GREEN; break;
    case CharacterType::ASSASSIN: characterColor = RGB(128, 0, 128); break;
    }

    setfillcolor(characterColor);
    solidcircle(x + 125, y + 80, 40);
    
    // 添加角色图标边框
    setlinecolor(WHITE);
    setlinestyle(PS_SOLID, 2);
    circle(x + 125, y + 80, 40);

    // 属性信息
    settextcolor(WHITE);
    settextstyle(16, 0, _T("Arial"));

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
    settextcolor(YELLOW);
    settextstyle(14, 0, _T("Arial"));

    std::string skillName;
    switch (character.skill) {
    case SkillType::BERSERKER_RAGE: skillName = "Berserker Rage"; break;
    case SkillType::FIREBALL: skillName = "Fireball"; break;
    case SkillType::MULTI_SHOT: skillName = "Multi-Shot"; break;
    case SkillType::SHADOW_STRIKE: skillName = "Shadow Strike"; break;
    }

    TCHAR skillText[50];
    MultiByteToWideChar(CP_ACP, 0, skillName.c_str(), -1, skillText, 50);
    outtextxy(x + 10, y + 230, _T("Skill:"));
    outtextxy(x + 10, y + 250, skillText);

    _stprintf_s(statText, _T("Cooldown: %.1fs"), character.skillCooldown);
    outtextxy(x + 10, y + 270, statText);
}

int MainMenu::HandleInput() {
    if (_kbhit()) {
        int ch = _getch();
        switch (ch) {
        case 13: return 1; // Enter
        case 27: return 4; // ESC (可选：退出游戏)
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