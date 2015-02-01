#pragma once

#include "BitMap.h"
#include "gameConfig.h"

#include <OpenGL/gl3.h>

namespace Keg {

    struct Images;

    enum struct CubeFace {
        POSITIVE_X = 0,
        NEGATIVE_X = 1,
        POSITIVE_Y = 2,
        NEGATIVE_Y = 3,
        POSITIVE_Z = 4,
        NEGATIVE_Z = 5
    };

    struct TextureCube {
        // TODO: rename gl identifiers
        GLuint textureCube;

        int32_t facesImages[6];
    };

    struct TextureCubes {
        BitMap<MAX_TEXTURE_CUBES_COUNT> allocationMap;
        TextureCube textureCubes[MAX_TEXTURE_CUBES_COUNT];

        int32_t allocate();
        void release(int32_t textureCubeId, Images& images);

        void setImage(int32_t textureCubeId, CubeFace cubeFace, Images& images, int32_t imageId);
    };
}
