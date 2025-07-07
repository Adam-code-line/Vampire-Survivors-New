#pragma once
#include "Player.h"
#include "Bullet.h"
#include "MeleeAttack.h"
#include "Enemy.h"
#include <vector>
#include <memory>
#include <cmath>
#include <cfloat>

class EnhancedWeaponSystem {
private:
    Player* player;
    std::vector<std::unique_ptr<Bullet>>* bullets;
    std::vector<std::unique_ptr<MeleeAttack>>* meleeAttacks;

    float fireRate;
    float fireTimer;

    float meleeRate;
    float meleeTimer;
    float lastMeleeAngle;

public:
    EnhancedWeaponSystem(Player* p,
        std::vector<std::unique_ptr<Bullet>>* b,
        std::vector<std::unique_ptr<MeleeAttack>>* m);

    void Update(float deltaTime, const std::vector<std::unique_ptr<Enemy>>& enemies);
    void TriggerMeleeAttack(float angle);
    void UpgradeFireRate(float multiplier = 1.2f);
    void UpgradeMeleeRate(float multiplier = 1.2f);
};
