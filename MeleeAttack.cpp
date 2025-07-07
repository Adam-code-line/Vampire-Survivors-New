#include "MeleeAttack.h"

MeleeAttack::MeleeAttack(Vector2 pos, Player* player, float attackAngle, float attackRange)
    : GameObject(pos, attackRange), lifetime(0), maxLifetime(0.2f),
    damage(25), owner(player), angle(attackAngle), range(attackRange) {
}

void MeleeAttack::Update(float deltaTime) {
    lifetime += deltaTime;
    if (lifetime >= maxLifetime) {
        active = false;
    }
    // 近战攻击跟随玩家位置
    position = owner->position;
}

void MeleeAttack::Render() {
    // 绘制近战攻击范围扇形
    float startAngle = angle - PI / 6; // 60度扇形
    float endAngle = angle + PI / 6;

    // 添加透明度效果
    float alpha = 1.0f - (lifetime / maxLifetime);
    COLORREF meleeColor = RGB((int)(255 * alpha), (int)(100 * alpha), (int)(100 * alpha));

    setfillcolor(meleeColor);
    setlinecolor(meleeColor);

    // 绘制扇形攻击范围
    int centerX = (int)position.x;
    int centerY = (int)position.y;

    // 绘制扇形
    POINT points[10];
    points[0] = { centerX, centerY };

    for (int i = 0; i < 8; i++) {
        float currentAngle = startAngle + (endAngle - startAngle) * i / 7.0f;
        points[i + 1] = {
            centerX + (int)(cos(currentAngle) * range),
            centerY + (int)(sin(currentAngle) * range)
        };
    }
    points[9] = { centerX, centerY };

    solidpolygon(points, 10);
}

bool MeleeAttack::IsInAttackRange(const Vector2& targetPos) const {
    Vector2 toTarget = targetPos - position;
    float distance = toTarget.Length();

    if (distance > range) return false;

    float targetAngle = atan2(toTarget.y, toTarget.x);
    float angleDiff = abs(targetAngle - angle);

    // 处理角度环绕
    if (angleDiff > PI) angleDiff = 2 * PI - angleDiff;

    return angleDiff <= PI / 6; // 60度范围内
}