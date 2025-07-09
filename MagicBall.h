#pragma once
#include "GameObject.h"
#include "GameConfig.h"
#include <graphics.h>
#include <cmath>  // 添加这行

#ifndef PI
#define PI 3.14159265358979323846  // 添加PI常量定义
#endif

class MagicBall : public GameObject {
private:
    float speed;
    float lifetime;
    float maxLifetime;
    int damage;
    float animationTimer;
    float animationSpeed;
    int currentFrame;
    bool useImageRendering;
    
    // 魔法球特效
    float rotationAngle;
    float rotationSpeed;
    float pulseTimer;
    float pulseSpeed;

public:
    MagicBall(Vector2 pos, Vector2 dir, float spd = 250.0f);

    void Update(float deltaTime) override;
    void Render() override;
    void RenderWithImage();
    void RenderWithGeometry();

    int GetDamage() const { return damage; }
    void SetDamage(int newDamage) { damage = newDamage; }
};
