#pragma once
#include "GameObject.h"
#include "GameConfig.h"
#include "Player.h"
#include <graphics.h>
#include <string>
#include <memory>
#include <vector>

// 前向声明
class Bullet;
class MeleeAttack;

// 简化的4级怪物状态枚举
enum class Monster4State {
    MOVING,         // 移动状态
    MELEE_ATTACK,   // 近战攻击
    RANGE_ATTACK    // 远程攻击
};

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
    
    // 4级怪物专用变量（简化）
    Monster4State currentState;     // 当前状态
    float stateTimer;              // 状态计时器
    float attackCooldown;          // 攻击冷却
    float attackTimer;             // 攻击计时器
    float hoverOffset;             // 浮动偏移
    float hoverSpeed;              // 浮动速度
    bool attackExecuted;           // 攻击是否已执行
    
    // 引用游戏对象容器（用于生成子弹和攻击）
    std::vector<std::unique_ptr<Bullet>>* bullets;
    std::vector<std::unique_ptr<MeleeAttack>>* meleeAttacks;

public:
    Enemy(Vector2 pos, Player* player, int enemyLevel = 1);

    void Update(float deltaTime) override;
    void UpdateAnimation(float deltaTime);  // 更新动画
    void Render() override;

    int GetExperienceValue() const { return experienceValue; }
    int GetLevel() const { return level; }
    void SetLevel(int newLevel);
    
    // 4级怪物专用方法
    void SetGameContainers(std::vector<std::unique_ptr<Bullet>>* b, 
                          std::vector<std::unique_ptr<MeleeAttack>>* m);
    
private:
    void RenderWithImage();     // 使用图片渲染
    void RenderWithGeometry();  // 使用几何图形渲染
    void CheckImageResources(); // 检查图片资源
    
    // 4级怪物专用方法（简化）
    void UpdateMonster4(float deltaTime);    // 4级怪物更新逻辑
    void UpdateMonster4State(float deltaTime); // 更新4级怪物状态
    void PerformMeleeAttack();              // 执行近战攻击
    void PerformRangeAttack();              // 执行远程攻击
    void RenderMonster4();                  // 4级怪物渲染
};
