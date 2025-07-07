#pragma once
#include "Player.h"
#include "Bullet.h"
#include "MeleeAttack.h"
#include "Enemy.h"
#include "Character.h"
#include <vector>
#include <memory>

class SlashAttack : public MeleeAttack {
private:
    int currentFrame;
    float frameTime;
    float maxFrameTime;
    bool isWarriorSlash;
    
public:
    SlashAttack(Vector2 pos, Player* player, float attackAngle, float attackRange, bool warrior = false);
    void Update(float deltaTime) override;
    void Render() override;
    int GetCurrentFrame() const { return currentFrame; }
};

class EnhancedWeaponSystem {
private:
    Player* player;
    std::vector<std::unique_ptr<Bullet>>* bullets;
    std::vector<std::unique_ptr<MeleeAttack>>* meleeAttacks;
    
    float fireTimer;
    float meleeTimer;
    float fireRate;
    float meleeRate;
    
    // 战士斩击相关
    float slashCooldown;
    float slashTimer;
    
public:
    EnhancedWeaponSystem(Player* p, 
                        std::vector<std::unique_ptr<Bullet>>* b,
                        std::vector<std::unique_ptr<MeleeAttack>>* m);
    
    void Update(float deltaTime, const std::vector<std::unique_ptr<Enemy>>& enemies);
    void UpgradeFireRate(float multiplier);
    void UpgradeMeleeRate(float multiplier);
    void UpgradeSlashRate(float multiplier);
    
private:
    void HandleWarriorSlash(const std::vector<std::unique_ptr<Enemy>>& enemies);
    void HandleOtherCharacterAttacks(const std::vector<std::unique_ptr<Enemy>>& enemies);
    Vector2 FindNearestEnemy(const std::vector<std::unique_ptr<Enemy>>& enemies);
};
