#pragma once

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cstring>

namespace Nl {

template <typename T>
class BlockingCircularBuffer
{
public:
	BlockingCircularBuffer(unsigned int size) :
		m_buffer(new T[size]),
		m_size(size),
		m_bytesRead(0),
		m_bytesWritten(0),
		m_readIndex(0),
		m_writeIndex(0)
	{
		memset(m_buffer, 0, m_size);
	}

	~BlockingCircularBuffer()
	{
		delete[] m_buffer;
	}

	// Block callee, if nothing to read
	void get(T *buffer, unsigned int size)
	{
		std::unique_lock<std::mutex> mlock(m_mutex);
		while (availableToRead() < size)
			m_condition.wait(mlock);

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
		std::unique_lock<std::mutex> mlock(m_mutex);
		while (availableToWrite() < size)
			m_condition.wait(mlock);

		m_bytesWritten += size;

		for (unsigned int i=0; i<size; i++) {
			m_writeIndex++;
			m_writeIndex = m_writeIndex % m_size;
			m_buffer[m_writeIndex] = buffer[i];
		}

		m_condition.notify_one();
	}

	void getStat(unsigned long *readBytes, unsigned long *writtenBytes)
	{
		*readBytes = m_bytesRead;
		*writtenBytes = m_bytesWritten;
	}

	inline unsigned int availableToRead()
	{
		return m_size - availableToWrite();
	}

	inline unsigned int availableToWrite()
	{
		int distance = abs(m_readIndex - m_writeIndex);
		return m_writeIndex < m_readIndex ? distance : m_size - distance;
	}

private:
	T *m_buffer;
	std::atomic<int> m_size;

	std::mutex m_mutex;
	std::condition_variable m_condition;

	std::atomic<unsigned long> m_bytesRead;
	std::atomic<unsigned long> m_bytesWritten;

	std::atomic<unsigned int> m_readIndex;
	std::atomic<unsigned int> m_writeIndex;


};

} // namespace Nl
