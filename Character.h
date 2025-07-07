#pragma once
#include "Vector2.h"
#include <string>

// ��ɫ����ö��
enum class CharacterType {
    WARRIOR,    // սʿ
    MAGE,       // ��ʦ
    ARCHER,     // ������
    ASSASSIN    // �̿�
};

// ��ɫ��������
enum class SkillType {
    BERSERKER_RAGE,     // ��սʿ֮ŭ
    FIREBALL,          // ������
    MULTI_SHOT,        // �������
    SHADOW_STRIKE      // ��ӰͻϮ
};

// ��ɫ���ݽṹ
struct CharacterData {
    CharacterType type;
    std::string name;
    std::string description;

    // ��������
    float speed;
    int health;
    float attackSpeed;
    int damage;

    // ����
    SkillType skill;
    float skillCooldown;

    CharacterData(CharacterType t, const std::string& n, const std::string& desc,
        float spd, int hp, float atkSpd, int dmg, SkillType sk, float cooldown)
        : type(t), name(n), description(desc), speed(spd), health(hp),
        attackSpeed(atkSpd), damage(dmg), skill(sk), skillCooldown(cooldown) {
    }
};
