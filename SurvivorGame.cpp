#include "SurvivorGame.h"
#include "ImageManager.h"

SurvivorGame::SurvivorGame() : gameTime(0), gameOver(false), score(0), 
                               selectedCharacter(CharacterType::WARRIOR) {
}

void SurvivorGame::Run() {
    // 显示开始界面
    StartScreen startScreen;
    if (!startScreen.ShowStartScreen()) {
        return; // 用户选择退出
    }
    
    // 显示角色选择菜单
    selectedCharacter = ShowCharacterSelection();
    
    // 重新初始化图形窗口用于游戏
    closegraph();
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // 移除原来的背景色设置，因为现在使用图片背景
    // setbkcolor(RGB(34, 139, 34));
    
    // 加载图片资源
    ImageManager* imgManager = ImageManager::GetInstance();
    imgManager->LoadCharacterImages();
    
    // 初始化游戏
    InitializeGame(selectedCharacter);
    
    // 设置中文字体支持
    LOGFONT font;
    gettextstyle(&font);
    _tcscpy_s(font.lfFaceName, _T("SimHei"));
    font.lfCharSet = GB2312_CHARSET;
    font.lfQuality = ANTIALIASED_QUALITY;
    settextstyle(&font);
    
    BeginBatchDraw();

    while (!gameOver && !_kbhit()) {
        Update();
        Render();
        Sleep(16);
    }

    EndBatchDraw();
    ShowGameOver();
    
    // 清理图片资源
    imgManager->ReleaseAll();
    
    closegraph();
}

CharacterType SurvivorGame::ShowCharacterSelection() {
    MainMenu menu;
    return menu.ShowMenu();
}

void SurvivorGame::InitializeGame(CharacterType characterType) {
    // 创建玩家
    player = std::make_unique<Player>(
        Vector2((float)(WINDOW_WIDTH / 2), (float)(WINDOW_HEIGHT / 2)), 
        characterType
    );
    
    // 应用角色数据
    CharacterData charData = GetCharacterData(characterType);
    player->ApplyCharacterStats(charData);
    
    // 初始化系统
    weaponSystem = std::make_unique<EnhancedWeaponSystem>(player.get(), &bullets, &meleeAttacks);
    spawner = std::make_unique<EnemySpawner>(player.get());
    skillSystem = std::make_unique<SkillSystem>(player.get(), &bullets, &meleeAttacks, &enemies);
    skillSystem->SetSkill(charData.skill, charData.skillCooldown);
    itemSystem = std::make_unique<ItemSystem>(player.get(), &items);
    
    lastTime = GetTickCount();
}

CharacterData SurvivorGame::GetCharacterData(CharacterType type) {
    switch (type) {
        case CharacterType::WARRIOR:
            return CharacterData(type, "Warrior", "", 120.0f, 150, 1.0f, 30, 
                               SkillType::BERSERKER_RAGE, 8.0f);
        case CharacterType::MAGE:
            return CharacterData(type, "Mage", "", 100.0f, 80, 1.5f, 20, 
                               SkillType::FIREBALL, 6.0f);
        case CharacterType::ARCHER:
            return CharacterData(type, "Archer", "", 140.0f, 100, 2.0f, 25, 
                               SkillType::MULTI_SHOT, 7.0f);
        case CharacterType::ASSASSIN:
            return CharacterData(type, "Assassin", "", 160.0f, 90, 1.8f, 35, 
                               SkillType::SHADOW_STRIKE, 10.0f);
        default:
            return CharacterData(type, "Unknown", "", 100.0f, 100, 1.0f, 20, 
                               SkillType::BERSERKER_RAGE, 5.0f);
    }
}

void SurvivorGame::Update() {
    DWORD currentTime = GetTickCount();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    gameTime += deltaTime;

    // 检查技能释放
    if (GetAsyncKeyState('Q') & 0x8000) {
        skillSystem->TriggerSkill();
    }

    player->Update(deltaTime);
    weaponSystem->Update(deltaTime, enemies);
    spawner->Update(deltaTime, enemies);
    skillSystem->Update(deltaTime);
    itemSystem->Update(deltaTime);

    for (auto& bullet : bullets) {
        bullet->Update(deltaTime);
    }

    for (auto& melee : meleeAttacks) {
        melee->Update(deltaTime);
    }

    for (auto& enemy : enemies) {
        enemy->Update(deltaTime);
    }

    for (auto& gem : gems) {
        gem->Update(deltaTime);
    }
    
    for (auto& item : items) {
        item->Update(deltaTime);
    }

    HandleCollisions();
    CleanupObjects();

    if (!player->active) {
        gameOver = true;
    }
}

void SurvivorGame::HandleCollisions() {
    // 子弹与敌人碰撞
    for (auto& bullet : bullets) {
        if (!bullet->active) continue;

        for (auto& enemy : enemies) {
            if (!enemy->active) continue;

            if (bullet->CheckCollision(*enemy)) {
                enemy->TakeDamage(bullet->GetDamage());
                bullet->active = false;

                if (!enemy->active) {
                    gems.push_back(std::make_unique<ExperienceGem>(
                        enemy->position, enemy->GetExperienceValue()));
                    score += 10;
                    
                    // 在敌人死亡时生成道具
                    itemSystem->SpawnItem(enemy->position);
                }
                break;
            }
        }
    }

    // 近战攻击与敌人碰撞
    for (auto& melee : meleeAttacks) {
        if (!melee->active) continue;

        for (auto& enemy : enemies) {
            if (!enemy->active) continue;

            if (melee->IsInAttackRange(enemy->position)) {
                enemy->TakeDamage(melee->GetDamage());

                if (!enemy->active) {
                    gems.push_back(std::make_unique<ExperienceGem>(
                        enemy->position, enemy->GetExperienceValue()));
                    score += 15;
                    
                    // 在敌人死亡时生成道具
                    itemSystem->SpawnItem(enemy->position);
                }
            }
        }
    }

    // 剑气与敌人碰撞
    weaponSystem->CheckSwordAuraCollisions(enemies);

    // 处理剑气击杀敌人后的奖励
    for (auto& enemy : enemies) {
        if (!enemy->active) {
            gems.push_back(std::make_unique<ExperienceGem>(
                enemy->position, enemy->GetExperienceValue()));
            score += 20; // 剑气击杀给更多分数
            itemSystem->SpawnItem(enemy->position);
        }
    }

    // 敌人与玩家碰撞
    for (auto& enemy : enemies) {
        if (enemy->active && player->CheckCollision(*enemy)) {
            player->TakeDamage(1);
            enemy->active = false;
        }
    }

    // 经验宝石收集
    for (auto& gem : gems) {
        if (gem->active && player->CheckCollision(*gem)) {
            player->GainExperience(gem->GetValue());
            gem->active = false;

            if (player->GetExperience() == 0) {
                weaponSystem->UpgradeFireRate(1.1f);
                weaponSystem->UpgradeMeleeRate(1.1f);
            }
        }
    }

    // 道具收集
    for (auto& item : items) {
        if (item->active && player->CheckCollision(*item)) {
            itemSystem->CollectItem(item.get());
        }
    }
}

void SurvivorGame::CleanupObjects() {
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const std::unique_ptr<Bullet>& b) { return !b->active; }), bullets.end());

    meleeAttacks.erase(std::remove_if(meleeAttacks.begin(), meleeAttacks.end(),
        [](const std::unique_ptr<MeleeAttack>& m) { return !m->active; }), meleeAttacks.end());

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const std::unique_ptr<Enemy>& e) { return !e->active; }), enemies.end());

    gems.erase(std::remove_if(gems.begin(), gems.end(),
        [](const std::unique_ptr<ExperienceGem>& g) { return !g->active; }), gems.end());

    items.erase(std::remove_if(items.begin(), items.end(),
        [](const std::unique_ptr<Item>& i) { return !i->active; }), items.end());
}

void SurvivorGame::Render() {
    // 不再使用 cleardevice()，而是直接绘制背景图片
    
    // 绘制背景图片
    ImageManager* imgManager = ImageManager::GetInstance();
    imgManager->DrawScaledBackground(WINDOW_WIDTH, WINDOW_HEIGHT);

    for (auto& gem : gems) {
        gem->Render();
    }
    
    for (auto& item : items) {  // 新增：渲染道具
        item->Render();
    }

    for (auto& enemy : enemies) {
        enemy->Render();
    }

    for (auto& bullet : bullets) {
        bullet->Render();
    }

    for (auto& melee : meleeAttacks) {
        melee->Render();
    }

    // 渲染武器系统（包括剑气）
    weaponSystem->Render();

    player->Render();
    DrawUI();
    FlushBatchDraw();
}

void SurvivorGame::DrawUI() {
    // === 顶部信息栏 ===
    settextcolor(WHITE);
    settextstyle(24, 0, _T("Arial"));

    // 时间显示
    TCHAR timeText[50];
    int minutes = (int)gameTime / 60;
    int seconds = (int)gameTime % 60;
    _stprintf_s(timeText, _T("Time: %02d:%02d"), minutes, seconds);
    outtextxy(WINDOW_WIDTH / 2 - 80, 20, timeText);

    // 等级显示
    TCHAR levelText[50];
    _stprintf_s(levelText, _T("Lv.%d"), player->GetLevel());
    outtextxy(20, 20, levelText);

    // 分数显示
    TCHAR scoreText[50];
    _stprintf_s(scoreText, _T("Score: %d"), score);
    outtextxy(WINDOW_WIDTH - 150, 20, scoreText);

    // === 经验条 ===
    int expBarWidth = 200;
    int expBarHeight = 20;
    int expBarX = 20;
    int expBarY = 50;

    // 经验条背景
    setfillcolor(RGB(40, 40, 40));
    solidrectangle(expBarX, expBarY, expBarX + expBarWidth, expBarY + expBarHeight);
    
    // 经验条边框
    setlinecolor(WHITE);
    rectangle(expBarX, expBarY, expBarX + expBarWidth, expBarY + expBarHeight);

    // 经验条填充
    int expWidth = (expBarWidth * player->GetExperience()) / player->GetExperienceToNext();
    setfillcolor(RGB(0, 150, 255)); // 蓝色经验条
    solidrectangle(expBarX + 1, expBarY + 1, expBarX + expWidth - 1, expBarY + expBarHeight - 1);

    // 经验文字
    settextcolor(WHITE);
    settextstyle(12, 0, _T("Arial"));
    TCHAR expText[50];
    _stprintf_s(expText, _T("EXP: %d/%d"), player->GetExperience(), player->GetExperienceToNext());
    outtextxy(expBarX + 5, expBarY + 4, expText);

    // === 敌人信息 ===
    TCHAR enemyText[50];
    _stprintf_s(enemyText, _T("Enemies: %d"), (int)enemies.size());
    outtextxy(WINDOW_WIDTH - 150, 50, enemyText);

    // === 右侧信息面板 ===
    DrawEnemyLevelInfo();
    
    // === 左侧面板 ===
    DrawSkillUI();
    DrawItemEffects();  // 道具效果显示

    // === 底部控制提示 ===
    settextcolor(RGB(255, 255, 100)); // 浅黄色
    settextstyle(14, 0, _T("Arial"));
    outtextxy(20, WINDOW_HEIGHT - 80, _T("Controls:"));
    outtextxy(20, WINDOW_HEIGHT - 60, _T("Q - Use Skill"));
    outtextxy(20, WINDOW_HEIGHT - 45, _T("WASD - Move"));
    outtextxy(20, WINDOW_HEIGHT - 30, _T("Collect items for power-ups!"));
}

void SurvivorGame::DrawSkillUI() {
    // 技能面板
    int skillPanelX = 15;
    int skillPanelY = 80;
    int skillPanelWidth = 220;
    int skillPanelHeight = 35;
    
    // 技能面板背景
    setfillcolor(RGB(0, 0, 0));
    setlinecolor(RGB(100, 100, 100));
    fillrectangle(skillPanelX, skillPanelY, skillPanelX + skillPanelWidth, skillPanelY + skillPanelHeight);
    rectangle(skillPanelX, skillPanelY, skillPanelX + skillPanelWidth, skillPanelY + skillPanelHeight);
    
    // 技能标题
    settextcolor(RGB(255, 215, 0)); // 金色
    settextstyle(16, 0, _T("Arial"));
    outtextxy(skillPanelX + 10, skillPanelY + 5, _T("Skill (Q)"));
    
    // 技能冷却条
    float cooldownPercent = skillSystem->GetSkillCooldownPercent();
    
    int skillBarWidth = 150;
    int skillBarHeight = 8;
    int skillBarX = skillPanelX + 60;
    int skillBarY = skillPanelY + 22;
    
    // 技能条背景
    setfillcolor(RGB(40, 40, 40));
    solidrectangle(skillBarX, skillBarY, skillBarX + skillBarWidth, skillBarY + skillBarHeight);
    
    // 技能条填充
    int fillWidth = (int)(skillBarWidth * cooldownPercent);
    if (skillSystem->IsSkillReady()) {
        setfillcolor(RGB(0, 255, 0)); // 绿色 - 可用
        // 添加就绪提示
        settextcolor(RGB(0, 255, 0));
        settextstyle(12, 0, _T("Arial"));
        outtextxy(skillBarX + skillBarWidth + 10, skillBarY - 2, _T("READY"));
    } else {
        setfillcolor(RGB(255, 165, 0)); // 橙色 - 冷却中
    }
    solidrectangle(skillBarX, skillBarY, skillBarX + fillWidth, skillBarY + skillBarHeight);
    
    // 技能条边框
    setlinecolor(WHITE);
    rectangle(skillBarX, skillBarY, skillBarX + skillBarWidth, skillBarY + skillBarHeight);
}

void SurvivorGame::DrawEnemyLevelInfo() {
    int currentEnemyLevel = spawner->GetCurrentEnemyLevel();
    
    // 显示当前敌人等级
    settextcolor(RGB(255, 100, 100));
    settextstyle(20, 0, _T("Arial"));
    TCHAR enemyLevelText[50];
    _stprintf_s(enemyLevelText, _T("Enemy Lv: %d"), currentEnemyLevel);
    outtextxy(WINDOW_WIDTH - 150, 80, enemyLevelText);
    
    // 显示下次升级时间
    float nextLevelTime = (currentEnemyLevel * 30.0f) - gameTime;
    if (nextLevelTime > 0 && currentEnemyLevel < 5) {
        TCHAR nextUpgradeText[50];
        _stprintf_s(nextUpgradeText, _T("Next: %.0fs"), nextLevelTime);
        outtextxy(WINDOW_WIDTH - 150, 105, nextUpgradeText);
    } else if (currentEnemyLevel >= 5) {
        settextcolor(RED);
        outtextxy(WINDOW_WIDTH - 150, 105, _T("MAX LEVEL"));
    }
}

void SurvivorGame::DrawItemEffects() {
    const auto& effects = itemSystem->GetActiveEffects();
    
    if (effects.empty()) return; // 如果没有效果就不显示
    
    // 绘制效果面板背景
    int panelX = 15;
    int panelY = 120;
    int panelWidth = 220;
    int panelHeight = 20 + (int)effects.size() * 18 + 10;
    
    // 半透明背景
    setfillcolor(RGB(0, 0, 0));
    setlinecolor(RGB(100, 100, 100));
    fillrectangle(panelX, panelY, panelX + panelWidth, panelY + panelHeight);
    rectangle(panelX, panelY, panelX + panelWidth, panelY + panelHeight);
    
    // 标题
    settextcolor(RGB(255, 215, 0)); // 金色
    settextstyle(16, 0, _T("Arial"));
    outtextxy(panelX + 10, panelY + 5, _T("Active Effects"));
    
    int yOffset = panelY + 25;
    settextstyle(14, 0, _T("Arial"));
    
    for (const auto& effect : effects) {
        TCHAR effectText[100];
        COLORREF effectColor;
        LPCTSTR effectName;  // 使用 LPCTSTR 而不是 std::string
        LPCTSTR effectIcon;
        
        // 根据道具类型设置名称、图标和颜色
        switch (effect.type) {
            case ItemType::SPEED_BOOST: 
                effectName = _T("Speed Boost");
                effectIcon = _T(">>>");
                effectColor = RGB(0, 255, 255); // 青色
                break;
            case ItemType::DAMAGE_UP: 
                effectName = _T("Damage Up");
                effectIcon = _T("ATK");
                effectColor = RGB(255, 100, 100); // 红色
                break;
            case ItemType::FIRE_RATE_UP: 
                effectName = _T("Fire Rate Up");
                effectIcon = _T("RPD");
                effectColor = RGB(255, 255, 0); // 黄色
                break;
            case ItemType::SHIELD: 
                effectName = _T("Shield");
                effectIcon = _T("DEF");
                effectColor = RGB(192, 192, 192); // 银色
                break;
            case ItemType::EXPERIENCE_BOOST: 
                effectName = _T("EXP Boost");
                effectIcon = _T("EXP");
                effectColor = RGB(0, 255, 0); // 绿色
                break;
            case ItemType::MULTI_SHOT: 
                effectName = _T("Multi Shot");
                effectIcon = _T("MUL");
                effectColor = RGB(100, 100, 255); // 蓝色
                break;
            default: 
                effectName = _T("Unknown");
                effectIcon = _T("???");
                effectColor = WHITE;
                break;
        }
        
        // 绘制效果图标
        settextcolor(effectColor);
        outtextxy(panelX + 10, yOffset, effectIcon);
        
        // 绘制效果名称和剩余时间
        if (effect.remainingTime > 0) {
            _stprintf_s(effectText, _T("%s: %.1fs"), effectName, effect.remainingTime);
        } else {
            _stprintf_s(effectText, _T("%s: Permanent"), effectName);
        }
        
        // 根据剩余时间改变颜色
        if (effect.remainingTime < 3.0f && effect.remainingTime > 0) {
            settextcolor(RGB(255, 150, 150)); // 即将过期时闪烁红色
        } else {
            settextcolor(effectColor);
        }
        
        outtextxy(panelX + 45, yOffset, effectText);
        
        // 绘制时间条
        if (effect.remainingTime > 0 && effect.duration > 0) {
            int barWidth = 150;
            int barHeight = 4;
            int barX = panelX + 45;
            int barY = yOffset + 14;
            
            // 背景条
            setfillcolor(RGB(50, 50, 50));
            solidrectangle(barX, barY, barX + barWidth, barY + barHeight);
            
            // 进度条
            int fillWidth = (int)(barWidth * (effect.remainingTime / effect.duration));
            setfillcolor(effectColor);
            solidrectangle(barX, barY, barX + fillWidth, barY + barHeight);
        }
        
        yOffset += 18;
    }
}

void SurvivorGame::ShowGameOver() {
    // 在游戏结束界面也使用背景图片
    ImageManager* imgManager = ImageManager::GetInstance();
    imgManager->DrawScaledBackground(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // 添加半透明遮罩
    setfillcolor(RGB(0, 0, 0));
    setfillstyle(BS_SOLID);
    // 创建半透明效果
    for (int alpha = 0; alpha < 128; alpha += 8) {
        setfillcolor(RGB(alpha/5, alpha/5, alpha/5));
        solidrectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    settextcolor(RED);
    settextstyle(48, 0, _T("Arial"));
    outtextxy(WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 100, _T("GAME OVER"));

    settextcolor(WHITE);
    settextstyle(24, 0, _T("Arial"));

    TCHAR finalScore[100];
    _stprintf_s(finalScore, _T("Final Score: %d"), score);
    outtextxy(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 50, finalScore);

    TCHAR survivalTime[100];
    int minutes = (int)gameTime / 60;
    int seconds = (int)gameTime % 60;
    _stprintf_s(survivalTime, _T("Survived: %02d:%02d"), minutes, seconds);
    outtextxy(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 20, survivalTime);

    outtextxy(WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 + 50, _T("Press any key to exit"));

    FlushBatchDraw();
    _getch();
}