#pragma once

namespace Keg {
    inline bool isPowerOfTwo(uint32_t value) {
        return (value & (value-1)) == 0;
    }

    inline uint32_t nextPowerOfTwo(uint32_t value) {
        value--;
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 4;
        value |= value >> 8;
        value |= value >> 16;
        value++;
        return value;
    }

    inline uint32_t logTwo(uint32_t value) {
        uint32_t ret = 0;
        while (value >>= 1)
            ++ret;
        return ret;
    }
}
