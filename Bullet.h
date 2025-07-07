#pragma once
#include "GameObject.h"
#include "GameConfig.h"
#include <graphics.h>

class Bullet : public GameObject {
private:
    float speed;
    float lifetime;
    float maxLifetime;
    int damage;

public:
    Bullet(Vector2 pos, Vector2 dir, float spd = 400.0f);

    void Update(float deltaTime) override;
    void Render() override;

    int GetDamage() const { return damage; }
};