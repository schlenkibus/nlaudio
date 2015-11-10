#pragma once

#include "audio.h"

#include <alsa/asoundlib.h>
#include <thread>
#include <iostream>
#include <atomic>
#include <sstream>

#include "blockingcircularbuffer.h"

namespace Nl {

struct Statistics {
	unsigned long bytesReadFromBuffer;
	unsigned long bytesWrittenToBuffer;
	unsigned int xrunCount;
};
std::ostream& operator<<(std::ostream& lhs, const Statistics& rhs);

class AlsaDeviceIdentifier {
public:
	AlsaDeviceIdentifier(unsigned int card, unsigned int device, unsigned int subdevice, std::string name) :
		m_card(card),
		m_device(device),
		m_subdevice(subdevice),
		m_name(name)
	{
		std::stringstream ss;
		ss << "hw:" << card << "," << device << "," << subdevice;
		m_cardString = ss.str();
	}
private:
	unsigned int m_card;
	unsigned int m_device;
	unsigned int m_subdevice;
	std::string m_cardString;
	std::string m_name;
};



class AudioAlsaException : public std::exception
{
public:
	AudioAlsaException(std::string func, std::string file, int line, int errorNumber, std::string what) :
		m_func(func),
		m_file(file),
		m_line(line),
		m_errno(errorNumber),
		m_msg(what) {}
	virtual const char* what() const noexcept
	{
		std::stringstream ss;
		ss << m_file << ":" << m_func << ":" << m_line << ": " << m_msg;
		return ss.str().c_str();
	}
private:
	std::string m_func;
	std::string m_file;
	int m_line;
	int m_errno;
	std::string m_msg;
};

class AudioAlsa : public Audio
{
public:
	typedef Audio basetype;

	AudioAlsa(const devicename_t& device, std::shared_ptr<BlockingCircularBuffer<u_int8_t>> buffer, bool isInput);
	virtual ~AudioAlsa();

	virtual void open() = 0; // Might throw, therefore not in constructor
	virtual void close();

	virtual void start() = 0;
	virtual void stop() = 0;


	//TODO: Changing buffer size, while playback is running might have strange sideeffects!
	virtual void setBuffersize(unsigned int buffersize);
	virtual unsigned int getBuffersize();

	virtual void setBufferCount(unsigned int buffercount);
	virtual unsigned int getBufferCount();

	virtual samplerate_t getSamplerate() const;
	virtual void setSamplerate(samplerate_t rate);

	virtual std::list<sampleformat_t> getAvailableSampleformats() const;
	virtual sampleformat_t getSampleFormat() const;
	virtual void setSampleFormat(sampleformat_t format);

	virtual void setChannelCount(channelcount_t n);
	virtual channelcount_t getChannelCount();

	static std::list<AlsaDeviceIdentifier> getAvailableDevices();

	Statistics getStats();

protected:
	void openCommon();
	void throwOnDeviceClosed(const std::string &file, const std::string &func, int line) const;
	void throwOnDeviceRunning(const std::string& file, const std::string& func, int line) const;

	void setTerminateRequest() { m_requestTerminate = true; }
	void resetTerminateRequest() { m_requestTerminate = false; }
	bool getTerminateRequest() const { return m_requestTerminate; }
	SampleSpecs_t getSpecs();

	static int xrunRecovery(AudioAlsa *ptr, int err);

protected:
	snd_pcm_t *m_handle;
	std::thread *m_audioThread;
	snd_pcm_hw_params_t *m_hwParams;
	std::atomic<bool> m_requestTerminate;
	std::atomic<unsigned int> m_xrunRecoveryCounter;
	std::shared_ptr<BlockingCircularBuffer<u_int8_t>> m_audioBuffer;

	void throwOnAlsaError(const std::string &file, const std::string &func, int line, int e) const;
private:
	devicename_t m_deviceName;

	bool m_deviceOpen;
	bool m_isInput;

};

} // namespace Nl
