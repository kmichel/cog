#include "Meshes.h"

namespace Keg {

    void Mesh::calculateNormals() {
        // Note: these are area-weighted normals, not great
        for (uint32_t i = 0; i < verticesCount; ++i)
            normals[i] = {0, 0, 0};
        for (uint32_t i = 0; i < indicesCount; i += 3) {
            uint32_t indexA = indices[i];
            uint32_t indexB = indices[i + 1];
            uint32_t indexC = indices[i + 2];
            Vector3 a = positions[indexA];
            Vector3 b = positions[indexB];
            Vector3 c = positions[indexC];
            Vector3 normalByArea = Vector3::cross(b - a, c - a);
            normals[indexA] += normalByArea;
            normals[indexB] += normalByArea;
            normals[indexC] += normalByArea;
        }
        for (uint32_t i = 0; i < verticesCount; ++i)
            normals[i] = normals[i].normalized();
    }

    void Mesh::uploadData() {
        if (vertexArray == 0)
            glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);

        // XXX: we should interleave vertex attributes, this would actually simplify buffers management
        if (positionsBuffer == 0)
            glGenBuffers(1, &positionsBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, positionsBuffer);
        glBufferData(GL_ARRAY_BUFFER, verticesCount * 3 * sizeof(float), positions, GL_STATIC_DRAW);
        glVertexAttribPointer(SHADER_POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(SHADER_POSITION_ATTRIBUTE_LOCATION);

        if (normals) {
            if (normalsBuffer == 0)
                glGenBuffers(1, &normalsBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
            glBufferData(GL_ARRAY_BUFFER, verticesCount * 3 * sizeof(float), normals, GL_STATIC_DRAW);
            glVertexAttribPointer(SHADER_NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray(SHADER_NORMAL_ATTRIBUTE_LOCATION);
        } else {
            if (normalsBuffer != 0) {
                glDeleteBuffers(1, &normalsBuffer);
                normalsBuffer = 0;
                glDisableVertexAttribArray(SHADER_NORMAL_ATTRIBUTE_LOCATION);
            }
        }

        if (uvs) {
            if (uvsBuffer == 0)
                glGenBuffers(1, &uvsBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, uvsBuffer);
            glBufferData(GL_ARRAY_BUFFER, verticesCount * 2 * sizeof(float), uvs, GL_STATIC_DRAW);
            glVertexAttribPointer(SHADER_TEXCOORDS_ATTRIBUTE_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray(SHADER_TEXCOORDS_ATTRIBUTE_LOCATION);
        } else {
            if (uvsBuffer != 0) {
                glDeleteBuffers(1, &uvsBuffer);
                uvsBuffer = 0;
                glDisableVertexAttribArray(SHADER_TEXCOORDS_ATTRIBUTE_LOCATION);
            }
        }

        if (indicesBuffer == 0)
            glGenBuffers(1, &indicesBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(uint32_t), indices, GL_STATIC_DRAW);
    }

    void Mesh::draw() {
        glBindVertexArray(vertexArray);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCount), GL_UNSIGNED_INT, nullptr);
    }

    int32_t Meshes::allocate() {
        int32_t meshId = allocationMap.allocateId();
        auto& mesh = meshes[meshId];
        mesh.vertexArray = 0;
        mesh.positionsBuffer = 0;
        mesh.normalsBuffer = 0;
        mesh.uvsBuffer = 0;
        mesh.indicesBuffer = 0;
        mesh.verticesCount = 0;
        mesh.indicesCount = 0;
        mesh.positions = nullptr;
        mesh.normals = nullptr;
        mesh.uvs = nullptr;
        mesh.indices = nullptr;
        return meshId;
    }

    void Meshes::release(int32_t meshId) {
        allocationMap.releaseId(meshId);
        clearData(meshId);
        if (meshes[meshId].vertexArray != 0)
            glDeleteVertexArrays(1, &meshes[meshId].vertexArray);
        if (meshes[meshId].positionsBuffer != 0)
            glDeleteBuffers(1, &meshes[meshId].positionsBuffer);
        if (meshes[meshId].normalsBuffer != 0)
            glDeleteBuffers(1, &meshes[meshId].normalsBuffer);
        if (meshes[meshId].uvsBuffer != 0)
            glDeleteBuffers(1, &meshes[meshId].uvsBuffer);
        if (meshes[meshId].indicesBuffer != 0)
            glDeleteBuffers(1, &meshes[meshId].indicesBuffer);
    }

    void Meshes::clearData(int32_t meshId) {
        auto& mesh = meshes[meshId];
        if (mesh.positions) {
            meshesData.release(mesh.positions);
            mesh.positions = nullptr;
        }
        if (mesh.normals) {
            meshesData.release(mesh.normals);
            mesh.normals = nullptr;
        }
        if (mesh.uvs) {
            meshesData.release(mesh.uvs);
            mesh.uvs = nullptr;
        }
        if (mesh.indices) {
            meshesData.release(mesh.indices);
            mesh.indices = nullptr;
        }
        mesh.verticesCount = 0;
        mesh.indicesCount = 0;
    }

    void Meshes::allocateData(int32_t meshId, uint32_t verticesCount, uint32_t indicesCount, bool includeNormals, bool includeUvs) {
        auto& mesh = meshes[meshId];
        clearData(meshId);
        mesh.positions = meshesData.allocate<Vector3>(verticesCount);
        if (includeNormals)
            mesh.normals = meshesData.allocate<Vector3>(verticesCount);
        if (includeUvs)
            mesh.uvs = meshesData.allocate<Vector2>(verticesCount);
        mesh.indices = meshesData.allocate<uint32_t>(indicesCount);
        mesh.verticesCount = verticesCount;
        mesh.indicesCount = indicesCount;
    }

}
