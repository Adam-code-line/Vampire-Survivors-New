#include "EnhancedWeaponSystem.h"
#include "ImageManager.h"
#include <cmath>

// SwordAura implementation
SwordAura::SwordAura(Vector2 pos, Player* player, Vector2 dir, float playerLevel)
    : GameObject(pos, 20.0f), owner(player), direction(dir), lifetime(0), maxLifetime(2.0f), speed(250.0f) {
    damage = 40 + playerLevel * 8; // 基础伤害 + 等级加成
    scale = 1.0f + playerLevel * 0.2f; // 随等级增大范围
    radius = 20.0f * scale; // 碰撞半径也随等级增加
    auraImageKey = GetAuraImageKey(dir);
    active = true;
}

std::string SwordAura::GetAuraImageKey(Vector2 dir) {
    // 根据方向选择对应的剑气图片
    if (abs(dir.y) > abs(dir.x)) {
        // 垂直方向优先
        if (dir.y < 0) return "attack_up";
        else return "attack_down";
    } else {
        // 水平方向优先
        if (dir.x > 0) return "attack_right";
        else return "attack_left";
    }
}

void SwordAura::Update(float deltaTime) {
    lifetime += deltaTime;
    
    // 剑气向前飞行
    position = position + direction * speed * deltaTime;
    
    // 超时或超出边界则消失
    if (lifetime >= maxLifetime || 
        position.x < -50 || position.x > 1250 || 
        position.y < -50 || position.y > 850) {
        active = false;
    }
}

void SwordAura::Render() {
    ImageManager* imgManager = ImageManager::GetInstance();
    IMAGE* auraImg = imgManager->GetImage(auraImageKey);
    
    if (auraImg) {
        // 计算绘制位置（考虑缩放）
        int scaledWidth = (int)(auraImg->getwidth() * scale);
        int scaledHeight = (int)(auraImg->getheight() * scale);
        
        int drawX = (int)position.x - scaledWidth / 2;
        int drawY = (int)position.y - scaledHeight / 2;
        
        // 使用缩放绘制
        imgManager->DrawImageScaled(auraImg, drawX, drawY, scale);
    } else {
        // 备用绘制：如果图片加载失败，使用几何图形
        setfillcolor(RGB(100, 200, 255)); // 蓝色剑气效果
        fillcircle((int)position.x, (int)position.y, (int)(radius));
    }
}

bool SwordAura::CheckCollision(const GameObject& other) const {
    float distance = (position - other.position).Length();
    return distance < (radius + other.radius);
}

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
    
    // 更新剑气
    for (auto& aura : swordAuras) {
        aura->Update(deltaTime);
    }
    
    // 清理失效的剑气
    swordAuras.erase(
        std::remove_if(swordAuras.begin(), swordAuras.end(),
            [](const std::unique_ptr<SwordAura>& aura) {
                return !aura->active;
            }),
        swordAuras.end()
    );
    
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
            direction = direction.Normalized();
            float angle = atan2(direction.y, direction.x);
            
            // 斩击范围随等级增加
            float slashRange = 80.0f + player->GetLevel() * 10.0f;
            
            // 触发玩家的斩击动画
            player->TriggerSlashAnimation();
            
            // 创建近身斩击攻击
            meleeAttacks->push_back(std::make_unique<SlashAttack>(
                player->position, player, angle, slashRange, true));
            
            // 创建剑气
            CreateSwordAura(direction);
            
            slashTimer = 0;
        }
    }
}

void EnhancedWeaponSystem::CreateSwordAura(Vector2 direction) {
    // 在玩家前方一定距离创建剑气
    Vector2 auraPos = player->position + direction * 40.0f;
    swordAuras.push_back(std::make_unique<SwordAura>(auraPos, player, direction, player->GetLevel()));
}

void EnhancedWeaponSystem::Render() {
    // 渲染所有剑气
    for (auto& aura : swordAuras) {
        aura->Render();
    }
}

void EnhancedWeaponSystem::CheckSwordAuraCollisions(std::vector<std::unique_ptr<Enemy>>& enemies) {
    for (auto& aura : swordAuras) {
        if (!aura->active) continue;
        
        for (auto& enemy : enemies) {
            if (!enemy->active) continue;
            
            if (aura->CheckCollision(*enemy)) {
                enemy->TakeDamage(aura->GetDamage());
                aura->active = false; // 剑气击中后消失
                break; // 一个剑气只能击中一个敌人
            }
        }
    }
}

void EnhancedWeaponSystem::HandleOtherCharacterAttacks(const std::vector<std::unique_ptr<Enemy>>& enemies) {
    // 其他角色的原有攻击逻辑
    if (fireTimer >= 1.0f / fireRate && !enemies.empty()) {
        Vector2 nearestEnemyPos = FindNearestEnemy(enemies);
        if (nearestEnemyPos.x != -1) {
            Vector2 direction = nearestEnemyPos - player->position;
            direction = direction.Normalized();

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