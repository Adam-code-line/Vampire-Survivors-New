#pragma once
#include "Vector2.h"
#include <string>
#include <graphics.h>

// 道具类型
enum class ItemType {
    HEALTH_POTION,      // 血瓶
    SPEED_BOOST,        // 速度提升
    DAMAGE_UP,          // 攻击力提升
    FIRE_RATE_UP,       // 射速提升
    SHIELD,             // 护盾
    EXPERIENCE_BOOST,   // 经验加成
    AMMO_REFILL,        // 弹药补充
    MULTI_SHOT          // 多重射击
};

// 道具稀有度
enum class ItemRarity {
    COMMON,     // 普通 - 白色
    RARE,       // 稀有 - 蓝色
    EPIC,       // 史诗 - 紫色
    LEGENDARY   // 传说 - 金色
};

// 道具数据结构
struct ItemData {
    ItemType type;
    ItemRarity rarity;
    std::string name;
    std::string description;
    float effectValue;      // 效果数值
    float duration;         // 持续时间（0表示永久）
    COLORREF color;         // 显示颜色

    ItemData(ItemType t, ItemRarity r, const std::string& n, const std::string& desc,
        float value, float dur, COLORREF col)
        : type(t), rarity(r), name(n), description(desc),
        effectValue(value), duration(dur), color(col) {
    }
};
