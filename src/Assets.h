#pragma once

#include "gameApi.h"
#include "gameConfig.h"
#include <cstdint>

namespace Keg {

    struct Images;
    struct Shaders;
    struct Textures;
    struct TextureCubes;

    enum struct AssetType {
        VERTEX_SHADER,
        FRAGMENT_SHADER,
        TEXTURE,
        TEXTURE_CUBE_POSITIVE_X,
        TEXTURE_CUBE_NEGATIVE_X,
        TEXTURE_CUBE_POSITIVE_Y,
        TEXTURE_CUBE_NEGATIVE_Y,
        TEXTURE_CUBE_POSITIVE_Z,
        TEXTURE_CUBE_NEGATIVE_Z
    };

    enum struct AssetState {
        MUST_SEND_REQUEST,
        WAIT_FOR_REPLY,
        WAIT_FOR_UPDATE
    };

    struct Asset {
        AssetType assetType;
        int32_t componentId;
        AssetState assetState;
        char filename[MAX_ASSET_PATH_LENGTH];

        void handleContent(Images& images, Shaders& shaders, Textures& textures, TextureCubes& textureCubes, void const* content, uint32_t contentLength);
    };

    struct Assets {
        Asset assets[MAX_ASSETS];
        int32_t assetsCount;

        void handleQueue(AssetQueue& queue, Images& images, Shaders& shaders, Textures& textures, TextureCubes& textureCubes);
        int32_t requestAsset(AssetType assetType, int32_t componentId, char const* filename);
    };

}
