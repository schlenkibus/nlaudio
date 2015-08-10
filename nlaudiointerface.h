#pragma once

#include <cstdint>
#include <list>
#include <queue>
#include <string>

typedef unsigned int samplerate_t;
typedef std::string sampleformat_t;
typedef std::string devicename_t;

class NlAudioInterface
{
public:
	NlAudioInterface() {}
	virtual ~NlAudioInterface() {}

	virtual void open() = 0;
	virtual void close() = 0;

	virtual void setBuffersize(unsigned int buffersize) = 0;
	virtual unsigned int getBuffersize() = 0;

	virtual samplerate_t getSamplerate() const = 0;
	virtual void setSamplerate(samplerate_t rate) = 0;

	virtual std::list<sampleformat_t> getAvailableSampleformats() const = 0;
	virtual void setSampleFormat(sampleformat_t format) = 0;

	virtual void start() = 0;
	virtual void stop() = 0;


protected:

private:
	virtual void process(float *in, float *out, unsigned int count) = 0;
	virtual void process(char *in, char *out, unsigned int count) = 0;

};

