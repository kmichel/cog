#include "Cameras.h"

#include "../Matrix4x4.h"
#include "../Entities.h"
#include <OpenGL/gl3.h>

namespace Keg {

    int32_t Cameras::allocate(int32_t entityId) {
        int32_t cameraId = allocationMap.allocateId();
        auto& camera = cameras[cameraId];
        camera.entity = entityId;
        camera.verticalFov = 0.25f * 3.141592653f;
        camera.nearPlane = 0.1f;
        camera.farPlane = 100.0f;
        return cameraId;
    }

    void Cameras::release(int32_t cameraId) {
        allocationMap.releaseId(cameraId);
    }

    void Cameras::render(float width, float height, Entities& entities, Meshes& meshes, Materials& materials, Shaders& shaders) {
        glEnable(GL_FRAMEBUFFER_SRGB);
        // This does not work on OSX / Nvidia 9600
        //glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.5f, 0.5f, 0.5f, 0);
        glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        float aspectRatio = width / height;

        // TODO: add camera ordering
        for (int32_t cameraId = allocationMap.getNext(0); cameraId != 0; cameraId = allocationMap.getNext(cameraId)) {
            auto& camera = cameras[cameraId];
            auto& transform = entities.getTransform(camera.entity);
            Matrix4x4 perspectiveMatrix = Matrix4x4::perspective(camera.verticalFov, aspectRatio, camera.nearPlane, camera.farPlane);
            Matrix4x4 viewMatrix = transform.localToWorldMatrix.transformationInverse();
            Matrix4x4 viewProjectionMatrix = perspectiveMatrix * viewMatrix;
            entities.meshRenderers.render(viewProjectionMatrix, entities, meshes, materials, shaders);
        }
    }

}
