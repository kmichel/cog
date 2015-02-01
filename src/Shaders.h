#pragma once

#include "BitMap.h"
#include "gameConfig.h"
#include "Matrix4x4.h"

#include <OpenGL/gl3.h>

namespace Keg {

    enum struct ShaderPartState {
        EMPTY,
        INVALID,
        VALID
    };

    enum struct ShaderState {
        INCOMPLETE,
        INVALID,
        VALID
    };

    struct ShaderPart {
        GLuint shader;
        ShaderPartState state;
        char source[MAX_SHADER_SOURCE_SIZE];
        char errorLog[MAX_SHADER_ERROR_LOG_SIZE];

        void initialize(GLenum shaderType);

        void setSource(char const* source, int32_t sourceLength);
    };

    struct ShaderUniform {
        GLint location;
        GLuint textureUnit;
        GLenum type;
        int32_t size;
        char name[MAX_SHADER_UNIFORM_NAME_LENGTH];
    };

    struct Shader {
        GLuint program;
        GLint modelViewProjectionUniform;
        GLint localToWorldUniform;
        ShaderState state;
        uint32_t textureUnits;
        ShaderUniform uniforms[MAX_SHADER_UNIFORMS];
    };

    struct ShaderData {
        ShaderPart vertexShader;
        ShaderPart fragmentShader;

        char programErrorLog[MAX_SHADER_PROGRAM_ERROR_LOG_SIZE];
    };

    struct Shaders {
        BitMap<MAX_SHADERS_COUNT> allocationMap;
        Shader shaders[MAX_SHADERS_COUNT];
        ShaderData shadersData[MAX_SHADERS_COUNT];
        int32_t errorShaderId;

        int32_t allocate();
        void release(int32_t shaderId);

        void setVertexSource(int32_t shaderId, char const* source, int32_t sourceLength);
        void setFragmentSource(int32_t shaderId, char const* source, int32_t sourceLength);
        void tryCompile(int32_t shaderId);

        void use(int32_t shaderId);
        void setModelViewProjectionMatrix(int32_t shaderId, Matrix4x4 matrix);
        void setLocalToWorldMatrix(int32_t shaderId, Matrix4x4 matrix);
    };

}
