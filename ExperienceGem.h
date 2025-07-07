#pragma once
#include "GameObject.h"
#include <graphics.h>
#include <cmath>

class ExperienceGem : public GameObject {
private:
    int value;
    float bobTimer;

public:
    ExperienceGem(Vector2 pos, int val = 5);

    void Update(float deltaTime) override;
    void Render() override;

    int GetValue() const { return value; }
};
