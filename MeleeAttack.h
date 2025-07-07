#pragma once
#include "GameObject.h"
#include "GameConfig.h"
#include "Player.h"
#include <graphics.h>
#include <cmath>

class MeleeAttack : public GameObject {
protected:
    float lifetime;
    float maxLifetime;
    int damage;
    Player* owner;
    float angle;
    float range;

public:
    MeleeAttack(Vector2 pos, Player* player, float attackAngle, float attackRange = 60.0f);

    virtual void Update(float deltaTime);
    virtual void Render();

    bool IsInAttackRange(const Vector2& targetPos) const;
    int GetDamage() const { return damage; }
    void SetDamage(int newDamage) { damage = newDamage; } // 添加SetDamage方法
    Player* GetOwner() const { return owner; }
};
