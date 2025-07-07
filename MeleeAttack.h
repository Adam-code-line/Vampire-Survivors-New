#pragma once
#include "GameObject.h"
#include "GameConfig.h"
#include "Player.h"
#include <graphics.h>
#include <cmath>

class MeleeAttack : public GameObject {
private:
    float lifetime;
    float maxLifetime;
    int damage;
    Player* owner;
    float angle;
    float range;

public:
    MeleeAttack(Vector2 pos, Player* player, float attackAngle, float attackRange = 60.0f);

    void Update(float deltaTime) override;
    void Render() override;

    bool IsInAttackRange(const Vector2& targetPos) const;
    int GetDamage() const { return damage; }
    Player* GetOwner() const { return owner; }
};
