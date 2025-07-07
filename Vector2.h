#pragma once
#include <cmath>

// 基础向量类
struct Vector2 {
    float x, y;

    Vector2(float x = 0, float y = 0) : x(x), y(y) {}

    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    float Length() const {
        return sqrt(x * x + y * y);
    }

    Vector2 Normalized() const {
        float len = Length();
        return len > 0 ? Vector2(x / len, y / len) : Vector2(0, 0);
    }

    float Distance(const Vector2& other) const {
        return (*this - other).Length();
    }
};