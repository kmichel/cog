#include "Images.h"

namespace Keg {
    int32_t Images::allocate(uint32_t width, uint32_t height, uint32_t components, bool isLinear) {
        int32_t imageId = allocationMap.allocateId();
        auto& image = getImage(imageId);
        image.width = width;
        image.height = height;
        image.components = components;
        image.isLinear = isLinear;
        image.pixels = imagePixels.allocate<unsigned char>(width * height * components);
        return imageId;
    }

    void Images::release(int32_t imageId) {
        auto& image = getImage(imageId);
        imagePixels.release(image.pixels);
        allocationMap.releaseId(imageId);
    }

    Image& Images::getImage(int32_t imageId) {
        return images[imageId];
    }
}
