#pragma once

#include "nlaudiointerface.h"

#include <alsa/asoundlib.h>
#include <thread>
#include <iostream>
#include <atomic>

#include <mutex>
#include <condition_variable>


class NlAudioAlsaException : std::exception
{
public:
	NlAudioAlsaException(int errorNumber, std::string what) : m_errno(errorNumber), m_msg(what) {}
	virtual const char* what() const throw() { return m_msg.c_str(); }
private:
	int m_errno;
	std::string m_msg;
};

class NlAudioAlsa : public NlAudioInterface
{
public:
	typedef NlAudioInterface basetype;

	NlAudioAlsa(const devicename_t& device, bool isInput);
	virtual ~NlAudioAlsa();

	virtual void open() = 0; // Might throw, therefore not in constructor
	virtual void close();

	virtual void start() = 0;
	virtual void stop() = 0;


	//TODO: Changing buffer size, while playback is running might have strange sideeffects!
	virtual void setBuffersize(unsigned int buffersize);
	virtual unsigned int getBuffersize();

	virtual samplerate_t getSamplerate() const;
	virtual void setSamplerate(samplerate_t rate);

	virtual std::list<sampleformat_t> getAvailableSampleformats() const;
	virtual void setSampleFormat(sampleformat_t format);

	static std::list<devicename_t> getAvailableDevices();

protected:
	void openCommon();
	void throwOnDeviceClosed() const;
	void throwOnDeviceRunning() const;

	void setTerminateRequest() { m_requestTerminate = true; }
	void resetTerminateRequest() { m_requestTerminate = false; }
	bool getTerminateRequest() const { return m_requestTerminate; }

	void process(float *in, float *out, unsigned int count);
	void process(char *in, char *out, unsigned int count);

	void createBuffer(unsigned int size);
	void destroyBuffer(); //TODO: Causes errors see impl.
	void getBuffer(char *buffer, unsigned int size);
	void setBuffer(char *buffer, unsigned int size);
	void getBufferStat(unsigned int *reades, unsigned int writes);

	static int xrunRecovery(snd_pcm_t *handle, int err);

//TODO: Only Public for debug purposes!!!!
public:
	snd_pcm_t *m_handle;

protected:
	std::thread *m_audioThread;
	snd_pcm_hw_params_t *m_hwParams;
	std::atomic<bool> m_requestTerminate;

	char *m_buffer;
	std::mutex m_bufferMutex;
	std::condition_variable m_bufferConditionVariable;

	void throwOnAlsaError(int e) const;
private:
	devicename_t m_deviceName;

	bool m_deviceOpen;
	bool m_isInput;

	int m_bufferReadStat;
	int m_bufferWriteStat;

};

