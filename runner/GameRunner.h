#pragma once

#include "AssetLoader.h"
#include "../src/gameApi.h"

#include <pthread.h>

namespace Keg {

    struct GameRunner {
        char libraryPath[MAX_ASSET_PATH_LENGTH];
        char assetsPath[MAX_ASSET_PATH_LENGTH];
        void* memory;

        pthread_mutex_t libraryLock;
        void* library;
        GameUpdateFunction gameUpdateFunction;

        AssetLoader assetLoader;
        AssetQueue assetQueue;
        pthread_t assetLoadingThread;

        pthread_mutex_t inputLock;
        InputState readableInputState;
        InputState writableInputState;

        InputConfig inputConfig;

        pthread_mutex_t redrawLock;
        pthread_cond_t redrawCondition;
        bool hasRedrawn;

        GameRunner(char const* libraryPath, char const* assetsPath);

        ~GameRunner();

        void allocateMemory();
        void releaseMemory();

        void startAssetLoader();
        void stopAssetLoader();

        void reloadLibrary();

        void handleFileModification(char const* filename);

        void drawFrame(float width, float height, float elapsedTime);

        void lockInputState();
        void unlockInputState();

        void signalRedraw();
        void waitRedraw();
    };

}
