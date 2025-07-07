#pragma once
#include "Player.h"
#include "Bullet.h"
#include "MeleeAttack.h"
#include "Enemy.h"
#include "Character.h"
#include <vector>
#include <memory>

// 剑气攻击类
class SwordAura : public GameObject {
private:
    Player* owner;
    float damage;
    float maxLifetime;
    float lifetime;
    std::string auraImageKey;
    float scale; // 缩放因子（随等级增加）
    Vector2 direction;
    float speed;
    
public:
    SwordAura(Vector2 pos, Player* player, Vector2 dir, float playerLevel);
    void Update(float deltaTime) override;
    void Render() override;
    bool CheckCollision(const GameObject& other) const;
    float GetDamage() const { return damage; }
    
private:
    std::string GetAuraImageKey(Vector2 dir);
};

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
    std::vector<std::unique_ptr<SwordAura>> swordAuras; // 剑气容器
    
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
    void Render(); // 渲染剑气
    void UpgradeFireRate(float multiplier);
    void UpgradeMeleeRate(float multiplier);
    void UpgradeSlashRate(float multiplier);
    
    // 剑气碰撞检测
    void CheckSwordAuraCollisions(std::vector<std::unique_ptr<Enemy>>& enemies);
    
private:
    void HandleWarriorSlash(const std::vector<std::unique_ptr<Enemy>>& enemies);
    void HandleOtherCharacterAttacks(const std::vector<std::unique_ptr<Enemy>>& enemies);
    Vector2 FindNearestEnemy(const std::vector<std::unique_ptr<Enemy>>& enemies);
    void CreateSwordAura(Vector2 direction);
};
