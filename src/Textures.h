#pragma once

#include "BitMap.h"
#include "BuddyAllocator.h"
#include "gameConfig.h"

#include <OpenGL/gl3.h>

namespace Keg {
    struct Images;

    struct Texture {
        GLuint texture;
        int32_t imageId;
    };

    struct Textures {
        BitMap<MAX_TEXTURES_COUNT> allocationMap;
        Texture textures[MAX_TEXTURES_COUNT];

        int32_t allocate();
        void release(int32_t textureId, Images& images);

        void setImage(int32_t textureId, Images& images, int32_t imageId);
    };

}
