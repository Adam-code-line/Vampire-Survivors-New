#pragma once
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include <vector>
#include <memory>
#include <cfloat>

class WeaponSystem {
private:
    Player* player;
    std::vector<std::unique_ptr<Bullet>>* bullets;
    float fireRate;
    float fireTimer;

public:
    WeaponSystem(Player* p, std::vector<std::unique_ptr<Bullet>>* b);

    void Update(float deltaTime, const std::vector<std::unique_ptr<Enemy>>& enemies);
};
