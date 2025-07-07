#pragma once
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "MeleeAttack.h"
#include "ExperienceGem.h"
#include "Item.h"  // 新增
#include "WeaponSystem.h"
#include "EnemySpawner.h"
#include "SkillSystem.h"
#include "ItemSystem.h"  // 新增
#include "GameConfig.h"
#include "Character.h"
#include "MainMenu.h"
#include <graphics.h>
#include <vector>
#include <memory>
#include <windows.h>
#include <tchar.h>
#include <conio.h>
#include <algorithm>
#include "EnhancedWeaponSystem.h"

class SurvivorGame {
private:
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Bullet>> bullets;
    std::vector<std::unique_ptr<MeleeAttack>> meleeAttacks;
    std::vector<std::unique_ptr<ExperienceGem>> gems;
    std::vector<std::unique_ptr<Item>> items;  // 新增：道具列表
    std::unique_ptr<EnhancedWeaponSystem> weaponSystem;
    std::unique_ptr<EnemySpawner> spawner;
    std::unique_ptr<SkillSystem> skillSystem;
    std::unique_ptr<ItemSystem> itemSystem;  // 新增：道具系统

    float gameTime;
    bool gameOver;
    int score;
    DWORD lastTime;
    
    CharacterType selectedCharacter;

public:
    SurvivorGame();
    void Run();

private:
    CharacterType ShowCharacterSelection();
    void InitializeGame(CharacterType characterType);
    void Update();
    void HandleCollisions();
    void CleanupObjects();
    void Render();
    void DrawUI();
    void ShowGameOver();
    void DrawEnemyLevelInfo();
    void DrawSkillUI();
    void DrawItemEffects();  // 新增：显示道具效果
    CharacterData GetCharacterData(CharacterType type);
};
