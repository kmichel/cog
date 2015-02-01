#pragma once

#include "../BitMap.h"
#include "../gameConfig.h"

namespace Keg {

    struct Entities;
    struct Materials;
    struct Meshes;
    struct Shaders;

    struct Camera {
        int32_t entity;

        float verticalFov;
        float nearPlane;
        float farPlane;
        //TODO: add a viewport
    };

    struct Cameras {
        BitMap<MAX_ENTITIES_COUNT> allocationMap;
        Camera cameras[MAX_ENTITIES_COUNT];

        int32_t allocate(int32_t entityId);

        void release(int32_t cameraId);

        void render(float width, float height, Entities& entities, Meshes& meshes, Materials& materials, Shaders& shaders);
    };

}
