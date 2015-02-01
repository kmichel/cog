#pragma once

#include "gameConfig.h"
#include "Vector2.h"
#include <cstdint>

namespace Keg {

    struct AssetRequest {
        int32_t assetId;
        char filename[MAX_ASSET_PATH_LENGTH];
    };

    struct AssetResponse {
        int32_t assetId;
        void* content;
        uint32_t contentLength;
    };

    struct AssetQueue {
        uint32_t requestsCount;
        AssetRequest requests[MAX_ASSETS];

        uint32_t responsesCount;
        AssetResponse responses[MAX_ASSETS];
    };

    struct ButtonState {
        bool isDown;
        bool wasPressed;
        bool wasReleased;
    };

    struct MouseState {
        Vector2 position;
        Vector2 positionDelta;
        Vector2 scrollDelta;
        ButtonState leftButton;
        ButtonState rightButton;
    };

    struct KeyboardState {
        ButtonState leftKey;
        ButtonState rightKey;
        ButtonState forwardKey;
        ButtonState backKey;
        ButtonState walkKey;
    };

    struct InputState {
        MouseState mouse;
        KeyboardState keyboard;
    };

    struct InputConfig {
        bool captureCursor;
    };

    typedef void (* GameUpdateFunction)(void* memory, float width, float height, float elapsedTime, AssetQueue& assetQueue, InputState const& inputState, InputConfig& inputConfig);

}
