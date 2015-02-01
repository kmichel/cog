#pragma once

#include <cstdint>

namespace Keg {

    struct Meshes;

    void buildUVSphere(Meshes& meshes, int32_t meshId, uint32_t xSplits, uint32_t ySplits);
}
