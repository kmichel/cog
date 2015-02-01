#pragma once

#include <cmath>

namespace Keg {

    struct Vector2 {
        float x;
        float y;

        static float dot(Vector2 lhs, Vector2 rhs) {
            return lhs.x * rhs.x + lhs.y * rhs.y;
        }

        float length() const {
            return sqrtf(dot(*this, *this));
        }

        Vector2 normalized() const {
            float length_ = length();
            return {x / length_, y / length_};
        }
    };

    inline
    Vector2 operator+(Vector2 lhs, Vector2 rhs) {
        return {rhs.x + lhs.x, rhs.y + lhs.y};
    }

    inline
    Vector2 operator-(Vector2 lhs, Vector2 rhs) {
        return {rhs.x - lhs.x, rhs.y - lhs.y};
    }

    inline
    Vector2 operator*(Vector2 lhs, float scale) {
        return {lhs.x * scale, lhs.y * scale};
    }


    inline
    Vector2& operator+=(Vector2& lhs, Vector2 rhs) {
        lhs = lhs + rhs;
        return lhs;
    }

    inline
    Vector2& operator-=(Vector2& lhs, Vector2 rhs) {
        lhs = lhs - rhs;
        return lhs;
    }

}
