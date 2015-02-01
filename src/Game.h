#pragma once

#include "Assets.h"
#include "Entities.h"
#include "Images.h"
#include "Materials.h"
#include "Meshes.h"
#include "Shaders.h"
#include "Textures.h"
#include "TextureCubes.h"
#include "gameApi.h"

extern "C"
__attribute__((visibility("default")))
void kegGameUpdate(void* memory, float width, float height, float elapsedTime, Keg::AssetQueue& assetQueue, Keg::InputState const& inputState, Keg::InputConfig &inputConfig);

namespace Keg {

    struct Game {
        bool initialized;
        float time;
        Vector2 cameraAngles;

        Assets assets;
        Entities entities;
        Images images;
        Materials materials;
        Meshes meshes;
        Shaders shaders;
        Textures textures;
        TextureCubes textureCubes;

        void initialize();
        void update(float width, float height, float elapsedTime, AssetQueue& assetQueue, InputState const& inputState, InputConfig &inputConfig);
    };

}
