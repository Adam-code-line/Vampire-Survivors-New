#pragma once
#include "Vector2.h"
#include <string>
#include <graphics.h>

// ��������
enum class ItemType {
    HEALTH_POTION,      // Ѫƿ
    SPEED_BOOST,        // �ٶ�����
    DAMAGE_UP,          // ����������
    FIRE_RATE_UP,       // ��������
    SHIELD,             // ����
    EXPERIENCE_BOOST,   // ����ӳ�
    AMMO_REFILL,        // ��ҩ����
    MULTI_SHOT          // �������
};

// ����ϡ�ж�
enum class ItemRarity {
    COMMON,     // ��ͨ - ��ɫ
    RARE,       // ϡ�� - ��ɫ
    EPIC,       // ʷʫ - ��ɫ
    LEGENDARY   // ��˵ - ��ɫ
};

// �������ݽṹ
struct ItemData {
    ItemType type;
    ItemRarity rarity;
    std::string name;
    std::string description;
    float effectValue;      // Ч����ֵ
    float duration;         // ����ʱ�䣨0��ʾ���ã�
    COLORREF color;         // ��ʾ��ɫ

    ItemData(ItemType t, ItemRarity r, const std::string& n, const std::string& desc,
        float value, float dur, COLORREF col)
        : type(t), rarity(r), name(n), description(desc),
        effectValue(value), duration(dur), color(col) {
    }
};
