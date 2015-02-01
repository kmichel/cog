#pragma once

#include "Quaternion.h"
#include "Vector3.h"

namespace Keg {

    struct Matrix4x4 {
        float values[4][4];

        static
        Matrix4x4 fromTranslation(Vector3 translation) {
            return {
                    {
                            {1, 0, 0, translation.x}, {0, 1, 0, translation.y}, {0, 0, 1, translation.z}, {0, 0, 0, 1}
                    }
            };
        }

        static
        Matrix4x4 fromRotation(Quaternion rotation) {
            float xx = rotation.x * rotation.x;
            float xy = rotation.x * rotation.y;
            float xz = rotation.x * rotation.z;
            float xw = rotation.x * rotation.w;

            float yy = rotation.y * rotation.y;
            float yz = rotation.y * rotation.z;
            float yw = rotation.y * rotation.w;

            float zz = rotation.z * rotation.z;
            float zw = rotation.z * rotation.w;

            return {
                    {
                            {1 - 2 * yy - 2 * zz, 2 * xy - 2 * zw, 2 * xz + 2 * yw, 0},
                            {2 * xy + 2 * zw, 1 - 2 * xx - 2 * zz, 2 * yz - 2 * xw, 0},
                            {2 * xz - 2 * yw, 2 * yz + 2 * xw, 1 - 2 * xx - 2 * yy, 0},
                            {0, 0, 0, 1}
                    }
            };
        }

        static
        Matrix4x4 fromTranslationRotation(Vector3 translation, Quaternion rotation);

        static
        Matrix4x4 perspective(float verticalFov, float aspectRatio, float near, float far) {
            float depth = far - near;
            float inverseDepth = 1.0f / depth;

            float inverseTan = 1.0f / tanf(0.5f * verticalFov);
            return {
                    {
                            {inverseTan / aspectRatio, 0, 0, 0},
                            {0, inverseTan, 0, 0},
                            {0, 0, -(far + near) * inverseDepth, -2 * far * near * inverseDepth},
                            {0, 0, -1, 0}
                    }
            };
        }

        Matrix4x4 transpose() {
            return {
                    {
                            {values[0][0], values[1][0], values[2][0], values[3][0]},
                            {values[0][1], values[1][1], values[2][1], values[3][1]},
                            {values[0][2], values[1][2], values[2][2], values[3][2]},
                            {values[0][3], values[1][3], values[2][3], values[3][3]}
                    }
            };
        }

        Matrix4x4 transformationInverse() {
            float t0 = values[0][0] * values[0][3] + values[1][0] * values[1][3] + values[2][0] * values[2][3];
            float t1 = values[0][1] * values[0][3] + values[1][1] * values[1][3] + values[2][1] * values[2][3];
            float t2 = values[0][2] * values[0][3] + values[1][2] * values[1][3] + values[2][2] * values[2][3];
            Matrix4x4 ret = {
                    {
                            {values[0][0], values[1][0], values[2][0], -t0},
                            {values[0][1], values[1][1], values[2][1], -t1},
                            {values[0][2], values[1][2], values[2][2], -t2},
                            {0, 0, 0, 1}
                    }
            };
            return ret;
        }

        Matrix4x4 inverse() {
            Matrix4x4 ret;

            ret.values[0][0] = values[1][1] * values[2][2] * values[3][3] - values[1][1] * values[3][2] * values[2][3] -
                    values[1][2] * values[2][1] * values[3][3] + values[1][2] * values[3][1] * values[2][3] + values[1][3] * values[2][1] * values[3][2] -
                    values[1][3] * values[3][1] * values[2][2];

            ret.values[0][1] = -values[0][1] * values[2][2] * values[3][3] + values[0][1] * values[3][2] * values[2][3] +
                    values[0][2] * values[2][1] * values[3][3] - values[0][2] * values[3][1] * values[2][3] - values[0][3] * values[2][1] * values[3][2] +
                    values[0][3] * values[3][1] * values[2][2];

            ret.values[0][2] = values[0][1] * values[1][2] * values[3][3] - values[0][1] * values[3][2] * values[1][3] -
                    values[0][2] * values[1][1] * values[3][3] + values[0][2] * values[3][1] * values[1][3] + values[0][3] * values[1][1] * values[3][2] -
                    values[0][3] * values[3][1] * values[1][2];

            ret.values[0][3] = -values[0][1] * values[1][2] * values[2][3] + values[0][1] * values[2][2] * values[1][3] +
                    values[0][2] * values[1][1] * values[2][3] - values[0][2] * values[2][1] * values[1][3] - values[0][3] * values[1][1] * values[2][2] +
                    values[0][3] * values[2][1] * values[1][2];

            ret.values[1][0] = -values[1][0] * values[2][2] * values[3][3] + values[1][0] * values[3][2] * values[2][3] +
                    values[1][2] * values[2][0] * values[3][3] - values[1][2] * values[3][0] * values[2][3] - values[1][3] * values[2][0] * values[3][2] +
                    values[1][3] * values[3][0] * values[2][2];

            ret.values[1][1] = values[0][0] * values[2][2] * values[3][3] - values[0][0] * values[3][2] * values[2][3] -
                    values[0][2] * values[2][0] * values[3][3] + values[0][2] * values[3][0] * values[2][3] + values[0][3] * values[2][0] * values[3][2] -
                    values[0][3] * values[3][0] * values[2][2];

            ret.values[1][2] = -values[0][0] * values[1][2] * values[3][3] + values[0][0] * values[3][2] * values[1][3] +
                    values[0][2] * values[1][0] * values[3][3] - values[0][2] * values[3][0] * values[1][3] - values[0][3] * values[1][0] * values[3][2] +
                    values[0][3] * values[3][0] * values[1][2];

            ret.values[1][3] = values[0][0] * values[1][2] * values[2][3] - values[0][0] * values[2][2] * values[1][3] -
                    values[0][2] * values[1][0] * values[2][3] + values[0][2] * values[2][0] * values[1][3] + values[0][3] * values[1][0] * values[2][2] -
                    values[0][3] * values[2][0] * values[1][2];

            ret.values[2][0] = values[1][0] * values[2][1] * values[3][3] - values[1][0] * values[3][1] * values[2][3] -
                    values[1][1] * values[2][0] * values[3][3] + values[1][1] * values[3][0] * values[2][3] + values[1][3] * values[2][0] * values[3][1] -
                    values[1][3] * values[3][0] * values[2][1];

            ret.values[2][1] = -values[0][0] * values[2][1] * values[3][3] + values[0][0] * values[3][1] * values[2][3] +
                    values[0][1] * values[2][0] * values[3][3] - values[0][1] * values[3][0] * values[2][3] - values[0][3] * values[2][0] * values[3][1] +
                    values[0][3] * values[3][0] * values[2][1];

            ret.values[2][2] = values[0][0] * values[1][1] * values[3][3] - values[0][0] * values[3][1] * values[1][3] -
                    values[0][1] * values[1][0] * values[3][3] + values[0][1] * values[3][0] * values[1][3] + values[0][3] * values[1][0] * values[3][1] -
                    values[0][3] * values[3][0] * values[1][1];

            ret.values[2][3] = -values[0][0] * values[1][1] * values[2][3] + values[0][0] * values[2][1] * values[1][3] +
                    values[0][1] * values[1][0] * values[2][3] - values[0][1] * values[2][0] * values[1][3] - values[0][3] * values[1][0] * values[2][1] +
                    values[0][3] * values[2][0] * values[1][1];

            ret.values[3][0] = -values[1][0] * values[2][1] * values[3][2] + values[1][0] * values[3][1] * values[2][2] +
                    values[1][1] * values[2][0] * values[3][2] - values[1][1] * values[3][0] * values[2][2] - values[1][2] * values[2][0] * values[3][1] +
                    values[1][2] * values[3][0] * values[2][1];

            ret.values[3][1] = values[0][0] * values[2][1] * values[3][2] - values[0][0] * values[3][1] * values[2][2] -
                    values[0][1] * values[2][0] * values[3][2] + values[0][1] * values[3][0] * values[2][2] + values[0][2] * values[2][0] * values[3][1] -
                    values[0][2] * values[3][0] * values[2][1];

            ret.values[3][2] = -values[0][0] * values[1][1] * values[3][2] + values[0][0] * values[3][1] * values[1][2] +
                    values[0][1] * values[1][0] * values[3][2] - values[0][1] * values[3][0] * values[1][2] - values[0][2] * values[1][0] * values[3][1] +
                    values[0][2] * values[3][0] * values[1][1];

            ret.values[3][3] = values[0][0] * values[1][1] * values[2][2] - values[0][0] * values[2][1] * values[1][2] -
                    values[0][1] * values[1][0] * values[2][2] + values[0][1] * values[2][0] * values[1][2] + values[0][2] * values[1][0] * values[2][1] -
                    values[0][2] * values[2][0] * values[1][1];

            float det = values[0][0] * ret.values[0][0] + values[1][0] * ret.values[0][1] + values[2][0] * ret.values[0][2] + values[3][0] * ret.values[0][3];

            if (det == 0) {
                // TODO: warn about the problem
            }

            det = 1.0f / det;

            for (int y = 0; y < 4; ++y)
                for (int x = 0; x < 4; ++x)
                    ret.values[y][x] *= det;

            return ret;
        }
    };


    inline
    Matrix4x4 operator*(Matrix4x4 lhs, Matrix4x4 rhs) {
        Matrix4x4 ret;
        for (int y = 0; y < 4; ++y)
            for (int x = 0; x < 4; ++x)
                ret.values[y][x] = lhs.values[y][0] * rhs.values[0][x] + lhs.values[y][1] * rhs.values[1][x] + lhs.values[y][2] * rhs.values[2][x] +
                        lhs.values[y][3] * rhs.values[3][x];
        return ret;
    }

    inline
    Matrix4x4 Matrix4x4::fromTranslationRotation(Vector3 translation, Quaternion rotation) {
        return fromTranslation(translation) * fromRotation(rotation);
    }

    inline
    Vector3 operator*(Matrix4x4 lhs, Vector3 rhs) {
        return {
                lhs.values[0][0]*rhs.x+lhs.values[0][1]*rhs.y+lhs.values[0][2]*rhs.z,
                lhs.values[1][0]*rhs.x+lhs.values[1][1]*rhs.y+lhs.values[1][2]*rhs.z,
                lhs.values[2][0]*rhs.x+lhs.values[2][1]*rhs.y+lhs.values[2][2]*rhs.z
        };
    }

}
