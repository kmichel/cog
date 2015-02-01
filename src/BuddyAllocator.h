#pragma once

#include "intMath.h"

#include <cstdio>

namespace Keg {
    template<uint32_t totalSize, uint32_t blockSize>
    struct BuddyAllocator {
        bool isAllocated[2 * (totalSize / blockSize) - 1];
        bool isSplit[2 * (totalSize / blockSize) - 1];

        char memoryBlocks[totalSize / blockSize][blockSize];

        template<typename T>
        T* allocate(uint32_t count) {
            return static_cast<T*>(allocate(count * static_cast<uint32_t>(sizeof(T))));
        }

        void* allocate(uint32_t size) {
            if (size == 0) {
                fprintf(stderr, "Trying to allocate a block of size zero\n");
                return nullptr;
            }
            uint32_t requiredBlocks = nextPowerOfTwo((size - 1) / blockSize + 1);
            void* ret = allocateBlocks(0, 0, totalSize / blockSize, requiredBlocks);
            if (ret == nullptr)
                fprintf(stderr, "Couldn't allocate block of size: %i\n", size);
            else
                printf("Allocated %i bytes of memory (%i blocks) at position %p -> %p\n", size, requiredBlocks, ret, static_cast<char*>(ret) + (requiredBlocks * blockSize));
            return ret;
        }

        void* allocateBlocks(uint32_t index, uint32_t blockIndex, uint32_t blocksCount, uint32_t requiredBlocks) {
            if (isAllocated[index]) {
                return nullptr;
            } else {
                if (blocksCount == requiredBlocks) {
                    if (isSplit[index])
                        return nullptr;
                    else {
                        isAllocated[index] = true;
                        return memoryBlocks[blockIndex];
                    }
                } else {
                    isSplit[index] = true;
                    blocksCount /= 2;
                    void* ret = allocateBlocks((index + 1) * 2 - 1, blockIndex, blocksCount, requiredBlocks);
                    if (ret == nullptr)
                        ret = allocateBlocks((index + 1) * 2, blockIndex + blocksCount,  blocksCount, requiredBlocks);
                    return ret;
                }
            }
        }

        void release(void* memory) {
            releaseBlocks(0, 0, totalSize / blockSize, memory);
        }

        void releaseBlocks(uint32_t index, uint32_t blockIndex, uint32_t blocksCount, void* memory) {
            if (isAllocated[index]) {
                if (memoryBlocks[blockIndex] != memory)
                    fprintf(stderr, "Something wrong is happening when releasing memory");
                isAllocated[index] = false;
            } else {
                uint32_t leftIndex = (index+1) * 2 -1;
                uint32_t rightIndex = (index +1) * 2;
                blocksCount /= 2;
                void* rightStart = memoryBlocks[blockIndex + blocksCount];
                if (memory < rightStart) {
                    releaseBlocks(leftIndex, blockIndex, blocksCount, memory);
                    if (!isAllocated[rightIndex] && !isSplit[rightIndex])
                        isSplit[index] = false;
                } else {
                    releaseBlocks(rightIndex, blockIndex + blocksCount, blocksCount, memory);
                    if (!isAllocated[leftIndex] && !isSplit[leftIndex])
                        isSplit[index] = false;
                }
            }
        }
    };
}
