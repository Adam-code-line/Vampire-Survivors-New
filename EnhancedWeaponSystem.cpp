#include "EnhancedWeaponSystem.h"
#include "ImageManager.h"
#include <cmath>

// SlashAttack implementation
SlashAttack::SlashAttack(Vector2 pos, Player* player, float attackAngle, float attackRange, bool warrior)
    : MeleeAttack(pos, player, attackAngle, attackRange), 
      currentFrame(0), frameTime(0), maxFrameTime(0.1f), isWarriorSlash(warrior) {
    maxLifetime = 0.3f; // 增加持续时间以显示完整动画
    damage = 30 + player->GetLevel() * 5; // 伤害随等级增加
}

void SlashAttack::Update(float deltaTime) {
    lifetime += deltaTime;
    frameTime += deltaTime;
    
    // 动画帧切换
    if (frameTime >= maxFrameTime) {
        frameTime = 0;
        currentFrame++;
        if (currentFrame >= 3) {
            currentFrame = 2; // 保持在最后一帧
        }
    }
    
    if (lifetime >= maxLifetime) {
        active = false;
    }
    
    // 跟随玩家位置
    position = owner->position;
}

void SlashAttack::Render() {
    if (!isWarriorSlash) {
        // 非战士角色使用原有的扇形渲染
        MeleeAttack::Render();
        return;
    }
    
    // 战士的斩击动画完全由Player类处理，这里不再绘制任何视觉效果
    // 只保留伤害判定功能，不进行任何渲染
    
    // 可选：绘制一个很小的攻击范围指示器用于调试（注释掉以避免重复渲染）
    /*
    setlinecolor(RGB(255, 0, 0));
    setlinestyle(PS_DOT, 1);
    circle((int)position.x, (int)position.y, (int)range);
    */
}

// EnhancedWeaponSystem implementation
EnhancedWeaponSystem::EnhancedWeaponSystem(Player* p, 
                                          std::vector<std::unique_ptr<Bullet>>* b,
                                          std::vector<std::unique_ptr<MeleeAttack>>* m)
    : player(p), bullets(b), meleeAttacks(m), 
      fireTimer(0), meleeTimer(0), fireRate(1.0f), meleeRate(2.0f),
      slashCooldown(1.5f), slashTimer(0) {
}

void EnhancedWeaponSystem::Update(float deltaTime, const std::vector<std::unique_ptr<Enemy>>& enemies) {
    fireTimer += deltaTime;
    meleeTimer += deltaTime;
    slashTimer += deltaTime;
    
    if (player->GetCharacterType() == CharacterType::WARRIOR) {
        HandleWarriorSlash(enemies);
    } else {
        HandleOtherCharacterAttacks(enemies);
    }
}

void EnhancedWeaponSystem::HandleWarriorSlash(const std::vector<std::unique_ptr<Enemy>>& enemies) {
    // 战士只使用斩击攻击，移除远程攻击
    float adjustedSlashRate = slashCooldown / (1.0f + player->GetLevel() * 0.1f); // 等级提高攻击速度
    
    if (slashTimer >= adjustedSlashRate && !enemies.empty()) {
        Vector2 nearestEnemyPos = FindNearestEnemy(enemies);
        if (nearestEnemyPos.x != -1) {
            Vector2 direction = nearestEnemyPos - player->position;
            float angle = atan2(direction.y, direction.x);
            
            // 斩击范围随等级增加
            float slashRange = 80.0f + player->GetLevel() * 10.0f;
            
            // 触发玩家的斩击动画
            player->TriggerSlashAnimation();
            
            meleeAttacks->push_back(std::make_unique<SlashAttack>(
                player->position, player, angle, slashRange, true));
            slashTimer = 0;
        }
    }
}

void EnhancedWeaponSystem::HandleOtherCharacterAttacks(const std::vector<std::unique_ptr<Enemy>>& enemies) {
    // 其他角色的原有攻击逻辑
    if (fireTimer >= 1.0f / fireRate && !enemies.empty()) {
        Vector2 nearestEnemyPos = FindNearestEnemy(enemies);
        if (nearestEnemyPos.x != -1) {
            Vector2 direction = nearestEnemyPos - player->position;
            // 修复 Normalize 方法调用
            direction = direction.Normalized();

            // 修复：只传递3个参数，移除第4个参数（伤害值25）
            bullets->push_back(std::make_unique<Bullet>(
                player->position, direction, 300.0f));
            fireTimer = 0;
        }
    }

    if (meleeTimer >= 1.0f / meleeRate && !enemies.empty()) {
        Vector2 nearestEnemyPos = FindNearestEnemy(enemies);
        if (nearestEnemyPos.x != -1) {
            Vector2 direction = nearestEnemyPos - player->position;
            float angle = atan2(direction.y, direction.x);

            meleeAttacks->push_back(std::make_unique<MeleeAttack>(
                player->position, player, angle));
            meleeTimer = 0;
        }
    }
}

Vector2 EnhancedWeaponSystem::FindNearestEnemy(const std::vector<std::unique_ptr<Enemy>>& enemies) {
    Vector2 nearestPos(-1, -1);
    float nearestDistance = 200.0f; // 攻击范围

    for (const auto& enemy : enemies) {
        if (enemy->active) {
            float distance = (enemy->position - player->position).Length();
            if (distance < nearestDistance) {
                nearestDistance = distance;
                nearestPos = enemy->position;
            }
        }
    }
    return nearestPos;
}

void EnhancedWeaponSystem::UpgradeFireRate(float multiplier) {
    fireRate *= multiplier;
}

void EnhancedWeaponSystem::UpgradeMeleeRate(float multiplier) {
    meleeRate *= multiplier;
}

void EnhancedWeaponSystem::UpgradeSlashRate(float multiplier) {
    if (slashCooldown > 0.3f) { // 最小冷却时间限制
        slashCooldown /= multiplier;
    }
}