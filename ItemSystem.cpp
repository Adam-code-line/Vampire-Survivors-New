#include "ItemSystem.h"

ItemSystem::ItemSystem(Player* p, std::vector<std::unique_ptr<Item>>* itemList)
    : player(p), items(itemList), dropChance(0.0f, 1.0f),
    itemTypeDist(0, 7), rarityDist(0, 100) {
    rng.seed(std::random_device{}());
}

void ItemSystem::Update(float deltaTime) {
    UpdateEffects(deltaTime);
}

void ItemSystem::SpawnItem(Vector2 position) {
    // 30% 概率掉落道具
    if (dropChance(rng) < 0.3f) {
        ItemData itemData = GenerateRandomItem();
        items->push_back(std::make_unique<Item>(position, itemData));
    }
}

bool ItemSystem::CollectItem(Item* item) {
    if (item && item->active) {
        ApplyItemEffect(item->GetItemData());
        item->active = false;
        return true;
    }
    return false;
}

void ItemSystem::ApplyItemEffect(const ItemData& itemData) {
    switch (itemData.type) {
    case ItemType::HEALTH_POTION:
        player->health = min(player->maxHealth,
            player->health + (int)itemData.effectValue);
        break;

    case ItemType::SPEED_BOOST:
    case ItemType::DAMAGE_UP:
    case ItemType::FIRE_RATE_UP:
    case ItemType::EXPERIENCE_BOOST:
        // 添加临时效果
        activeEffects.emplace_back(itemData.type, itemData.effectValue, itemData.duration);
        break;

    case ItemType::SHIELD:
        // 临时护盾效果
        activeEffects.emplace_back(itemData.type, itemData.effectValue, itemData.duration);
        break;

    case ItemType::AMMO_REFILL:
        // 立即效果：重置攻击冷却
        // 这里可以调用武器系统的相关方法
        break;

    case ItemType::MULTI_SHOT:
        activeEffects.emplace_back(itemData.type, itemData.effectValue, itemData.duration);
        break;
    }
}

void ItemSystem::UpdateEffects(float deltaTime) {
    for (auto it = activeEffects.begin(); it != activeEffects.end();) {
        it->remainingTime -= deltaTime;

        if (it->remainingTime <= 0) {
            it = activeEffects.erase(it);
        }
        else {
            ++it;
        }
    }
}

ItemData ItemSystem::GenerateRandomItem() {
    ItemType type = static_cast<ItemType>(itemTypeDist(rng));
    ItemRarity rarity = GenerateRarity();

    std::string name, description;
    float effectValue, duration;
    COLORREF color = GetItemColor(type);

    switch (type) {
    case ItemType::HEALTH_POTION:
        name = "Health Potion";
        description = "Restores health";
        effectValue = 30.0f + (int)rarity * 20.0f;
        duration = 0;
        break;

    case ItemType::SPEED_BOOST:
        name = "Speed Boost";
        description = "Increases movement speed";
        effectValue = 1.2f + (int)rarity * 0.2f;
        duration = 10.0f + (int)rarity * 5.0f;
        break;

    case ItemType::DAMAGE_UP:
        name = "Damage Boost";
        description = "Increases attack damage";
        effectValue = 1.3f + (int)rarity * 0.3f;
        duration = 15.0f + (int)rarity * 5.0f;
        break;

    case ItemType::FIRE_RATE_UP:
        name = "Fire Rate Up";
        description = "Increases attack speed";
        effectValue = 1.5f + (int)rarity * 0.5f;
        duration = 12.0f + (int)rarity * 3.0f;
        break;

    case ItemType::SHIELD:
        name = "Shield";
        description = "Temporary damage protection";
        effectValue = 2.0f + (int)rarity * 2.0f;
        duration = 8.0f + (int)rarity * 4.0f;
        break;

    case ItemType::EXPERIENCE_BOOST:
        name = "EXP Boost";
        description = "Increases experience gain";
        effectValue = 1.5f + (int)rarity * 0.5f;
        duration = 20.0f + (int)rarity * 10.0f;
        break;

    case ItemType::AMMO_REFILL:
        name = "Ammo Refill";
        description = "Resets attack cooldown";
        effectValue = 1.0f;
        duration = 0;
        break;

    case ItemType::MULTI_SHOT:
        name = "Multi Shot";
        description = "Shoot multiple bullets";
        effectValue = 2.0f + (int)rarity;
        duration = 8.0f + (int)rarity * 4.0f;
        break;

    default:
        name = "Unknown Item";
        description = "Unknown effect";
        effectValue = 1.0f;
        duration = 5.0f;
        break;
    }

    return ItemData(type, rarity, name, description, effectValue, duration, color);
}

ItemRarity ItemSystem::GenerateRarity() {
    int roll = rarityDist(rng);
    if (roll < 60) return ItemRarity::COMMON;      // 60%
    if (roll < 85) return ItemRarity::RARE;        // 25%
    if (roll < 95) return ItemRarity::EPIC;        // 10%
    return ItemRarity::LEGENDARY;                   // 5%
}

COLORREF ItemSystem::GetItemColor(ItemType type) {
    switch (type) {
    case ItemType::HEALTH_POTION: return RED;
    case ItemType::SPEED_BOOST: return CYAN;
    case ItemType::DAMAGE_UP: return RGB(255, 100, 0);
    case ItemType::FIRE_RATE_UP: return YELLOW;
    case ItemType::SHIELD: return RGB(192, 192, 192);
    case ItemType::EXPERIENCE_BOOST: return RGB(0, 255, 100);
    case ItemType::AMMO_REFILL: return RGB(255, 0, 255);
    case ItemType::MULTI_SHOT: return RGB(100, 100, 255);
    default: return WHITE;
    }
}