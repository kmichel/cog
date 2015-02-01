#pragma once

#include "BitMap.h"
#include "gameConfig.h"

#include <OpenGL/gl3.h>

namespace Keg {

    struct Shaders;
    struct Textures;
    struct TextureCubes;

    struct MaterialTexture {
        int32_t texture;
        char uniformName[MAX_SHADER_UNIFORM_NAME_LENGTH];
    };

    struct MaterialTextureCube {
        int32_t textureCube;
        char uniformName[MAX_SHADER_UNIFORM_NAME_LENGTH];
    };

    struct Material {
        int32_t shader;
        GLuint textureUnitToTexture[MAX_MATERIAL_TEXTURES_COUNT];
        MaterialTexture textures[MAX_MATERIAL_TEXTURES_COUNT];
        MaterialTextureCube textureCubes[MAX_MATERIAL_TEXTURE_CUBES_COUNT];

        void setTexture(char const* textureName, int32_t textureId);
        void setTextureCube(char const* textureName, int32_t textureCubeId);
    };

    struct Materials {
        BitMap<MAX_MATERIALS_COUNT> allocationMap;
        Material materials[MAX_MATERIALS_COUNT];

        int32_t allocate();
        void release(int32_t materialId);

        void updateTextureBindings(Shaders& shaders, Textures& textures, TextureCubes& textureCubes);
    };
}
