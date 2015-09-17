#pragma once

#include <mutex>
#include <condition_variable>

template <typename T>
class BlockingLinearBuffer
{
public:
	BlockingLinearBuffer(unsigned int size) :
		m_buffer(new T[size]),
		m_size(size),
		m_bytesRead(0),
		m_bytesWritten(0)
	{
		memset(m_buffer, 0, m_size);
	}

	~BlockingLinearBuffer()
	{
		delete[] m_buffer;
	}

	// Block callee, if nothing to read
	void BlockingLinearBuffer::get(T *buffer, unsigned int size)
	{
		std::unique_lock<std::mutex> mlock(m_mutex);
		while ((m_bytesWritten - m_bytesRead) < size)
			m_condition.wait(mlock);

		m_bytesRead += size;
		memcpy(buffer, m_buffer, size);

		return size;
	}

	// Block callee, if read in progress.
	// Wake up sleeping readers, if any
	void set(void *buffer, unsigned int size)
	{
		std::unique_lock<std::mutex> mlock(m_mutex);
		m_bytesWritten += size;
		memcpy(m_buffer, buffer, size);
		mlock.unlock();
		m_condition.notify_one();
	}

	getStat(unsigned long *readBytes, unsigned long *writtenBytes)
	{
		std::unique_lock<std::mutex> mlock(m_mutex);
		*readBytes = m_bytesRead;
		*writtenBytes = m_bytesWritten;
	}

private:
	T *m_buffer;
	int m_size;

	std::mutex m_mutex;
	std::condition_variable m_condition;

	unsigned long m_bytesRead;
	unsigned long m_bytesWritten;

};
