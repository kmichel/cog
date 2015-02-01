#include "MeshRenderers.h"

#include "../Entities.h"
#include "../Materials.h"
#include "../Meshes.h"
#include "../Shaders.h"

#include <cstring>

namespace Keg {

    int32_t MeshRenderers::allocate(int32_t entityId) {
        int32_t meshRendererId = allocationMap.allocateId();
        auto& meshRenderer = meshRenderers[meshRendererId];
        meshRenderer.entity = entityId;
        meshRenderer.material = 0;
        meshRenderer.mesh = 0;
        return meshRendererId;
    }

    void MeshRenderers::release(int32_t meshRendererId) {
        allocationMap.releaseId(meshRendererId);
    }

    void MeshRenderers::render(Matrix4x4 viewProjectionMatrix, Entities& entities, Meshes& meshes, Materials& materials, Shaders& shaders) {
        // TODO: group/order rendering by shader, textures, meshes, transform...
        for (int32_t meshRendererId = allocationMap.getNext(0); meshRendererId != 0; meshRendererId = allocationMap.getNext(meshRendererId)) {
            auto& meshRenderer = meshRenderers[meshRendererId];
            auto& transform = entities.getTransform(meshRenderer.entity);
            auto& material = materials.materials[meshRenderer.material];
            int32_t shaderId = shaders.shaders[material.shader].state == ShaderState::VALID ? material.shader : shaders.errorShaderId;
            // TODO: do not issue commands if errorShader is also invalid
            shaders.use(shaderId);
            shaders.setModelViewProjectionMatrix(shaderId, viewProjectionMatrix * transform.localToWorldMatrix);
            shaders.setLocalToWorldMatrix(shaderId, transform.localToWorldMatrix);
            uint32_t textureUnits = shaders.shaders[shaderId].textureUnits;
            for (uint32_t textureUnitIndex=0; textureUnitIndex<textureUnits; ++textureUnitIndex) {
                glActiveTexture(GL_TEXTURE0 + textureUnitIndex);
                // TODO: fix this stupidity
                if (textureUnitIndex == 0)
                    glBindTexture(GL_TEXTURE_2D, material.textureUnitToTexture[textureUnitIndex]);
                else
                    glBindTexture(GL_TEXTURE_CUBE_MAP, material.textureUnitToTexture[textureUnitIndex]);
            }
            meshes.meshes[meshRenderer.mesh].draw();
        }
    }

}
