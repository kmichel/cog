#pragma once

#include "../BitMap.h"
#include "../gameConfig.h"
#include "../Matrix4x4.h"

namespace Keg {

    struct Entities;
    struct Materials;
    struct Meshes;
    struct Shaders;

    struct MeshRenderer {
        int32_t entity;

        int32_t material;
        int32_t mesh;
    };

    struct MeshRenderers {
        BitMap<MAX_ENTITIES_COUNT> allocationMap;
        MeshRenderer meshRenderers[MAX_ENTITIES_COUNT];

        int32_t allocate(int32_t entityId);

        void release(int32_t meshRendererId);

        void render(Matrix4x4 viewProjectionMatrix, Entities& entities, Meshes& meshes, Materials& materials, Shaders& shaders);
    };

}
