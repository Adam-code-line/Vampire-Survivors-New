#pragma once
#include "GameObject.h"
#include "Item.h"
#include <graphics.h>
#include <cmath>

class Item : public GameObject {
private:
    ItemData itemData;
    float bobTimer;
    float lifetime;
    float maxLifetime;

public:
    Item(Vector2 pos, const ItemData& data);

    void Update(float deltaTime) override;
    void Render() override;

    const ItemData& GetItemData() const { return itemData; }
    bool IsExpired() const { return lifetime >= maxLifetime; }
};
