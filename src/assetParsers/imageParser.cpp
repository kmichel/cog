#include "imageParser.h"

#include "../Images.h"
#include "../../libs/stb_image.h"

#include <cmath>

namespace Keg {

    int32_t parseImage(Images& images, void const* content, uint32_t contentLength) {
        uint32_t width;
        uint32_t height;
        uint32_t components;
        stbi_uc* stbImage = stbi_load_from_memory(
                static_cast<stbi_uc const*>(content),
                static_cast<int>(contentLength), reinterpret_cast<int*>(&width),
                reinterpret_cast<int*>(&height),
                reinterpret_cast<int*>(&components), 0);
        if (stbImage != nullptr) {
            // How do we know if the image is linear ?
            int32_t imageId = images.allocate(width, height, components, false);
            auto& image = images.getImage(imageId);
            if (components == 4) {
                // TODO: better gamma conversion
                // https://www.opengl.org/registry/specs/EXT/texture_sRGB.txt
                float alphaGammaCorrection = image.isLinear ? 1 : 1/2.2f;
                for (uint32_t y = 0; y < height; ++y) {
                    for (uint32_t x = 0; x < width; ++x) {
                        uint32_t pixel = (y * width + x) * 4;
                        uint32_t outPixel = ((height - y - 1) * width + x) * 4;
                        float alpha = powf(stbImage[pixel + 3] / 255.0f, alphaGammaCorrection);
                        image.pixels[outPixel] = static_cast<unsigned char>(roundf(stbImage[pixel + 2] * alpha));
                        image.pixels[outPixel + 1] = static_cast<unsigned char>(roundf(stbImage[pixel + 1] * alpha));
                        image.pixels[outPixel + 2] = static_cast<unsigned char>(roundf(stbImage[pixel] * alpha));
                        image.pixels[outPixel + 3] = stbImage[pixel + 3];
                    }
                }
            } else if (components == 3) {
                for (uint32_t y = 0; y < height; ++y) {
                    for (uint32_t x = 0; x < width; ++x) {
                        uint32_t pixel = (y * width + x) * 3;
                        uint32_t outPixel = ((height - y - 1) * width + x) * 3;
                        image.pixels[outPixel] = stbImage[pixel + 2];
                        image.pixels[outPixel + 1] = stbImage[pixel + 1];
                        image.pixels[outPixel + 2] = stbImage[pixel];
                    }
                }
            } else if (components == 1) {
                for (uint32_t y = 0; y < height; ++y) {
                    for (uint32_t x = 0; x < width; ++x) {
                        uint32_t pixel = y * width + x;
                        uint32_t outPixel = (height - y - 1) * width + x;
                        image.pixels[outPixel] = stbImage[pixel];
                    }
                }
            }
            stbi_image_free(stbImage);
            return imageId;
        } else {
            fprintf(stderr, "Failed loading image: %s\n", stbi_failure_reason());
            return 0;
        }
    }
}
