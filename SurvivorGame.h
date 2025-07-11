#pragma once
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "MeleeAttack.h"
#include "MagicBall.h"
#include "ExperienceGem.h"
#include "Item.h"
#include "WeaponSystem.h"
#include "EnemySpawner.h"
#include "SkillSystem.h"
#include "ItemSystem.h"
#include "GameConfig.h"
#include "Character.h"
#include "MainMenu.h"
#include "StartScreen.h"
#include "VictoryScreen.h"  // 新增
#include "MusicManager.h"
#include <graphics.h>
#include <vector>
#include <memory>
#include <windows.h>
#include <tchar.h>
#include <conio.h>
#include <algorithm>
#include "EnhancedWeaponSystem.h"

enum class GameResult {  // 新增
    CONTINUE,
    RETURN_TO_MENU,
    EXIT_GAME
};

class SurvivorGame {
private:
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Bullet>> bullets;
    std::vector<std::unique_ptr<MeleeAttack>> meleeAttacks;
    std::vector<std::unique_ptr<MagicBall>> magicBalls;
    std::vector<std::unique_ptr<ExperienceGem>> gems;
    std::vector<std::unique_ptr<Item>> items;
    std::unique_ptr<EnhancedWeaponSystem> weaponSystem;
    std::unique_ptr<EnemySpawner> spawner;
    std::unique_ptr<SkillSystem> skillSystem;
    std::unique_ptr<ItemSystem> itemSystem;

    float gameTime;
    bool gameOver;
    bool gameWon;  // 新增：游戏胜利标志
    int score;
    DWORD lastTime;

    static const float WIN_TIME; // 新增：通关时间常量

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
    GameResult ShowVictory();  // 修改返回类型
    GameResult ShowEndGameChoice();  // 修改返回类型
    void DrawEnemyLevelInfo();
    void DrawSkillUI();
    void DrawItemEffects();
    CharacterData GetCharacterData(CharacterType type);
};
