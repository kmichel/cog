#include "Game.h"

#include "meshBuilders/Quad.h"
#include "meshBuilders/UVSphere.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

void kegGameUpdate(void* memory, float width, float height, float elapsedTime, Keg::AssetQueue& assetQueue, Keg::InputState const& inputState, Keg::InputConfig &inputConfig) {
    Keg::Game* game = reinterpret_cast<Keg::Game*>(memory);
    game->update(width, height, elapsedTime, assetQueue, inputState, inputConfig);
};

namespace Keg {

    void Game::initialize() {
        if (!initialized) {
            printf("Initializing...\n");

            printf(
                    "game: %ldM, entities:%ldM, shaders: %ldM, meshes: %ldM, textures: %ldM\n",
                    sizeof(Game) / 1024 / 1024, sizeof(Entities) / 1024 / 1024,
                    sizeof(Shaders) / 1024 / 1024, sizeof(Meshes) / 1024 / 1024,
                    sizeof(Textures) / 1024 / 1024);

            shaders.errorShaderId = shaders.allocate();
            assets.requestAsset(AssetType::VERTEX_SHADER, shaders.errorShaderId, "shaders/error_vertex.glsl");
            assets.requestAsset(AssetType::FRAGMENT_SHADER, shaders.errorShaderId, "shaders/error_fragment.glsl");

            int32_t shaderId = shaders.allocate();
            assets.requestAsset(AssetType::VERTEX_SHADER, shaderId, "shaders/simple_vertex.glsl");
            assets.requestAsset(AssetType::FRAGMENT_SHADER, shaderId, "shaders/simple_fragment.glsl");

            int32_t meshId = meshes.allocate();
            //buildQuad(meshes, meshId);
            buildUVSphere(meshes, meshId, 16, 18);

            int32_t textureId = textures.allocate();
            assets.requestAsset(AssetType::TEXTURE, textureId, "sample.png");

            int32_t textureCubeId = textureCubes.allocate();
            assets.requestAsset(AssetType::TEXTURE_CUBE_POSITIVE_X, textureCubeId, "saintpeter/posx_s.png");
            assets.requestAsset(AssetType::TEXTURE_CUBE_NEGATIVE_X, textureCubeId, "saintpeter/negx_s.png");
            assets.requestAsset(AssetType::TEXTURE_CUBE_POSITIVE_Y, textureCubeId, "saintpeter/posy_s.png");
            assets.requestAsset(AssetType::TEXTURE_CUBE_NEGATIVE_Y, textureCubeId, "saintpeter/negy_s.png");
            assets.requestAsset(AssetType::TEXTURE_CUBE_POSITIVE_Z, textureCubeId, "saintpeter/posz_s.png");
            assets.requestAsset(AssetType::TEXTURE_CUBE_NEGATIVE_Z, textureCubeId, "saintpeter/negz_s.png");

            int32_t materialId = materials.allocate();
            auto& material = materials.materials[materialId];
            material.shader = shaderId;
            material.setTexture("albedo", textureId);
            material.setTextureCube("environment", textureCubeId);

            int32_t cameraEntity = entities.allocate();
            {
                entities.addTransform(cameraEntity);
                entities.addCamera(cameraEntity);
            }

            int32_t entity = entities.allocate();
            {
                entities.addTransform(entity);
                auto& meshRenderer = entities.addMeshRenderer(entity);
                meshRenderer.material = materialId;
                meshRenderer.mesh = meshId;
            }

            int32_t subEntity = entities.allocate();
            {
                entities.addTransform(subEntity);
                auto& meshRenderer = entities.addMeshRenderer(subEntity);
                meshRenderer.material = materialId;
                meshRenderer.mesh = meshId;
            }

            entities.transforms.setParent(3, 2);

            initialized = true;
        }
    }

    void Game::update(float width, float height, float elapsedTime, AssetQueue& assetQueue, InputState const& inputState, InputConfig &inputConfig) {
        initialize();
        assets.handleQueue(assetQueue, images, shaders, textures, textureCubes);

        time += elapsedTime;

        auto& cameraTransform = entities.getTransform(1);
        //cameraTransform.position = {0, 0, 2.2723f};

        if (inputConfig.captureCursor)
            cameraAngles += inputState.mouse.positionDelta * - 0.1f * elapsedTime;
        else
            cameraAngles += inputState.mouse.scrollDelta * 0.2f * elapsedTime;
        float maxAngle = 3.151692653f * 0.25f;
        cameraAngles.y = cameraAngles.y > maxAngle ? maxAngle : (cameraAngles.y < -maxAngle ? -maxAngle : cameraAngles.y);
        cameraTransform.rotation =  Quaternion::fromAngleAxis(cameraAngles.y, {1, 0, 0}) * Quaternion::fromAngleAxis(cameraAngles.x, {0, 1, 0});

        if (inputState.mouse.leftButton.wasPressed)
            inputConfig.captureCursor = !inputConfig.captureCursor;

        Vector3 movement = {0, 0, 0};
        if (inputState.keyboard.leftKey.isDown)
            movement.x -= 1.0f;
        if (inputState.keyboard.rightKey.isDown)
            movement.x += 1.0f;
        if (inputState.keyboard.forwardKey.isDown)
            movement.z -= 1.0f;
        if (inputState.keyboard.backKey.isDown)
            movement.z += 1.0f;

        if (movement.squaredLength() > 0.0f) {
            Quaternion xRotation = Quaternion::fromAngleAxis(cameraAngles.x, {0, 1, 0});
            float movementSpeed = inputState.keyboard.walkKey.isDown ? 1.0f : 5.0f;
            movement = movementSpeed * elapsedTime * movement.normalized();
            movement = xRotation * movement;
            cameraTransform.position += movement;
        }

        auto& transform = entities.getTransform(2);
        transform.position = {0, -0.5f, -2};
        transform.rotation = {0, 0, 0, 1};
        transform.rotation = Quaternion::fromAngleAxis(sinf(3 * time), {1, 0, 0});
        //transform.rotation = Quaternion::fromAngleAxis(-0.45f * 3.141592653f, {1, 0, 0});

        auto& subTransform = entities.getTransform(3);
        subTransform.position.y = 2;
        subTransform.rotation = Quaternion::fromAngleAxis(time, {1, 0, 0});

        entities.transforms.updateWorldMatrices();
        materials.updateTextureBindings(shaders, textures, textureCubes);
        entities.cameras.render(width, height, entities, meshes, materials, shaders);
    };
}
