#pragma once

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cstring>
#include <iostream>
#include "tools.h"

namespace Nl {

template <typename T>
class BlockingCircularBuffer
{
public:
    BlockingCircularBuffer(const std::string& name) :
        m_buffer(nullptr),
        m_size(0),
        m_name(name),
        m_bytesRead(0),
        m_bytesWritten(0),
        m_readIndex(0),
        m_writeIndex(0)
    {}

    ~BlockingCircularBuffer()
    {
        if (m_buffer)
            delete[] m_buffer;
    }

    void init(int size)
    {
        std::unique_lock<std::mutex> mlock(m_mutex);

        if (m_buffer)
            delete [] m_buffer;

        m_size = size;

        m_buffer = new T[m_size];
        memset(m_buffer, 0, m_size);

        m_bytesRead = 0;
        m_bytesWritten = 0;
        m_readIndex = 0;
        m_writeIndex = 0;

		//std::cout << "Buffer: " << m_name << " resized to: " << m_size << std::endl;

        m_condition.notify_one();
    }

    // Not sure if this is proper. This class should not know something about SampleSpecs.
    void init(const SampleSpecs_t &sampleSpecs)
    {
        init(sampleSpecs.buffersizeInBytes);
        m_sampleSpecs = sampleSpecs;
    }

    // Block callee, if nothing to read
    void get(T *buffer, unsigned int size)
    {
        if (!m_buffer) {
            std::cout << "Buffer (" << m_name << ") not initialized!" << std::endl;
        }

        std::unique_lock<std::mutex> mlock(m_mutex);
        while (availableToRead() < size || !m_buffer) {
            m_condition.wait(mlock);
        }

        m_bytesRead += size;

        for (unsigned int i=0; i<size; i++) {
            m_readIndex++;
            m_readIndex = m_readIndex % m_size;
            buffer[i] = m_buffer[m_readIndex];
        }

        m_condition.notify_one();
    }

    // Block callee, if no space in buffer
    void set(T *buffer, unsigned int size)
    {
        if (!m_buffer) {
            std::cout << "Buffer (" << m_name << ") not initialized!" << std::endl;
            //exit(-1);
        }

        std::unique_lock<std::mutex> mlock(m_mutex);
        while (availableToWrite() < size || !m_buffer) {
            m_condition.wait(mlock);
        }

        m_bytesWritten += size;

        for (unsigned int i=0; i<size; i++) {
            m_writeIndex++;
            m_writeIndex = m_writeIndex % m_size;
            m_buffer[m_writeIndex] = buffer[i];
        }

        m_condition.notify_one();
    }

    void getStat(unsigned long *readBytes, unsigned long *writtenBytes) const
    {
        *readBytes = m_bytesRead;
        *writtenBytes = m_bytesWritten;
    }

    inline unsigned int availableToRead() const
    {
        return m_size - availableToWrite();
    }

    inline unsigned int availableToWrite() const
    {
        //TODO: This is not really bullet proove, check again!!!
        int distance = abs(m_readIndex - m_writeIndex - 1);
        return m_writeIndex < m_readIndex ? distance : m_size - distance;
    }

    inline int size() const
    {
        return m_size;
    }

    inline std::string name() const
    {
        return m_name;
    }

    inline SampleSpecs_t sampleSpecs()
    {
        return m_sampleSpecs;
    }

private:
    T *m_buffer;
    std::atomic<int> m_size;
    std::string m_name;

    std::mutex m_mutex;
    std::condition_variable m_condition;

    std::atomic<unsigned long> m_bytesRead;
    std::atomic<unsigned long> m_bytesWritten;

    std::atomic<unsigned int> m_readIndex;
    std::atomic<unsigned int> m_writeIndex;

    SampleSpecs_t m_sampleSpecs;
};

} // namespace Nl
