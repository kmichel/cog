#include "Textures.h"

#include "Images.h"
#include "intMath.h"

#include <OpenGL/gl3ext.h>

namespace Keg {

    int32_t Textures::allocate() {
        int32_t textureId = allocationMap.allocateId();
        auto& texture = textures[textureId];
        texture.texture = 0;
        texture.imageId = 0;
        return textureId;
    }

    void Textures::release(int32_t textureId, Images& images) {
        auto& texture = textures[textureId];
        if (texture.texture != 0)
            glDeleteTextures(1, &texture.texture);
        if (texture.imageId)
            images.release(texture.imageId);
        allocationMap.releaseId(textureId);
    }

    void Textures::setImage(int32_t textureId, Images& images, int32_t imageId) {
        auto& texture = textures[textureId];
        // Why do we keep the image exactly ?
        auto& image = images.getImage(imageId);
        if (texture.imageId)
            images.release(texture.imageId);
        texture.imageId = imageId;
        if (image.width == image.height && isPowerOfTwo(image.width)) {
            if (texture.texture != 0)
                glDeleteTextures(1, &texture.texture);
            glGenTextures(1, &texture.texture);
            glBindTexture(GL_TEXTURE_2D, texture.texture);

            GLsizei glSize = static_cast<GLsizei>(image.width);
            GLsizei mipmapLevels = static_cast<GLsizei>(logTwo(image.width));

            // TODO: this should honour channels count

            GLenum internalFormat =
                    image.components == 4 ?
                            (image.isLinear ? GL_RGBA8 : GL_SRGB8_ALPHA8) :
                            (image.isLinear ? GL_RGB8 : GL_SRGB8);
            glTexStorage2D(GL_TEXTURE_2D, mipmapLevels, internalFormat, glSize, glSize);

            GLenum format = image.components == 4 ? GL_BGRA : GL_BGR;
            GLenum type = image.components == 4 ? GL_UNSIGNED_INT_8_8_8_8_REV : GL_UNSIGNED_BYTE;
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, glSize, glSize, format, type, image.pixels);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            GLfloat maximumAnisotropy;
            // TODO: only fetch this at startup: add prepareOpenGL to the gameApi
            // this will prepare for the shitty case of the lost context
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maximumAnisotropy);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maximumAnisotropy);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            fprintf(stderr, "Image must be square and its size must be a power of two");
        }
    }
}
