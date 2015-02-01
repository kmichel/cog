#pragma once

#include <cstdint>

namespace Keg {
    struct Images;

    int32_t parseImage(Images& images, void const* content, uint32_t contentLength);
}
