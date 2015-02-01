#pragma once

#include <cstdint>
#include <pthread.h>

namespace Keg {

    struct CircularBuffer {
        static const int32_t bufferSize = 256;
        int32_t buffer[bufferSize];
        int32_t readIndex;
        int32_t readCount;

        pthread_mutex_t mutex;
        pthread_cond_t condition;

        CircularBuffer();

        ~CircularBuffer();

        void write(int32_t value);

        int32_t read();

        bool tryRead(int32_t& value);
    };

}
