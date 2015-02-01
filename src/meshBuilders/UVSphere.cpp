#include "UVSphere.h"

#include "../Meshes.h"

namespace Keg {
    void buildUVSphere(Meshes& meshes, int32_t meshId, uint32_t xSplits, uint32_t ySplits) {
        uint32_t vertices = (xSplits + 1) * ySplits;
        uint32_t triangles = xSplits * ySplits* 2;
        meshes.allocateData(meshId, vertices, triangles*3, true, true);
        auto& mesh = meshes.meshes[meshId];

        for (uint32_t y=0; y<ySplits; ++y) {
            float base = 3.141592653f * y / (ySplits - 1);
            float latitude = -cosf(base);
            float radius = sinf(base);
            for (uint32_t x=0; x<xSplits+1; ++x) {
                uint32_t index = y*(xSplits+1)+x;
                float longitude = 2 * 3.141592653f * x / xSplits;
                mesh.positions[index] = {cosf(longitude) * radius, latitude, -sinf(longitude) * radius};
                mesh.normals[index] = mesh.positions[index];
                if (y ==  0)
                    mesh.uvs[index] =  {(x - 0.5f) / xSplits, float(y) / (ySplits - 1)};
                else if (y == ySplits-1)
                    mesh.uvs[index] =  {(x + 0.5f) / xSplits, float(y) / (ySplits - 1)};
                else
                    mesh.uvs[index] = {float(x) / xSplits, float(y) / (ySplits - 1)};
            }
        }

        uint32_t index = 0;
        if (xSplits * ySplits > 0) {
            for (uint32_t y = 0; y < ySplits - 1; ++y) {
                for (uint32_t x = 0; x < xSplits; ++x) {
                    if (y != 0) {
                        mesh.indices[index++] = y * (xSplits + 1) + x;
                        mesh.indices[index++] = y * (xSplits + 1) + x + 1;
                        mesh.indices[index++] = (y + 1) * (xSplits + 1) + x;
                    }
                    if (y != ySplits - 2) {
                        mesh.indices[index++] = (y + 1) * (xSplits + 1) + x;
                        mesh.indices[index++] = y * (xSplits + 1) + x + 1;
                        mesh.indices[index++] = (y + 1) * (xSplits + 1) + x + 1;
                    }
                }
            }
        }

        mesh.uploadData();
    }
}
