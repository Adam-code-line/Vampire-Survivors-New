#pragma once
#include "GameObject.h"
#include "GameConfig.h"
#include "Player.h"
#include <graphics.h>
#include <string>

class Enemy : public GameObject {
private:
    float speed;
    Player* target;
    int experienceValue;
    int level;          // 敌人等级
    float sizeMultiplier; // 大小倍数
    
    // 动画相关变量
    float animationTimer;    // 动画计时器
    float animationSpeed;    // 动画速度
    int currentFrame;        // 当前动画帧
    bool facingRight;        // 朝向（true=右，false=左）
    bool isMoving;          // 是否在移动
    bool useImageRendering; // 是否使用图片渲染

public:
    Enemy(Vector2 pos, Player* player, int enemyLevel = 1);

    void Update(float deltaTime) override;
    void UpdateAnimation(float deltaTime);  // 更新动画
    void Render() override;

    int GetExperienceValue() const { return experienceValue; }
    int GetLevel() const { return level; }
    void SetLevel(int newLevel);
    
private:
    void RenderWithImage();     // 使用图片渲染
    void RenderWithGeometry();  // 使用几何图形渲染
    void CheckImageResources(); // 新增：检查图片资源
};
