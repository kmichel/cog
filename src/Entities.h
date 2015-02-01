#pragma once

#include "BitMap.h"
#include "components/Cameras.h"
#include "components/MeshRenderers.h"
#include "components/Transforms.h"
#include "gameConfig.h"

namespace Keg {

    struct Entity {
        int32_t transform;
        int32_t meshRenderer;
        int32_t camera;
    };

    struct Entities {
        BitMap<MAX_ENTITIES_COUNT> allocationMap;
        Entity entities[MAX_ENTITIES_COUNT];

        Transforms transforms;
        MeshRenderers meshRenderers;
        Cameras cameras;

        int32_t allocate();

        void release(int32_t entityId);

        Transform& addTransform(int32_t entityId);

        MeshRenderer& addMeshRenderer(int32_t entityId);

        Camera& addCamera(int32_t entityId);

        Transform& getTransform(int32_t entityId);

        MeshRenderer& getMeshRenderer(int32_t entity_id);

        Camera& getCamera(int32_t entityId);
    };

}
