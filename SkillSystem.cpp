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
    // սʿ���ܣ�360�Ƚ�ս����
    for (int i = 0; i < 12; i++) {
        float angle = (2 * PI * i) / 12;
        meleeAttacks->push_back(std::make_unique<MeleeAttack>(
            player->position, player, angle, 100.0f));
    }
}

void SkillSystem::ExecuteFireball() {
    // ��ʦ���ܣ�����������
    if (enemies->empty()) return;

    // �ҵ�����ĵ���
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
        // ����3��������΢��ɢ
        for (int i = -1; i <= 1; i++) {
            float angle = atan2(direction.y, direction.x) + i * 0.3f;
            Vector2 fireballDir(cos(angle), sin(angle));
            bullets->push_back(std::make_unique<Bullet>(player->position, fireballDir, 600.0f));
        }
    }
}

void SkillSystem::ExecuteMultiShot() {
    // �����ּ��ܣ���8���������
    for (int i = 0; i < 8; i++) {
        float angle = (2 * PI * i) / 8;
        Vector2 direction(cos(angle), sin(angle));
        bullets->push_back(std::make_unique<Bullet>(player->position, direction, 500.0f));
    }
}

void SkillSystem::ExecuteShadowStrike() {
    // �̿ͼ��ܣ�˲�Ƶ�������߲�����
    if (enemies->empty()) return;

    // �ҵ�����ĵ���
    Enemy* target = nullptr;
    float minDist = FLT_MAX;
    for (auto& enemy : *enemies) {
        if (enemy->active) {
            float dist = player->position.Distance(enemy->position);
            if (dist < minDist && dist < 200.0f) { // ����˲�ƾ���
                minDist = dist;
                target = enemy.get();
            }
        }
    }

    if (target) {
        // ˲�Ƶ����˸���
        Vector2 direction = (player->position - target->position).Normalized();
        player->position = target->position + direction * 40.0f;

        // �������н�ս����
        float angle = atan2(-direction.y, -direction.x);
        meleeAttacks->push_back(std::make_unique<MeleeAttack>(
            player->position, player, angle, 80.0f));
    }
}