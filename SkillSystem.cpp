#include "SkillSystem.h"
#include "GameConfig.h"
#include <cmath>

SkillSystem::SkillSystem(Player* p,
    std::vector<std::unique_ptr<Bullet>>* b,
    std::vector<std::unique_ptr<MeleeAttack>>* m,
    std::vector<std::unique_ptr<Enemy>>* e)
    : player(p), bullets(b), meleeAttacks(m), enemies(e),
    currentSkill(SkillType::BERSERKER_RAGE), skillCooldown(5.0f), skillTimer(0) {
}

void SkillSystem::SetSkill(SkillType skill, float cooldown) {
    currentSkill = skill;
    skillCooldown = cooldown;
    skillTimer = 0;
}

void SkillSystem::Update(float deltaTime) {
    if (skillTimer > 0) {
        skillTimer -= deltaTime;
    }
}

bool SkillSystem::TriggerSkill() {
    if (!IsSkillReady()) return false;

    skillTimer = skillCooldown;

    switch (currentSkill) {
    case SkillType::BERSERKER_RAGE:
        ExecuteBerserkerRage();
        break;
    case SkillType::FIREBALL:
        ExecuteFireball();
        break;
    case SkillType::MULTI_SHOT:
        ExecuteMultiShot();
        break;
    case SkillType::SHADOW_STRIKE:
        ExecuteShadowStrike();
        break;
    }

    return true;
}

float SkillSystem::GetSkillCooldownPercent() const {
    if (skillCooldown <= 0) return 1.0f;
    return 1.0f - (skillTimer / skillCooldown);
}

void SkillSystem::ExecuteBerserkerRage() {
    // 战士技能：狂暴斩击 - 范围随等级增大
    float rageRange = 120.0f + player->GetLevel() * 2.0f; // 等级越高范围越大
    int rageDamage = 50 + player->GetLevel() * 2; // 等级越高伤害越高
    
    // 创建8个方向的强化斩击
    for (int i = 0; i < 8; i++) {
        float angle = (PI * 2 * i) / 8;
        
        // 创建技能斩击
        auto skillSlash = std::make_unique<MeleeAttack>(player->position, player, angle, rageRange);
        skillSlash->SetDamage(rageDamage);
        meleeAttacks->push_back(std::move(skillSlash));
    }
}

void SkillSystem::ExecuteFireball() {
    // 法师技能：发射火球
    if (enemies->empty()) return;

    // 找到最近的敌人
    Enemy* target = nullptr;
    float minDist = FLT_MAX;
    for (auto& enemy : *enemies) {
        if (enemy->active) {
            float dist = player->position.Distance(enemy->position);
            if (dist < minDist) {
                minDist = dist;
                target = enemy.get();
            }
        }
    }

    if (target) {
        Vector2 direction = (target->position - player->position).Normalized();
        // 发射3个火球，稍微分散
        for (int i = -1; i <= 1; i++) {
            float angle = atan2(direction.y, direction.x) + i * 0.3f;
            Vector2 fireballDir(cos(angle), sin(angle));
            bullets->push_back(std::make_unique<Bullet>(player->position, fireballDir, 600.0f));
        }
    }
}

void SkillSystem::ExecuteMultiShot() {
    // 弓箭手技能：向8个方向射箭
    for (int i = 0; i < 8; i++) {
        float angle = (2 * PI * i) / 8;
        Vector2 direction(cos(angle), sin(angle));
        bullets->push_back(std::make_unique<Bullet>(player->position, direction, 500.0f));
    }
}

void SkillSystem::ExecuteShadowStrike() {
    // 刺客技能：瞬移到最近敌人并攻击
    if (enemies->empty()) return;

    // 找到最近的敌人
    Enemy* target = nullptr;
    float minDist = FLT_MAX;
    for (auto& enemy : *enemies) {
        if (enemy->active) {
            float dist = player->position.Distance(enemy->position);
            if (dist < minDist && dist < 200.0f) { // 限制瞬移距离
                minDist = dist;
                target = enemy.get();
            }
        }
    }

    if (target) {
        // 瞬移到敌人附近
        Vector2 direction = (player->position - target->position).Normalized();
        player->position = target->position + direction * 40.0f;

        // 对周围所有敌人攻击
        float angle = atan2(-direction.y, -direction.x);
        int shadowDamage = 80 + player->GetLevel() * 15; // 刺客技能伤害
        auto shadowStrike = std::make_unique<MeleeAttack>(player->position, player, angle, 80.0f);
        shadowStrike->SetDamage(shadowDamage);
        meleeAttacks->push_back(std::move(shadowStrike));
    }
}