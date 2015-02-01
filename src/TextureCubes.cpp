#include "TextureCubes.h"

#include "Images.h"

#include <OpenGL/gl3ext.h>

namespace Keg {

    int32_t TextureCubes::allocate() {
        int32_t textureCubeId = allocationMap.allocateId();
        auto& textureCube = textureCubes[textureCubeId];
        textureCube.textureCube = 0;
        for (uint32_t i=0; i<6; ++i)
            textureCube.facesImages[i] = 0;
        return textureCubeId;
    }

    void TextureCubes::release(int32_t textureCubeId, Images& images) {
        allocationMap.releaseId(textureCubeId);
        auto& textureCube = textureCubes[textureCubeId];
        if (textureCube.textureCube != 0)
            glDeleteTextures(1, &textureCube.textureCube);
        for (uint32_t i=0; i<6; ++i)
            if (textureCube.facesImages[i])
                images.release(textureCube.facesImages[i]);
    }

    void TextureCubes::setImage(int32_t textureCubeId, CubeFace cubeFace, Images& images, int32_t imageId) {
        auto& textureCube = textureCubes[textureCubeId];
        uint32_t cubeFaceIndex = static_cast<uint32_t>(cubeFace);
        // Assign new face
        if (textureCube.facesImages[cubeFaceIndex])
                images.release(textureCube.facesImages[cubeFaceIndex]);
        textureCube.facesImages[cubeFaceIndex] = imageId;
        // Check for completeness
        for (uint32_t i=0; i<6; ++i)
            if (textureCube.facesImages[i] == 0)
                return;

        Image* faceImages[6];
        for (uint32_t i=0; i<6; ++i)
            faceImages[i] = &images.getImage(textureCube.facesImages[i]);

        // Check for consistency
        uint32_t width = faceImages[0]->width;
        uint32_t height = faceImages[0]->height;
        uint32_t components = faceImages[0]->components;
        bool isLinear = faceImages[0]->isLinear;
        if (width != height || !isPowerOfTwo(width)) {
            fprintf(stderr, "Cube face image must be square and its size must be a power of two");
            return;
        }
        for (uint32_t i=1; i<6; ++i) {
            auto& faceImage = faceImages[i];
            if (faceImage->width != width || faceImage->height != height || faceImage->components != components || faceImage->isLinear != isLinear) {
                fprintf(stderr, "Inconsistent geometry between cube faces\n");
                // TODO: more cleanup here
                return;
            }
        }

        fprintf(stderr, "Components: %i\n", components);

        if (textureCube.textureCube != 0)
            glDeleteTextures(1, &textureCube.textureCube);
        glGenTextures(1, &textureCube.textureCube);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureCube.textureCube);
        GLsizei glSize = static_cast<GLsizei>(width);
        GLsizei mipmapLevels = static_cast<GLsizei>(logTwo(width));
        // TODO: this should honour channels count
        GLenum internalFormat =
		components == 4 ?
		(isLinear ? GL_RGBA8 : GL_SRGB8_ALPHA8) :
		(isLinear ? GL_RGB8 : GL_SRGB8);
        glTexStorage2D(GL_TEXTURE_CUBE_MAP, mipmapLevels, internalFormat, glSize, glSize);
        
        GLenum format = components == 4 ? GL_BGRA : GL_BGR;
        GLenum type = components ==4 ? GL_UNSIGNED_INT_8_8_8_8_REV : GL_UNSIGNED_BYTE;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        for (uint32_t i=0; i<6; ++i)
            glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, glSize, glSize, format, type, faceImages[i]->pixels);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GLfloat maximumAnisotropy;
        // TODO: only fetch this at startup: add prepareOpenGL to the gameApi
        // this will prepare for the shitty case of the lost context
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maximumAnisotropy);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, maximumAnisotropy);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        fprintf(stderr, "Cube uploaded !\n");
    }
}
