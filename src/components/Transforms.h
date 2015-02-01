#pragma once

#include "../BitMap.h"
#include "../gameConfig.h"
#include "../Matrix4x4.h"
#include "../Quaternion.h"
#include "../Vector3.h"

namespace Keg {

    struct Transform {
        int32_t entity;

        int32_t parent;
        int32_t firstChild;
        int32_t nextSibling;

        Vector3 position;
        Quaternion rotation;

        Matrix4x4 localToWorldMatrix;
    };

    struct Transforms {
        BitMap<MAX_ENTITIES_COUNT> allocationMap;
        Transform transforms[MAX_ENTITIES_COUNT];

        int32_t allocate(int32_t entityId);

        void release(int32_t transformId);

        void setParent(int32_t childTransform, int32_t parentTransform);

        void clearParent(int32_t childTransform);

        void updateWorldMatrices();

        void updateChildrenMatrices(int32_t transformId);
    };

}
