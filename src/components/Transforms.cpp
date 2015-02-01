#include "Transforms.h"

namespace Keg {

    int32_t Transforms::allocate(int32_t entityId) {
        int32_t transformId = allocationMap.allocateId();
        transforms[transformId].entity = entityId;
        transforms[transformId].parent = 0;
        transforms[transformId].firstChild = 0;
        transforms[transformId].nextSibling = 0;
        transforms[transformId].position = {0, 0, 0};
        transforms[transformId].rotation = {0, 0, 0, 1};
        return transformId;
    }

    void Transforms::release(int32_t transformId) {
        allocationMap.releaseId(transformId);
    }

    void Transforms::setParent(int32_t childTransform, int32_t parentTransform) {
        int32_t oldParent = transforms[childTransform].parent;
        if (oldParent != 0)
            clearParent(childTransform);
        transforms[childTransform].parent = parentTransform;
        transforms[childTransform].nextSibling = transforms[parentTransform].firstChild;
        transforms[parentTransform].firstChild = childTransform;
    }

    void Transforms::clearParent(int32_t childTransform) {
        int32_t oldParent = transforms[childTransform].parent;
        if (transforms[oldParent].firstChild == childTransform) {
            transforms[oldParent].firstChild = transforms[childTransform].nextSibling;
        } else {
            int32_t childIterator = transforms[oldParent].firstChild;
            while (childIterator != 0) {
                int32_t nextChild = transforms[childIterator].nextSibling;
                if (nextChild == childTransform) {
                    transforms[childIterator].nextSibling = transforms[childTransform].nextSibling;
                    break;
                }
                childIterator = nextChild;
            }
        }
        transforms[childTransform].parent = 0;
        transforms[childTransform].nextSibling = 0;
    }

    void Transforms::updateWorldMatrices() {
        for (int32_t transformId = allocationMap.getNext(0); transformId != 0; transformId = allocationMap.getNext(transformId)) {
            if (transforms[transformId].parent == 0) {
                transforms[transformId].localToWorldMatrix = Matrix4x4::fromTranslationRotation(
                        transforms[transformId].position, transforms[transformId].rotation);
                updateChildrenMatrices(transformId);
            }
        }
    }

    void Transforms::updateChildrenMatrices(int32_t transformId) {
        Matrix4x4 parentToWorldMatrix = transforms[transformId].localToWorldMatrix;
        for (int32_t childId = transforms[transformId].firstChild; childId != 0; childId = transforms[childId].nextSibling) {
            Matrix4x4 childToParentMatrix = Matrix4x4::fromTranslationRotation(transforms[childId].position, transforms[childId].rotation);
            transforms[childId].localToWorldMatrix = parentToWorldMatrix * childToParentMatrix;
            updateChildrenMatrices(childId);
        }
    }

}
