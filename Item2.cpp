#include "Item2.h"

Item::Item(Vector2 pos, const ItemData& data)
    : GameObject(pos, 12), itemData(data), bobTimer(0), lifetime(0), maxLifetime(30.0f) {
}

void Item::Update(float deltaTime) {
    bobTimer += deltaTime * 2.0f;
    lifetime += deltaTime;

    if (lifetime >= maxLifetime) {
        active = false;
    }
}

void Item::Render() {
    // 闪烁效果（即将消失时）
    if (lifetime > maxLifetime - 5.0f) {
        if ((int)(lifetime * 4) % 2 == 0) return; // 闪烁
    }

    // 上下浮动效果
    float yOffset = sin(bobTimer) * 3.0f;

    // 绘制道具背景光环
    COLORREF rarityColor;
    switch (itemData.rarity) {
    case ItemRarity::COMMON: 
        rarityColor = WHITE; 
        break;
    case ItemRarity::RARE: 
        rarityColor = RGB(0, 150, 255); 
        break;
    case ItemRarity::EPIC: 
        rarityColor = RGB(160, 32, 240); 
        break;
    case ItemRarity::LEGENDARY: 
        rarityColor = RGB(255, 215, 0); 
        break;
    default:
        rarityColor = WHITE;
        break;
    }

    // 绘制光环
    setlinecolor(rarityColor);
    setlinestyle(PS_SOLID, 2);
    circle((int)position.x, (int)(position.y + yOffset), (int)(radius + 3));

    // 绘制道具主体
    setfillcolor(itemData.color);
    solidcircle((int)position.x, (int)(position.y + yOffset), (int)radius);

    // 绘制道具图标（简单的形状区分）
    setfillcolor(WHITE);
    switch (itemData.type) {
    case ItemType::HEALTH_POTION:
    {
        // 十字形
        solidrectangle((int)position.x - 2, (int)(position.y + yOffset - 6),
            (int)position.x + 2, (int)(position.y + yOffset + 6));
        solidrectangle((int)position.x - 6, (int)(position.y + yOffset - 2),
            (int)position.x + 6, (int)(position.y + yOffset + 2));
        break;
    }
    case ItemType::SPEED_BOOST:
    {
        // 箭头形
        POINT arrow[3] = {
            {(int)position.x, (int)(position.y + yOffset - 6)},
            {(int)position.x - 4, (int)(position.y + yOffset + 2)},
            {(int)position.x + 4, (int)(position.y + yOffset + 2)}
        };
        solidpolygon(arrow, 3);
        break;
    }
    case ItemType::DAMAGE_UP:
    {
        // 剑形
        solidrectangle((int)position.x - 1, (int)(position.y + yOffset - 6),
            (int)position.x + 1, (int)(position.y + yOffset + 4));
        solidrectangle((int)position.x - 3, (int)(position.y + yOffset + 2),
            (int)position.x + 3, (int)(position.y + yOffset + 4));
        break;
    }
    case ItemType::FIRE_RATE_UP:
    {
        // 闪电形
        POINT lightning[5] = {
            {(int)position.x - 2, (int)(position.y + yOffset - 6)},
            {(int)position.x + 2, (int)(position.y + yOffset - 2)},
            {(int)position.x - 1, (int)(position.y + yOffset)},
            {(int)position.x + 3, (int)(position.y + yOffset + 4)},
            {(int)position.x - 2, (int)(position.y + yOffset + 2)}
        };
        solidpolygon(lightning, 5);
        break;
    }
    case ItemType::SHIELD:
    {
        // 盾牌形
        POINT shield[6] = {
            {(int)position.x, (int)(position.y + yOffset - 6)},
            {(int)position.x - 4, (int)(position.y + yOffset - 3)},
            {(int)position.x - 4, (int)(position.y + yOffset + 3)},
            {(int)position.x, (int)(position.y + yOffset + 6)},
            {(int)position.x + 4, (int)(position.y + yOffset + 3)},
            {(int)position.x + 4, (int)(position.y + yOffset - 3)}
        };
        solidpolygon(shield, 6);
        break;
    }
    case ItemType::EXPERIENCE_BOOST:
    {
        // 星形
        POINT star[8] = {
            {(int)position.x, (int)(position.y + yOffset - 6)},
            {(int)position.x - 2, (int)(position.y + yOffset - 2)},
            {(int)position.x - 6, (int)(position.y + yOffset - 2)},
            {(int)position.x - 3, (int)(position.y + yOffset + 1)},
            {(int)position.x - 4, (int)(position.y + yOffset + 5)},
            {(int)position.x, (int)(position.y + yOffset + 3)},
            {(int)position.x + 4, (int)(position.y + yOffset + 5)},
            {(int)position.x + 3, (int)(position.y + yOffset + 1)}
        };
        solidpolygon(star, 8);
        break;
    }
    case ItemType::AMMO_REFILL:
    {
        // 弹药盒形
        solidrectangle((int)position.x - 4, (int)(position.y + yOffset - 4),
            (int)position.x + 4, (int)(position.y + yOffset + 4));
        solidrectangle((int)position.x - 2, (int)(position.y + yOffset - 6),
            (int)position.x + 2, (int)(position.y + yOffset - 4));
        break;
    }
    case ItemType::MULTI_SHOT:
    {
        // 多箭头形
        POINT arrow1[3] = {
            {(int)position.x - 2, (int)(position.y + yOffset - 4)},
            {(int)position.x - 5, (int)(position.y + yOffset + 1)},
            {(int)position.x - 2, (int)(position.y + yOffset + 1)}
        };
        POINT arrow2[3] = {
            {(int)position.x + 2, (int)(position.y + yOffset - 4)},
            {(int)position.x + 5, (int)(position.y + yOffset + 1)},
            {(int)position.x + 2, (int)(position.y + yOffset + 1)}
        };
        solidpolygon(arrow1, 3);
        solidpolygon(arrow2, 3);
        break;
    }
    default:
    {
        // 默认圆点
        solidcircle((int)position.x, (int)(position.y + yOffset), 3);
        break;
    }
    }
}