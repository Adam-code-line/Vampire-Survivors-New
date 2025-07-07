#pragma once
#include "GameObject.h"
#include "GameConfig.h"
#include "Character.h"
#include <graphics.h>
#include <windows.h>
#include <algorithm>
#include <string>  // 添加这个头文件

class Player : public GameObject {
private:
    float speed;
    int level;
    int experience;
    int experienceToNext;
    float attackCooldown;
    float attackTimer;

    // 角色系统
    CharacterType characterType;
    COLORREF characterColor;

    // 图片渲染相关 - 新增
    bool useImageRendering;
    std::string characterImageKey;

    void LoadCharacterImage();  // 新增

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

    // Getters
    int GetLevel() const { return level; }
    int GetExperience() const { return experience; }
    int GetExperienceToNext() const { return experienceToNext; }
    CharacterType GetCharacterType() const { return characterType; }

    // 角色系统
    void SetCharacterType(CharacterType type);
    void ApplyCharacterStats(const CharacterData& data);
};