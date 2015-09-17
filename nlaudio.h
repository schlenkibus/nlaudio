#pragma once

#include <cstdint>
#include <list>
#include <queue>
#include <string>


typedef unsigned int samplerate_t;
typedef std::string sampleformat_t;
typedef std::string devicename_t;
typedef unsigned int channelcount_t;

class NlAudio
{
public:
	virtual void open() = 0;
	virtual void close() = 0;

	virtual void setBuffersize(unsigned int buffersize) = 0;
	virtual unsigned int getBuffersize() = 0;

	virtual void setBufferCount(unsigned int buffercount) = 0;
	virtual unsigned int getBufferCount() = 0; // Periodes in alsa

	virtual samplerate_t getSamplerate() const = 0;
	virtual void setSamplerate(samplerate_t rate) = 0;

	virtual std::list<sampleformat_t> getAvailableSampleformats() const = 0;
	virtual void setSampleFormat(sampleformat_t format) = 0;

	virtual void setChannelCount(channelcount_t n) = 0;
	virtual channelcount_t getChannelCount() = 0;

	virtual void start() = 0;
	virtual void stop() = 0;


protected:



};

