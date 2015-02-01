#include "GameRunner.h"

#include <cstdio>
#include <cstring>
#include <dlfcn.h>
#include <sys/mman.h>

namespace Keg {

    GameRunner::GameRunner(char const* libraryPath_, char const* assetsPath_)
            : memory(nullptr), library(nullptr), gameUpdateFunction(nullptr), assetLoader(assetsPath_), assetQueue(),
            readableInputState(), writableInputState(), inputConfig(), hasRedrawn(false) {
        strncpy(libraryPath, libraryPath_, sizeof(libraryPath));
        strncpy(assetsPath, assetsPath_, sizeof(assetsPath));
        pthread_mutex_init(&libraryLock, nullptr);
        pthread_mutex_init(&inputLock, nullptr);
        pthread_mutex_init(&redrawLock, nullptr);
        pthread_cond_init(&redrawCondition, nullptr);
    }

    GameRunner::~GameRunner() {
        pthread_mutex_destroy(&libraryLock);
        pthread_mutex_destroy(&inputLock);
        pthread_mutex_destroy(&redrawLock);
        pthread_cond_destroy(&redrawCondition);
        releaseMemory();
    }

    void GameRunner::allocateMemory() {
        void* memoryAddress = reinterpret_cast<void*>(1ul << 42);
        memory = mmap(memoryAddress, GAME_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_FIXED | MAP_SHARED, -1, 0);
    }

    void GameRunner::releaseMemory() {
        if (memory != nullptr) {
            munmap(memory, GAME_MEMORY_SIZE);
            memory = nullptr;
        }
    }

    static void* loadRequestedAssets(void* context) {
        printf("Started asset thread");
        GameRunner* gameRunner = static_cast<GameRunner*>(context);
        gameRunner->assetLoader.loadRequestedAssets();
        return nullptr;
    }

    void GameRunner::startAssetLoader() {
        pthread_create(&assetLoadingThread, nullptr, loadRequestedAssets, this);
    }

    void GameRunner::stopAssetLoader() {
        assetLoader.requestAssetLoading(0);
        pthread_join(assetLoadingThread, nullptr);
    }

    void GameRunner::reloadLibrary() {
        printf("Reloading game library\n");
        pthread_mutex_lock(&libraryLock);
        if (library != nullptr)
            dlclose(library);
        library = dlopen(libraryPath, RTLD_LAZY);
        gameUpdateFunction = reinterpret_cast<GameUpdateFunction>(dlsym(library, "kegGameUpdate"));
        pthread_mutex_unlock(&libraryLock);
    }

    void GameRunner::handleFileModification(char const* filename) {
        if (strcmp(filename, libraryPath) == 0)
            reloadLibrary();
        else
            assetLoader.requestAssetLoading(filename);
    }

    void GameRunner::drawFrame(float width, float height, float elapsedTime) {
        //fprintf(stdout, "Frame time: %5.2fms, FPS: %3.1f\n", elapsedTime*1000, 1/elapsedTime);
        pthread_mutex_lock(&libraryLock);
        if (gameUpdateFunction != nullptr) {
            gameUpdateFunction(memory, width, height, elapsedTime, assetQueue, readableInputState, inputConfig);
            assetLoader.handleQueue(assetQueue);
        }
        pthread_mutex_unlock(&libraryLock);

        pthread_mutex_lock(&inputLock);
        memcpy(&readableInputState, &writableInputState, sizeof(readableInputState));
        writableInputState.mouse.positionDelta = {0, 0};
        writableInputState.mouse.scrollDelta = {0, 0};
        writableInputState.mouse.leftButton.wasPressed = false;
        writableInputState.mouse.leftButton.wasReleased = false;
        writableInputState.mouse.rightButton.wasPressed = false;
        writableInputState.mouse.rightButton.wasReleased = false;
        writableInputState.keyboard.leftKey.wasPressed = false;
        writableInputState.keyboard.leftKey.wasReleased = false;
        writableInputState.keyboard.rightKey.wasPressed = false;
        writableInputState.keyboard.rightKey.wasReleased = false;
        writableInputState.keyboard.forwardKey.wasPressed = false;
        writableInputState.keyboard.forwardKey.wasReleased = false;
        writableInputState.keyboard.backKey.wasPressed = false;
        writableInputState.keyboard.backKey.wasReleased = false;
        writableInputState.keyboard.walkKey.wasPressed = false;
        writableInputState.keyboard.walkKey.wasReleased = false;
        pthread_mutex_unlock(&inputLock);
    }

    void GameRunner::lockInputState() {
        pthread_mutex_lock(&inputLock);
    }

    void GameRunner::unlockInputState() {
        pthread_mutex_unlock(&inputLock);
    }

    void GameRunner::signalRedraw() {
        pthread_mutex_lock(&redrawLock);
        hasRedrawn = true;
        pthread_mutex_unlock(&redrawLock);
        pthread_cond_signal(&redrawCondition);
    }

    void GameRunner::waitRedraw() {
        pthread_mutex_lock(&redrawLock);
        hasRedrawn = false;
        while (!hasRedrawn)
            pthread_cond_wait(&redrawCondition, &redrawLock);
        pthread_mutex_unlock(&redrawLock);
    }

}
