#pragma once
#include "Player.h"
#include "Enemy.h"
#include "GameConfig.h"
#include <vector>
#include <memory>
#include <random>

class EnemySpawner {
private:
    Player* player;
    float spawnTimer;
    float spawnRate;
    std::mt19937 rng;
    std::uniform_real_distribution<float> angleDist;
    
    float gameTime;         // 新增：追踪游戏时间
    int currentEnemyLevel;  // 新增：当前敌人等级

public:
    EnemySpawner(Player* p);

    void Update(float deltaTime, std::vector<std::unique_ptr<Enemy>>& enemies);
    int CalculateEnemyLevel(float gameTime);  // 新增：计算敌人等级
    int GetCurrentEnemyLevel() const { return currentEnemyLevel; }  // 新增：获取当前等级
};
