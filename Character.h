#pragma once
#include "Vector2.h"
#include <string>

// 角色类型枚举
enum class CharacterType {
    WARRIOR,    // 战士
    MAGE,       // 法师
    ARCHER,     // 弓箭手
    ASSASSIN    // 刺客
};

// 角色技能类型
enum class SkillType {
    BERSERKER_RAGE,     // 狂战士之怒
    FIREBALL,          // 火球术
    MULTI_SHOT,        // 多重射击
    SHADOW_STRIKE      // 暗影突袭
};

// 角色数据结构
struct CharacterData {
    CharacterType type;
    std::string name;
    std::string description;

    // 基础属性
    float speed;
    int health;
    float attackSpeed;
    int damage;

    // 技能
    SkillType skill;
    float skillCooldown;

    CharacterData(CharacterType t, const std::string& n, const std::string& desc,
        float spd, int hp, float atkSpd, int dmg, SkillType sk, float cooldown)
        : type(t), name(n), description(desc), speed(spd), health(hp),
        attackSpeed(atkSpd), damage(dmg), skill(sk), skillCooldown(cooldown) {
    }
};
