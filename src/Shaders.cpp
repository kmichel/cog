#include "Shaders.h"

#include <cstdio>
#include <cstring>

namespace Keg {

    void ShaderPart::initialize(GLenum shaderType) {
        shader = glCreateShader(shaderType);
        state = ShaderPartState::EMPTY;
        source[0] = '\0';
        errorLog[0] = '\0';
    }

    void ShaderPart::setSource(char const* source_, int32_t sourceLength) {
        if (sourceLength < 0)
            strncpy(source, source_, sizeof(sourceLength));
        else
            memcpy(source, source_, static_cast<size_t>(sourceLength));
        char const* sourceArray = &source[0];
        glShaderSource(shader, 1, &sourceArray, &sourceLength);
        glCompileShader(shader);

        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (compiled) {
            errorLog[0] = '\0';
            state = ShaderPartState::VALID;
        } else {
            glGetShaderInfoLog(shader, sizeof(errorLog), nullptr, errorLog);
            fprintf(stderr, "%s\n", errorLog);
            state = ShaderPartState::INVALID;
        }
    }

    void Shaders::setVertexSource(int32_t shaderId, char const* source, int32_t sourceLength) {
        shadersData[shaderId].vertexShader.setSource(source, sourceLength);
        tryCompile(shaderId);
    }

    void Shaders::setFragmentSource(int32_t shaderId, char const* source, int32_t sourceLength) {
        shadersData[shaderId].fragmentShader.setSource(source, sourceLength);
        tryCompile(shaderId);
    }

    void Shaders::tryCompile(int32_t shaderId) {
        auto& shader = shaders[shaderId];
        auto& shaderData = shadersData[shaderId];
        if (shaderData.vertexShader.state == ShaderPartState::VALID && shaderData.fragmentShader.state == ShaderPartState::VALID) {
            glAttachShader(shader.program, shaderData.vertexShader.shader);
            glAttachShader(shader.program, shaderData.fragmentShader.shader);
            glLinkProgram(shader.program);

            GLint linked;
            glGetProgramiv(shader.program, GL_LINK_STATUS, &linked);
            if (linked) {
                glUseProgram(shader.program);
                shaderData.programErrorLog[0] = '\0';
                // Introspect uniforms :
                shader.modelViewProjectionUniform = -1;
                shader.localToWorldUniform = -1;
                GLint activeUniforms;
                glGetProgramiv(shader.program, GL_ACTIVE_UNIFORMS, &activeUniforms);
                if (activeUniforms > MAX_SHADER_UNIFORMS) {
                    fprintf(stderr, "Too many uniforms in shader %i: %i\n", shaderId, activeUniforms);
                }
                GLint activeUniformMaxLength;
                glGetProgramiv(shader.program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &activeUniformMaxLength);
                if (activeUniformMaxLength > MAX_SHADER_UNIFORM_NAME_LENGTH)
                    fprintf(stderr, "Uniform name too long in shader %i: %i", shaderId, activeUniformMaxLength);
                uint32_t textureUnit = 0;
                for (GLint i=0; i<MAX_SHADER_UNIFORMS; ++i) {
                    auto& uniform = shader.uniforms[i];
                    if (i < activeUniforms) {
                        glGetActiveUniform(shader.program, static_cast<GLuint>(i), sizeof(uniform.name), nullptr, &uniform.size, &uniform.type, uniform.name);
                        uniform.location = glGetUniformLocation(shader.program, uniform.name);
                        fprintf(stdout, "Uniform: %i: %s: %i\n", i, uniform.name, uniform.location);
                        if (uniform.type == GL_SAMPLER_2D || uniform.type == GL_SAMPLER_CUBE) {
                            fprintf(stdout, "Bound to TU: %i\n", textureUnit);
                            uniform.textureUnit = textureUnit;
                            glUniform1i(uniform.location, static_cast<int32_t>(textureUnit));
                            ++textureUnit;
                        }
                        if (strcmp(uniform.name, "modelViewProjection") == 0)
                            shader.modelViewProjectionUniform = uniform.location;
                        else if (strcmp(uniform.name, "localToWorld") == 0)
                            shader.localToWorldUniform = uniform.location;
                    } else {
                        uniform.location = -1;
                        uniform.type = GL_NONE;
                        uniform.size = 0;
                        uniform.name[0] = '\0';
                    }
                }
                shader.textureUnits = textureUnit;

                shader.state = ShaderState::VALID;
            } else {
                glGetProgramInfoLog(shader.program, sizeof(shaderData.programErrorLog), nullptr, shaderData.programErrorLog);
                fprintf(stderr, "%s\n", shaderData.programErrorLog);
                shader.state = ShaderState::INVALID;
            }
        } else if (shaderData.vertexShader.state == ShaderPartState::INVALID || shaderData.fragmentShader.state == ShaderPartState::INVALID)
            shader.state = ShaderState::INVALID;
        else
            shader.state = ShaderState::INCOMPLETE;
    }

    void Shaders::use(int32_t shaderId) {
        auto& shader = shaders[shaderId];
        glUseProgram(shader.program);
    }

    void Shaders::setModelViewProjectionMatrix(int32_t shaderId, Matrix4x4 matrix) {
        auto& shader = shaders[shaderId];
        if (shader.modelViewProjectionUniform != -1)
            glUniformMatrix4fv(shader.modelViewProjectionUniform, 1, GL_TRUE, &matrix.values[0][0]);
    }

    void Shaders::setLocalToWorldMatrix(int32_t shaderId, Matrix4x4 matrix) {
        auto& shader = shaders[shaderId];
        if (shader.localToWorldUniform != -1)
        glUniformMatrix4fv(shader.localToWorldUniform, 1, GL_TRUE, &matrix.values[0][0]);
    }

    int32_t Shaders::allocate() {
        int32_t shaderId = allocationMap.allocateId();
        auto& shader = shaders[shaderId];
        auto& shaderData = shadersData[shaderId];
        shader.program = glCreateProgram();
        shader.modelViewProjectionUniform = -1;
        shader.localToWorldUniform = -1;
        shader.state = ShaderState::INCOMPLETE;
        for (int i=0; i<MAX_SHADER_UNIFORMS; ++i) {
            shader.uniforms[i].type = GL_NONE;
            shader.uniforms[i].size = 0;
            shader.uniforms[i].name[0] = '\0';
        }
        shaderData.vertexShader.initialize(GL_VERTEX_SHADER);
        shaderData.fragmentShader.initialize(GL_FRAGMENT_SHADER);
        return shaderId;
    }

    void Shaders::release(int32_t shaderId) {
        allocationMap.releaseId(shaderId);
        auto& shader = shaders[shaderId];
        auto& shaderData = shadersData[shaderId];
        glDeleteProgram(shader.program);
        glDeleteShader(shaderData.vertexShader.shader);
        glDeleteShader(shaderData.fragmentShader.shader);
    }

}
