#pragma once

#include "CircularBuffer.h"
#include "../src/BitMap.h"
#include "../src/gameApi.h"
#include "../src/gameConfig.h"

#include <cstdint>

namespace Keg {

enum struct LoadableAssetState {
    NOT_LOADED,
    HAS_PENDING_REQUEST,
    HAS_PENDING_RESPONSE
};

struct LoadableAsset {
    char filename[MAX_ASSET_PATH_LENGTH];
    void* content;
    uint32_t contentLength;
    LoadableAssetState state;
};

struct AssetLoader {
    char assetsPath[MAX_ASSET_PATH_LENGTH];

    BitMap<MAX_ASSETS> allocationMap;
    LoadableAsset assets[MAX_ASSETS];

    CircularBuffer requestQueue;
    CircularBuffer responseQueue;

    AssetLoader(char const* assetsPath);

    void handleQueue(AssetQueue& assetQueue);

    void addAsset(int32_t assetId, char const* filename);
    void requestAssetLoading(int32_t assetId);
    void requestAssetLoading(char const* filename);
    void acknowledgeLoadingResponse(int32_t assetId);

    void loadRequestedAssets();
    void loadAsset(int32_t assetId);
};

}
