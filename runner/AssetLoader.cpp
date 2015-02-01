#include "AssetLoader.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <limits>
#include <sys/errno.h>
#include <sys/stat.h>
#include <unistd.h>

// TODO: check potential race-condition on asset.state and content

namespace Keg {

    AssetLoader::AssetLoader(char const* assetsPath_)
            : assets(), requestQueue(), responseQueue() {
        strncpy(assetsPath, assetsPath_, sizeof(assetsPath));
        strncat(assetsPath, "/", sizeof(assetsPath) - strlen(assetsPath) - 1);
    }

    void AssetLoader::handleQueue(AssetQueue& assetQueue) {
        for (uint32_t responseId = 0; responseId < assetQueue.responsesCount; ++responseId)
            acknowledgeLoadingResponse(assetQueue.responses[responseId].assetId);
        assetQueue.responsesCount = 0;

        int32_t assetId;
        while (responseQueue.tryRead(assetId)) {
            auto& assetResponse = assetQueue.responses[assetQueue.responsesCount];
            assetResponse.assetId = assetId;
            assetResponse.content = assets[assetId].content;
            assetResponse.contentLength = assets[assetId].contentLength;
            ++assetQueue.responsesCount;
        }

        for (uint32_t requestId = 0; requestId < assetQueue.requestsCount; ++requestId) {
            auto& assetRequest = assetQueue.requests[requestId];
            addAsset(assetRequest.assetId, assetRequest.filename);
            requestAssetLoading(assetRequest.assetId);
        }
    }

    void AssetLoader::addAsset(int32_t assetId, char const* filename) {
        allocationMap.allocateId(assetId);
        auto& asset = assets[assetId];
        printf("Adding asset: %d: %s\n", assetId, filename);
        strncpy(asset.filename, filename, sizeof(asset.filename));
        asset.content = nullptr;
        asset.contentLength = 0;
        asset.state = LoadableAssetState::NOT_LOADED;
    }

    void AssetLoader::requestAssetLoading(int32_t assetId) {
        if (assets[assetId].state == LoadableAssetState::NOT_LOADED) {
            assets[assetId].state = LoadableAssetState::HAS_PENDING_REQUEST;
            requestQueue.write(assetId);
        }
    }

    void AssetLoader::acknowledgeLoadingResponse(int32_t assetId) {
        printf("Ack: %i\n", assetId);
        auto& asset = assets[assetId];
        free(asset.content);
        asset.content = nullptr;
        asset.contentLength = 0;
        asset.state = LoadableAssetState::NOT_LOADED;
    }

    void AssetLoader::requestAssetLoading(char const* filename) {
        size_t assetsPathLength = strlen(assetsPath);
        if (bcmp(assetsPath, filename, assetsPathLength) == 0) {
            char const* relativeFilename = filename + assetsPathLength;
            for (int32_t assetId = allocationMap.getNext(0); assetId!=0; assetId = allocationMap.getNext(assetId))
                if (strcmp(assets[assetId].filename, relativeFilename) == 0)
                    requestAssetLoading(assetId);
        }
    }

    void AssetLoader::loadRequestedAssets() {
        while (true) {
            int32_t assetId = requestQueue.read();
            if (assetId == 0)
                return;
            loadAsset(assetId);
        }
    }

    void AssetLoader::loadAsset(int32_t assetId) {
        auto& asset = assets[assetId];

        printf("Loading asset: %d: %s\n", assetId, asset.filename);

        char absoluteFilename[MAX_ASSET_PATH_LENGTH];
        strncpy(absoluteFilename, assetsPath, sizeof(absoluteFilename));
        strncat(absoluteFilename, asset.filename, sizeof(absoluteFilename) - strlen(absoluteFilename) - 1);

        void* content = nullptr;
        uint32_t contentLength = 0;
        int fileDescriptor = open(absoluteFilename, O_RDONLY);
        if (fileDescriptor == -1) {
            if (errno != ENOENT)
                fprintf(stderr, "Failed loading asset: %d: %s: %s\n", assetId, absoluteFilename, strerror(errno));
        } else {
            struct stat fileStats;
            int fstatStatus = fstat(fileDescriptor, &fileStats);
            if (fstatStatus == -1) {
                fprintf(stderr, "Failed loading asset: %d: %s: %s\n", assetId, absoluteFilename, strerror(errno));
            } else if (fileStats.st_size > std::numeric_limits<uint32_t>::max()) {
                fprintf(stderr, "Failed loading asset: %d: %s: Asset is too large\n", assetId, absoluteFilename);
            } else {
                contentLength = static_cast<uint32_t>(fileStats.st_size);
                content = malloc(contentLength);
                ssize_t readStatus = read(fileDescriptor, content, contentLength);
                if (readStatus != contentLength) {
                    free(content);
                    content = nullptr;
                    contentLength = 0;
                    if (readStatus == -1)
                        fprintf(stderr, "Failed loading asset: %d: %s: %s\n", assetId, absoluteFilename, strerror(errno));
                    else
                        fprintf(stderr, "Failed loading asset: %d: %s: Inconsistent file size\n", assetId, absoluteFilename);
                }
            }
            int closeStatus = close(fileDescriptor);
            if (closeStatus == -1)
                fprintf(stderr, "Failed closing asset: %d: %s: %s\n", assetId, absoluteFilename, strerror(errno));
        }
        asset.content = content;
        asset.contentLength = contentLength;
        asset.state = LoadableAssetState::HAS_PENDING_RESPONSE;
        responseQueue.write(assetId);
    }
}
