#include "EnemySpawner.h"

EnemySpawner::EnemySpawner(Player* p) : player(p), spawnTimer(0), spawnRate(2.0f),
    angleDist(0, 2 * PI), gameTime(0), currentEnemyLevel(1) {
    rng.seed(std::random_device{}());
}

int EnemySpawner::CalculateEnemyLevel(float gameTime) {
    // 每60秒提升一个等级
    int baseLevel = (int)(gameTime / 70.0f) + 1;

    // 限制最大等级为4
    return min(baseLevel, 4);
}

void EnemySpawner::Update(float deltaTime, std::vector<std::unique_ptr<Enemy>>& enemies) {
    gameTime += deltaTime;
    currentEnemyLevel = CalculateEnemyLevel(gameTime);
    
    spawnTimer -= deltaTime;

    if (spawnTimer <= 0) {
        // 在屏幕边缘生成敌人
        float angle = angleDist(rng);
        float distance = 600; // 屏幕外距离

        Vector2 spawnPos = player->position +
            Vector2((float)cos(angle) * distance, (float)sin(angle) * distance);

        // 确保在合理范围内
        spawnPos.x = (float)max(-100.0f, min((float)(WINDOW_WIDTH + 100), spawnPos.x));
        spawnPos.y = (float)max(-100.0f, min((float)(WINDOW_HEIGHT + 100), spawnPos.y));

        // 生成指定等级的敌人
        enemies.push_back(std::make_unique<Enemy>(spawnPos, player, currentEnemyLevel));
        spawnTimer = 1.0f / spawnRate;

        // 随时间增加生成速度
        spawnRate += 0.0005f;
    }
}