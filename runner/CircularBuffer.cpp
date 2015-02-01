#include "CircularBuffer.h"

namespace Keg {

    CircularBuffer::CircularBuffer()
            : readIndex(0), readCount(0) {
        pthread_mutex_init(&mutex, nullptr);
        pthread_cond_init(&condition, nullptr);
    }

    CircularBuffer::~CircularBuffer() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&condition);
    }

    void CircularBuffer::write(int32_t value) {
        pthread_mutex_lock(&mutex);
        while (readCount == bufferSize)
            pthread_cond_wait(&condition, &mutex);
        int32_t writeIndex = (readIndex + readCount) % bufferSize;
        buffer[writeIndex] = value;
        bool wasEmpty = readCount == 0;
        ++readCount;
        pthread_mutex_unlock(&mutex);
        if (wasEmpty)
            pthread_cond_signal(&condition);
    }

    int32_t CircularBuffer::read() {
        pthread_mutex_lock(&mutex);
        while (readCount == 0)
            pthread_cond_wait(&condition, &mutex);
        int32_t value = buffer[readIndex];
        readIndex = (readIndex + 1) % bufferSize;
        bool wasFull = readCount == bufferSize;
        --readCount;
        pthread_mutex_unlock(&mutex);
        if (wasFull)
            pthread_cond_signal(&condition);
        return value;
    }

    bool CircularBuffer::tryRead(int32_t& value) {
        pthread_mutex_lock(&mutex);
        bool wasFull = false;
        bool hasRead = false;
        if (readCount != 0) {
            value = buffer[readIndex];
            readIndex = (readIndex + 1) % bufferSize;
            wasFull = readCount == bufferSize;
            hasRead = true;
            --readCount;
        }
        pthread_mutex_unlock(&mutex);
        if (wasFull)
            pthread_cond_signal(&condition);
        return hasRead;
    }

}
