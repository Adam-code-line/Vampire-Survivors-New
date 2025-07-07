#pragma once
#include "GameObject.h"
#include "GameConfig.h"
#include "Character.h"
#include <graphics.h>
#include <windows.h>
#include <algorithm>
#include <string>

class Player : public GameObject {
private:
    CharacterType characterType;
    std::string characterImageKey;
    bool useImageRendering;
    COLORREF characterColor;
    
    float speed;
    int level;
    int experience;
    int experienceToNext;
    float attackCooldown;
    float attackTimer;
    
    // 斩击动画相关
    float slashAnimationTimer;
    float slashAnimationDuration;
    
    // 添加方向状态
    bool facingRight;
    Vector2 lastMoveDirection;

public:
    Player(Vector2 pos, CharacterType type = CharacterType::WARRIOR);

    void Update(float deltaTime) override;
    void HandleInput(float deltaTime);
    void Render() override;
    void DrawHealthBar();

    bool CanAttack() const;
    void Attack();
    void GainExperience(int exp);
    void LevelUp();

    // 添加触发斩击动画的方法
    void TriggerSlashAnimation() {
        slashAnimationTimer = slashAnimationDuration;
    }

    // Getters
    int GetLevel() const { return level; }
    int GetExperience() const { return experience; }
    int GetExperienceToNext() const { return experienceToNext; }
    CharacterType GetCharacterType() const { return characterType; }
    bool IsFacingRight() const { return facingRight; }

    // 角色系统
    void SetCharacterType(CharacterType type);
    void ApplyCharacterStats(const CharacterData& data);
    
    // 添加缺失的方法声明
    void LoadCharacterImage();
};