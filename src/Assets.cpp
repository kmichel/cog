#include "Assets.h"

#include "assetParsers/imageParser.h"
#include "Images.h"
#include "Shaders.h"
#include "Textures.h"
#include "TextureCubes.h"
#include "intMath.h"

#include <cstring>

namespace Keg {

    void Asset::handleContent( Images& images, Shaders& shaders,Textures& textures, TextureCubes& textureCubes, void const* content, uint32_t contentLength) {
        switch (assetType) {
            case AssetType::VERTEX_SHADER:
                shaders.setVertexSource(componentId, static_cast<char const*>(content), static_cast<int32_t>(contentLength));
                break;
            case AssetType::FRAGMENT_SHADER:
                shaders.setFragmentSource(componentId, static_cast<char const*>(content), static_cast<int32_t>(contentLength));
                break;
            case AssetType::TEXTURE:
                textures.setImage(componentId, images,  parseImage(images, content, contentLength));
                break;
            case AssetType::TEXTURE_CUBE_POSITIVE_X:
                fprintf(stderr, "POSX\n");
                textureCubes.setImage(componentId, CubeFace::POSITIVE_X, images, parseImage(images, content, contentLength));
                break;
            case AssetType::TEXTURE_CUBE_NEGATIVE_X:
                fprintf(stderr, "NEGX\n");
                textureCubes.setImage(componentId, CubeFace::NEGATIVE_X, images, parseImage(images, content, contentLength));
                break;
            case AssetType::TEXTURE_CUBE_POSITIVE_Y:
                fprintf(stderr, "POSY\n");
                textureCubes.setImage(componentId, CubeFace::POSITIVE_Y, images, parseImage(images, content, contentLength));
                break;
            case AssetType::TEXTURE_CUBE_NEGATIVE_Y:
                fprintf(stderr, "NEGY\n");
                textureCubes.setImage(componentId, CubeFace::NEGATIVE_Y, images, parseImage(images, content, contentLength));
                break;
            case AssetType::TEXTURE_CUBE_POSITIVE_Z:
                fprintf(stderr, "POSZ\n");
                textureCubes.setImage(componentId, CubeFace::POSITIVE_Z, images, parseImage(images, content, contentLength));
                break;
            case AssetType::TEXTURE_CUBE_NEGATIVE_Z:
                fprintf(stderr, "NEGZ\n");
                textureCubes.setImage(componentId, CubeFace::NEGATIVE_Z, images, parseImage(images, content, contentLength));
                break;
        }
        assetState = AssetState::WAIT_FOR_UPDATE;
    }

    void Assets::handleQueue(AssetQueue& assetQueue, Images& images, Shaders& shaders, Textures& textures, TextureCubes& textureCubes) {
        for (uint32_t responseId = 0; responseId < assetQueue.responsesCount; ++responseId) {
            auto& assetResponse = assetQueue.responses[responseId];
            // XXX: we should check that the request is really needed (in case the asset was "removed")
            auto& asset = assets[assetResponse.assetId];
            asset.handleContent(images, shaders, textures, textureCubes, assetResponse.content, assetResponse.contentLength);
        }

        assetQueue.requestsCount = 0;
        for (int32_t assetId = 1; assetId <= assetsCount; ++assetId) {
            auto& asset = assets[assetId];
            if (asset.assetState == AssetState::MUST_SEND_REQUEST) {
                auto& assetRequest = assetQueue.requests[assetQueue.requestsCount++];
                assetRequest.assetId = assetId;
                strncpy(assetRequest.filename, asset.filename, MAX_ASSET_PATH_LENGTH);
                asset.assetState = AssetState::WAIT_FOR_REPLY;
            }
        }
    }

    int32_t Assets::requestAsset(AssetType assetType, int32_t componentId, char const* filename) {
        int32_t assetId = ++assetsCount;
        auto& asset = assets[assetId];
        asset.assetType = assetType;
        asset.componentId = componentId;
        asset.assetState = AssetState::MUST_SEND_REQUEST;
        strncpy(asset.filename, filename, MAX_ASSET_PATH_LENGTH);
        return assetId;
    }

}
