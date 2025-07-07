#include "Bullet.h"

Bullet::Bullet(Vector2 pos, Vector2 dir, float spd)
    : GameObject(pos, 4), speed(spd), lifetime(0), maxLifetime(2.0f), damage(10) {
    velocity = dir.Normalized() * speed;
}

void Bullet::Update(float deltaTime) {
    position = position + velocity * deltaTime;
    lifetime += deltaTime;

    if (lifetime >= maxLifetime ||
        position.x < 0 || position.x > WINDOW_WIDTH ||
        position.y < 0 || position.y > WINDOW_HEIGHT) {
        active = false;
    }
}

void Bullet::Render() {
    setfillcolor(YELLOW);
    solidcircle((int)position.x, (int)position.y, (int)radius);
}