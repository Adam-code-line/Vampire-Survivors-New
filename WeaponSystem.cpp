#include "WeaponSystem.h"

WeaponSystem::WeaponSystem(Player* p, std::vector<std::unique_ptr<Bullet>>* b)
    : player(p), bullets(b), fireRate(2.0f), fireTimer(0) {
}

void WeaponSystem::Update(float deltaTime, const std::vector<std::unique_ptr<Enemy>>& enemies) {
    fireTimer -= deltaTime;

    if (fireTimer <= 0 && !enemies.empty()) {
        // 寻找最近的敌人
        Enemy* closest = nullptr;
        float closestDist = FLT_MAX;

        for (const auto& enemy : enemies) {
            if (enemy->active) {
                float dist = player->position.Distance(enemy->position);
                if (dist < closestDist) {
                    closestDist = dist;
                    closest = enemy.get();
                }
            }
        }

        if (closest) {
            // 向最近敌人射击
            Vector2 direction = (closest->position - player->position).Normalized();
            bullets->push_back(std::make_unique<Bullet>(player->position, direction));
            fireTimer = 1.0f / fireRate;
        }
    }
}