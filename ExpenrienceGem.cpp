#include "ExperienceGem.h"

ExperienceGem::ExperienceGem(Vector2 pos, int val)
    : GameObject(pos, 8), value(val), bobTimer(0) {
}

void ExperienceGem::Update(float deltaTime) {
    bobTimer += deltaTime * 3.0f;
    // �򵥵����¸���Ч��
}

void ExperienceGem::Render() {
    setfillcolor(CYAN);
    solidcircle((int)position.x, (int)(position.y + sin(bobTimer) * 2), (int)radius);
}