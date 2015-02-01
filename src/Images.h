#pragma once

#include "BitMap.h"
#include "BuddyAllocator.h"
#include "gameConfig.h"

namespace Keg {
    struct Image {
        uint32_t width;
        uint32_t height;
        uint32_t components;
        unsigned char* pixels;
        bool isLinear;
    };

    struct Images {
        BitMap<MAX_IMAGES_COUNT> allocationMap;
        Image images[MAX_IMAGES_COUNT];
        BuddyAllocator<MAX_IMAGES_DATA_SIZE, IMAGES_DATA_BLOCK_SIZE> imagePixels;

        int32_t allocate(uint32_t width, uint32_t height, uint32_t components, bool isLinear);
        void release(int32_t imageId);

        Image& getImage(int32_t imageId);
    };
}
