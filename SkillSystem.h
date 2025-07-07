#pragma once
#include "Character.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "MeleeAttack.h"
#include <vector>
#include <memory>

class SkillSystem {
private:
    Player* player;
    std::vector<std::unique_ptr<Bullet>>* bullets;
    std::vector<std::unique_ptr<MeleeAttack>>* meleeAttacks;
    std::vector<std::unique_ptr<Enemy>>* enemies;

    SkillType currentSkill;
    float skillCooldown;
    float skillTimer;

public:
    SkillSystem(Player* p,
        std::vector<std::unique_ptr<Bullet>>* b,
        std::vector<std::unique_ptr<MeleeAttack>>* m,
        std::vector<std::unique_ptr<Enemy>>* e);

    void SetSkill(SkillType skill, float cooldown);
    void Update(float deltaTime);
    bool TriggerSkill();
    bool IsSkillReady() const { return skillTimer <= 0; }
    float GetSkillCooldownPercent() const;

private:
    void ExecuteBerserkerRage();
    void ExecuteFireball();
    void ExecuteMultiShot();
    void ExecuteShadowStrike();
};
