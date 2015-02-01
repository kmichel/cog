#pragma once

#include "Vector3.h"
#include <cmath>

namespace Keg {

    struct Quaternion {
        float x;
        float y;
        float z;
        float w;

        Quaternion normalized() {
            float length = x * x + y * y + z * z + w * w;
            float inverseLength = 1 / length;
            return {x * inverseLength, y * inverseLength, z * inverseLength, w * inverseLength};
        }

        static
        Quaternion fromAngleAxis(float angle, Vector3 axis) {
            float sinAngle = sinf(angle * 0.5f);
            return {axis.x * sinAngle, axis.y * sinAngle, axis.z * sinAngle, cosf(angle * 0.5f)};
        }

        static Quaternion fromEuler(Vector3 angles);

        Quaternion inverse() {
            return {-x, -y, -z, w};
        }

    };

    inline
    Quaternion operator*(Quaternion lhs, Quaternion rhs) {
        Quaternion ret;
        ret.x = rhs.w * lhs.x + rhs.x * lhs.w + rhs.y * lhs.z - rhs.z * lhs.y;
        ret.y = rhs.w * lhs.y + rhs.y * lhs.w + rhs.z * lhs.x - rhs.x * lhs.z;
        ret.z = rhs.w * lhs.z + rhs.z * lhs.w + rhs.x * lhs.y - rhs.y * lhs.x;
        ret.w = rhs.w * lhs.w - rhs.x * lhs.x - rhs.y * lhs.y - rhs.z * lhs.z;
        return ret;
    }

    inline
    Quaternion Quaternion::fromEuler(Vector3 angles) {
        return fromAngleAxis(angles.x, {1, 0, 0}) * fromAngleAxis(angles.y, {0, 1, 0}) * fromAngleAxis(angles.z, {0, 0, 1});
    }

    inline
    Vector3 operator*(Quaternion lhs, Vector3 rhs) {
        Vector3 tmp = 2.0f * Vector3::cross({lhs.x, lhs.y, lhs.z}, rhs);
        return rhs + lhs.w * tmp + Vector3::cross({lhs.x, lhs.y, lhs.z}, tmp);
    }

}
