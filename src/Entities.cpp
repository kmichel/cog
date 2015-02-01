#include "Entities.h"

namespace Keg {

    int32_t Entities::allocate() {
        int32_t entityId = allocationMap.allocateId();
        auto& entity = entities[entityId];
        entity.transform = 0;
        entity.meshRenderer = 0;
        entity.camera = 0;
        return entityId;
    }

    void Entities::release(int32_t entityId) {
        allocationMap.releaseId(entityId);
        auto& entity = entities[entityId];
        if (entity.transform != 0)
            transforms.release(entity.transform);
        if (entity.meshRenderer != 0)
            meshRenderers.release(entity.meshRenderer);
        if (entity.camera != 0)
            cameras.release(entity.camera);
    }

    Transform& Entities::addTransform(int32_t entityId) {
        int32_t transformId = transforms.allocate(entityId);
        entities[entityId].transform = transformId;
        return transforms.transforms[transformId];
    }

    MeshRenderer& Entities::addMeshRenderer(int32_t entityId) {
        int32_t meshRendererId = meshRenderers.allocate(entityId);
        entities[entityId].meshRenderer = meshRendererId;
        return meshRenderers.meshRenderers[meshRendererId];
    }

    Camera& Entities::addCamera(int32_t entityId) {
        int32_t cameraId = cameras.allocate(entityId);
        entities[entityId].camera = cameraId;
        return cameras.cameras[cameraId];
    }

    Transform& Entities::getTransform(int32_t entityId) {
        return transforms.transforms[entities[entityId].transform];
    }

    MeshRenderer& Entities::getMeshRenderer(int32_t entityId) {
        return meshRenderers.meshRenderers[entities[entityId].meshRenderer];
    }

    Camera& Entities::getCamera(int32_t entityId) {
        return cameras.cameras[entities[entityId].camera];
    }

}
