#pragma once

#include <cmath>

namespace Keg {

    struct Vector3 {
        float x;
        float y;
        float z;

        static
        float dot(Vector3 lhs, Vector3 rhs) {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
        }

        static
        Vector3 cross(Vector3 lhs, Vector3 rhs) {
            return {
                    lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x
            };
        }

        float squaredLength() const {
            return dot(*this, *this);
        }

        float length() const {
            return sqrtf(squaredLength());
        }

        Vector3 normalized() const {
            float length_ = length();
            return {x / length_, y / length_, z / length_};
        }
    };

    inline
    Vector3 operator+(Vector3 lhs, Vector3 rhs) {
        return {rhs.x + lhs.x, rhs.y + lhs.y, rhs.z + lhs.z};
    }

    inline
    Vector3 operator-(Vector3 lhs, Vector3 rhs) {
        return {rhs.x - lhs.x, rhs.y - lhs.y, rhs.z - lhs.z};
    }

    inline
    Vector3 operator*(float scale, Vector3 rhs) {
        return {rhs.x * scale, rhs.y * scale, rhs.z * scale};
    }

    inline
    Vector3 operator*(Vector3 lhs, float scale) {
        return {lhs.x * scale, lhs.y * scale, lhs.z * scale};
    }

    inline
    Vector3& operator+=(Vector3& lhs, Vector3 rhs) {
        lhs = lhs + rhs;
        return lhs;
    }

    inline
    Vector3& operator-=(Vector3& lhs, Vector3 rhs) {
        lhs = lhs - rhs;
        return lhs;
    }

}
