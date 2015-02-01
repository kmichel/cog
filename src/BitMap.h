#pragma once

#include <cstdint>
#include <cstdio>

namespace Keg {

    template<int slots>
    struct BitMap {
        uint32_t bits[slots / 32];

        int32_t allocateId() {
            for (int i = 1; i < slots; ++i) {
                uint32_t mask = 1u << (i % 32);
                if ((bits[i / 32] & mask) == 0) {
                    bits[i / 32] |= mask;
                    return i;
                }
            }
            // TODO: fail here
            return 0;
        }

        void allocateId(int32_t id) {
            if (isAllocated(id))
                fprintf(stderr, "Trying to allocate an id twice");
            bits[id / 32] |= 1u << (id % 32);
        }

        void releaseId(int32_t id) {
            if (!isAllocated(id))
                fprintf(stderr, "Trying to release a non-allocated id");
            bits[id / 32] &= ~(1u << (id % 32));
        }

        bool isAllocated(int32_t id) {
            return (bits[id / 32] & 1u << (id % 32)) != 0;
        }

        int32_t getNext(int32_t id) {
            for (int i = id + 1; i < slots; ++i) {
                uint32_t mask = 1u << (i % 32);
                if ((bits[i / 32] & mask) != 0)
                    return i;
            }
            return 0;
        }
    };

}
