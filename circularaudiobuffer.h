#pragma once

#include <mutex>
#include <condition_variable>


template <typename T>
class CircularAudioBuffer
{
public:
	CircularAudioBuffer(unsigned int size) :
		m_buffer(new T[size]),
		m_size(size),
		m_bytesRead(0),
		m_bytesWritten(0),
		m_readIndex(0),
		m_writeIndex(1)
	{
		memset(m_buffer, 0, m_size);
	}

	~CircularAudioBuffer()
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

		for (int i=0; i<size; i++)
			buffer[i] = m_buffer[++m_readIndex % m_size];

		m_condition.notify_one();
	}

	// Block callee, if no space in buffer
	void set(T *buffer, unsigned int size)
	{
		std::unique_lock<std::mutex> mlock(m_mutex);
		while (availableToWrite() < size)
			m_condition.wait(mlock);

		m_bytesWritten += size;

		for (unsigned int i=0; i<size; i++)
			m_buffer[++m_writeIndex % m_size] = buffer[i];

		m_condition.notify_one();
	}

	void getStat(unsigned long *readBytes, unsigned long *writtenBytes)
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

	unsigned int m_readIndex;
	unsigned int m_writeIndex;

	unsigned int availableToRead()
	{
		int ret = m_size - m_readIndex + m_writeIndex;
		return ret;
	}

	unsigned int availableToWrite()
	{
		int ret = m_size - m_writeIndex + m_readIndex;
		return ret;
	}

};
