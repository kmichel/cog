#include "Materials.h"

#include "Shaders.h"
#include "Textures.h"
#include "TextureCubes.h"

#include <cstring>
#include <cstdio>

namespace Keg {

    void Material::setTexture(char const* textureName, int32_t textureId) {
        for (int i=0; i<MAX_MATERIAL_TEXTURES_COUNT; ++i) {
            if (strncmp(textures[i].uniformName, textureName, MAX_SHADER_UNIFORM_NAME_LENGTH) == 0) {
                textures[i].texture = textureId;
                return;
            } else if (textures[i].uniformName[0] == '\0') {
                textures[i].texture = textureId;
                strncpy(textures[i].uniformName, textureName, MAX_SHADER_UNIFORM_NAME_LENGTH);
                return;
            }
        }
        fprintf(stderr, "Not enough material texture slots to set texture\n");
    }

    void Material::setTextureCube(char const* textureName, int32_t textureCubeId) {
        for (int i=0; i<MAX_MATERIAL_TEXTURE_CUBES_COUNT; ++i) {
            if (strncmp(textureCubes[i].uniformName, textureName, MAX_SHADER_UNIFORM_NAME_LENGTH) == 0) {
                textureCubes[i].textureCube = textureCubeId;
                return;
            } else if (textureCubes[i].uniformName[0] == '\0') {
                textureCubes[i].textureCube = textureCubeId;
                strncpy(textureCubes[i].uniformName, textureName, MAX_SHADER_UNIFORM_NAME_LENGTH);
                return;
            }
        }
        fprintf(stderr, "Not enough material texture slots to set texture\n");
    }

    int32_t Materials::allocate() {
        int32_t materialId = allocationMap.allocateId();
        auto& material = materials[materialId];
        material.shader = 0;
        for (int i=0; i<MAX_MATERIAL_TEXTURES_COUNT; ++i) {
            auto& texture = material.textures[i];
            texture.texture = 0;
            texture.uniformName[0] = '\0';
        }
        for (int i=0; i<MAX_MATERIAL_TEXTURE_CUBES_COUNT; ++i) {
            auto& texture = material.textureCubes[i];
            texture.textureCube = 0;
            texture.uniformName[0] = '\0';
        }
        return materialId;
    }

    void Materials::release(int32_t materialId) {
        allocationMap.releaseId(materialId);
    }

    void Materials::updateTextureBindings(Shaders& shaders, Textures& textures, TextureCubes& textureCubes) {
        for (int32_t materialId = allocationMap.getNext(0); materialId != 0; materialId = allocationMap.getNext(materialId)) {
            auto& material = materials[materialId];
            // XXX: inconsistency because of texture/cube separation
            for (int textureUnitIndex = 0; textureUnitIndex < MAX_MATERIAL_TEXTURES_COUNT; ++textureUnitIndex)
                material.textureUnitToTexture[textureUnitIndex] = 0;
            if (material.shader) {
                auto& shader = shaders.shaders[material.shader];
                if (shader.state == ShaderState::VALID) {
                    for (int uniformIndex = 0; uniformIndex < MAX_SHADER_UNIFORMS; ++uniformIndex) {
                        auto& uniform = shader.uniforms[uniformIndex];
                        if (uniform.type == GL_SAMPLER_2D) {
                            for (int textureIndex = 0; textureIndex < MAX_MATERIAL_TEXTURES_COUNT; ++textureIndex) {
                                auto& texture = material.textures[textureIndex];
                                if (strcmp(texture.uniformName, uniform.name) == 0)
                                    material.textureUnitToTexture[uniform.textureUnit] = textures.textures[texture.texture].texture;
                            }
                        } else if (uniform.type == GL_SAMPLER_CUBE) {
                            for (int textureCubeIndex = 0; textureCubeIndex < MAX_MATERIAL_TEXTURE_CUBES_COUNT; ++textureCubeIndex) {
                                auto& textureCube = material.textureCubes[textureCubeIndex];
                                if (strcmp(textureCube.uniformName, uniform.name) == 0)
                                    material.textureUnitToTexture[uniform.textureUnit] = textureCubes.textureCubes[textureCube.textureCube].textureCube;
                            }
                        }
                    }
                }
            }
        }
    }
}
