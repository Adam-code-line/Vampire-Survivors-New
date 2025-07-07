#pragma once
#include "GameObject.h"
#include "GameConfig.h"
#include "Player.h"
#include <graphics.h>

class Enemy : public GameObject {
private:
    float speed;
    Player* target;
    int experienceValue;
    int level;          // 新增：敌人等级
    float sizeMultiplier; // 新增：大小倍数

public:
    Enemy(Vector2 pos, Player* player, int enemyLevel = 1);

    void Update(float deltaTime) override;
    void Render() override;

    int GetExperienceValue() const { return experienceValue; }
    int GetLevel() const { return level; }  // 新增：获取等级
    void SetLevel(int newLevel);            // 新增：设置等级
};
