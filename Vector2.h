#pragma once
#include <cmath>

// 2D Vector
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

    void Normalize() {
        float len = Length();
        if (len > 0) {
            x /= len;
            y /= len;
        }
    }

    Vector2 Normalized() const {
        Vector2 result = *this;
        result.Normalize();
        return result;
    }

    float Length() const {
        return sqrt(x * x + y * y);
    }

    float Distance(const Vector2& other) const {
        return (*this - other).Length();
    }
};