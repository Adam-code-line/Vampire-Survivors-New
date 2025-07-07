#pragma once
#include "Vector2.h"

// 游戏对象基类
class GameObject {
public:
    Vector2 position;
    Vector2 velocity;
    float radius;
    bool active;
    int health;
    int maxHealth;

    GameObject(Vector2 pos, float r)
        : position(pos), radius(r), active(true), health(1), maxHealth(1) {
    }

    virtual ~GameObject() = default;
    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;

    bool CheckCollision(const GameObject& other) const {
        return position.Distance(other.position) < (radius + other.radius);
    }

    void TakeDamage(int damage) {
        health -= damage;
        if (health <= 0) {
            active = false;
        }
    }
};