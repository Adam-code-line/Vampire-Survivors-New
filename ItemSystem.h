#pragma once
#include "Item.h"
#include "Player.h"
#include "Item.h"
#include "Item2.h"
#include <vector>
#include <memory>
#include <random>

// 道具效果结构
struct ItemEffect {
    ItemType type;
    float value;
    float duration;
    float remainingTime;

    ItemEffect(ItemType t, float v, float d)
        : type(t), value(v), duration(d), remainingTime(d) {
    }
};

class ItemSystem {
private:
    Player* player;
    std::vector<std::unique_ptr<Item>>* items;
    std::vector<ItemEffect> activeEffects;
    std::mt19937 rng;
    std::uniform_real_distribution<float> dropChance;
    std::uniform_int_distribution<int> itemTypeDist;
    std::uniform_int_distribution<int> rarityDist;

public:
    ItemSystem(Player* p, std::vector<std::unique_ptr<Item>>* itemList);

    void Update(float deltaTime);
    void SpawnItem(Vector2 position);
    bool CollectItem(Item* item);
    void ApplyItemEffect(const ItemData& itemData);
    void UpdateEffects(float deltaTime);

    const std::vector<ItemEffect>& GetActiveEffects() const { return activeEffects; }

private:
    ItemData GenerateRandomItem();
    ItemRarity GenerateRarity();
    COLORREF GetItemColor(ItemType type);
};
