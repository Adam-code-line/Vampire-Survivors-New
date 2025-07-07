#include "EnhancedWeaponSystem.h"

EnhancedWeaponSystem::EnhancedWeaponSystem(Player* p,
    std::vector<std::unique_ptr<Bullet>>* b,
    std::vector<std::unique_ptr<MeleeAttack>>* m)
    : player(p), bullets(b), meleeAttacks(m),
    fireRate(2.0f), fireTimer(0),
    meleeRate(3.0f), meleeTimer(0), lastMeleeAngle(0) {
}

void EnhancedWeaponSystem::Update(float deltaTime, const std::vector<std::unique_ptr<Enemy>>& enemies) {
    fireTimer -= deltaTime;

    if (!enemies.empty()) {
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
            Vector2 direction = (closest->position - player->position).Normalized();

            // 远程攻击
            if (fireTimer <= 0) {
                bullets->push_back(std::make_unique<Bullet>(player->position, direction));
                fireTimer = 1.0f / fireRate;
            }
        }
    }
}

void EnhancedWeaponSystem::TriggerMeleeAttack(float angle) {
    // 空实现，防止链接错误
}

void EnhancedWeaponSystem::UpgradeFireRate(float multiplier) {
    fireRate *= multiplier;
}

void EnhancedWeaponSystem::UpgradeMeleeRate(float multiplier) {
    meleeRate *= multiplier;
}