#include "Enemy.h"

Enemy::Enemy(Vector2 pos, Player* player, int enemyLevel)
    : GameObject(pos, 15), speed(80.0f), target(player), experienceValue(10), 
      level(enemyLevel), sizeMultiplier(1.0f) {
    
    // 根据等级调整属性
    SetLevel(level);
}

void Enemy::SetLevel(int newLevel) {
    level = newLevel;
    
    // 根据等级调整属性
    sizeMultiplier = 1.0f + (level - 1) * 0.2f;  // 每级增大20%
    radius = 15.0f * sizeMultiplier;              // 调整体型
    
    speed = 80.0f + (level - 1) * 10.0f;          // 每级增加10速度
    health = maxHealth = 20 + (level - 1) * 15;   // 每级增加15血量
    experienceValue = 10 + (level - 1) * 5;       // 每级增加5经验
}

void Enemy::Update(float deltaTime) {
    if (target && target->active) {
        // 向玩家移动
        Vector2 direction = (target->position - position).Normalized();
        position = position + direction * speed * deltaTime;
    }
}

void Enemy::Render() {
    // 根据等级设置颜色
    COLORREF enemyColor;
    switch (level) {
        case 1: enemyColor = RED; break;           // 1级：红色
        case 2: enemyColor = RGB(255, 100, 0); break;  // 2级：橙色
        case 3: enemyColor = RGB(255, 0, 255); break;  // 3级：紫色
        case 4: enemyColor = RGB(100, 0, 255); break;  // 4级：深紫色
        default: enemyColor = RGB(50, 50, 50); break;  // 5级+：深灰色
    }
    
    // 绘制敌人
    setfillcolor(enemyColor);
    solidcircle((int)position.x, (int)position.y, (int)radius);
    
    // 绘制等级标识
    if (level > 1) {
        settextcolor(WHITE);
        settextstyle(12, 0, _T("Arial"));
        TCHAR levelText[10];
        _stprintf_s(levelText, _T("%d"), level);
        outtextxy((int)position.x - 5, (int)position.y - 5, levelText);
    }

    // 绘制小血条
    if (health < maxHealth) {
        int barWidth = (int)(30 * sizeMultiplier);
        int barHeight = 4;
        int barX = (int)(position.x - barWidth / 2);
        int barY = (int)(position.y - radius - 8);

        setfillcolor(DARKGRAY);
        solidrectangle(barX, barY, barX + barWidth, barY + barHeight);

        int healthWidth = (barWidth * health) / maxHealth;
        setfillcolor(GREEN);
        solidrectangle(barX, barY, barX + healthWidth, barY + barHeight);
    }
}