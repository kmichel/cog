#include "Quad.h"

#include "../Meshes.h"

namespace Keg {
    void buildQuad(Meshes& meshes, int32_t meshId) {
        meshes.allocateData(meshId, 4, 6, true, true);
        auto& mesh = meshes.meshes[meshId];
        mesh.positions[0] = {0, 0, 0};
        mesh.positions[1] = {1, 0, 0};
        mesh.positions[2] = {0, 1, 0};
        mesh.positions[3] = {1, 1, 0};

        mesh.uvs[0] = {0, 0};
        mesh.uvs[1] = {1, 0};
        mesh.uvs[2] = {0, 1};
        mesh.uvs[3] = {1, 1};

        mesh.indices[0] = 0;
        mesh.indices[1] = 1;
        mesh.indices[2] = 2;
        mesh.indices[3] = 2;
        mesh.indices[4] = 1;
        mesh.indices[5] = 3;

        mesh.uploadData();
    }
}
