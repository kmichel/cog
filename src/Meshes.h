#pragma once

#include "BitMap.h"
#include "BuddyAllocator.h"
#include "gameConfig.h"
#include "Vector2.h"
#include "Vector3.h"

#include <OpenGL/gl3.h>

namespace Keg {

    struct Mesh {
        GLuint vertexArray;
        GLuint positionsBuffer;
        GLuint normalsBuffer;
        GLuint uvsBuffer;
        GLuint indicesBuffer;

        uint32_t verticesCount;
        uint32_t indicesCount;

        Vector3* positions;
        Vector3* normals;
        Vector2* uvs;
        uint32_t* indices;

        void calculateNormals();

        void uploadData();

        void draw();
    };

    struct Meshes {
        BitMap<MAX_MESHES_COUNT> allocationMap;
        Mesh meshes[MAX_MESHES_COUNT];
        BuddyAllocator<MAX_MESHES_DATA_SIZE, MESHES_DATA_BLOCK_SIZE> meshesData;

        int32_t allocate();
        void release(int32_t meshId);

        void clearData(int32_t meshId);
        void allocateData(int32_t meshId, uint32_t vertices, uint32_t indices, bool includeNormals, bool includeUvs);
    };

}
