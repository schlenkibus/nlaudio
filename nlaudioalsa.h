#pragma once

#include "nlaudiointerface.h"

#include <alsa/asoundlib.h>
#include <thread>
#include <iostream>
#include <atomic>

#include "audiobuffer.h"

struct SampleSpecs {
	unsigned int channels;			/// Channels
	unsigned int bytesPerFrame;		/// For 24_3LE this would be 3, even if actually 4 Bytes are needed for storage of one Sample
	unsigned int buffersize;		/// Size of buffer. This is relevant for latency
	unsigned int bytesPerSample;		///
	unsigned int bytesPerSampleStored;
};
std::ostream& operator<<(std::ostream& lhs, const SampleSpecs& rhs);

struct Statistics {
	unsigned long bytesReadFromBuffer;
	unsigned long bytesWrittenToBuffer;
	unsigned int xrunCount;
};
std::ostream& operator<<(std::ostream& lhs, const Statistics& rhs);

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

	NlAudioAlsa(const devicename_t& device, std::shared_ptr<AudioBuffer> buffer, bool isInput);
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

	virtual void setChannelCount(channelcount_t n);
	virtual channelcount_t getChannelCount();

	static std::list<devicename_t> getAvailableDevices();

	Statistics getStats();

protected:
	void openCommon();
	void throwOnDeviceClosed() const;
	void throwOnDeviceRunning() const;

	void setTerminateRequest() { m_requestTerminate = true; }
	void resetTerminateRequest() { m_requestTerminate = false; }
	bool getTerminateRequest() const { return m_requestTerminate; }

	void process(float *in, float *out, unsigned int count);
	void process(char *in, char *out, unsigned int count);


	static int xrunRecovery(NlAudioAlsa *ptr, int err);

	//TODO: Only Public for debug purposes!!!!
public:
	snd_pcm_t *m_handle;

protected:
	std::thread *m_audioThread;
	snd_pcm_hw_params_t *m_hwParams;
	std::atomic<bool> m_requestTerminate;
	std::atomic<unsigned int> m_xrunRecoveryCounter;
	std::shared_ptr<AudioBuffer> m_audioBuffer;

	void throwOnAlsaError(int e, const std::string& function) const;
private:
	devicename_t m_deviceName;

	bool m_deviceOpen;
	bool m_isInput;

};

