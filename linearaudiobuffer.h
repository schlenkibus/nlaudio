#pragma once

#include <mutex>
#include <condition_variable>

class LinearAudioBuffer
{
public:
	LinearAudioBuffer(unsigned int size);
	~LinearAudioBuffer();

	unsigned int get(void *buffer, unsigned int size);
	void set(void *buffer, unsigned int size);
	void getStat(unsigned long *readBytes, unsigned long *writtenBytes);

private:
	float *m_buffer;
	int m_size;

	std::mutex m_mutex;
	std::condition_variable m_condition;

	unsigned long m_bytesRead;
	unsigned long m_bytesWritten;

};
