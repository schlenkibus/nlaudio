#include "nlaudioalsa.h"
#include "nlaudiointerface.h"

#include <iostream>

NlAudioAlsa::NlAudioAlsa(const devicename_t &device, std::shared_ptr<AudioBuffer> buffer, bool isInput) :
	m_handle(nullptr),
	m_hwParams(nullptr),
	m_deviceName(device),
	m_deviceOpen(false),
	m_isInput(isInput),
	m_audioBuffer(buffer)
{
	std::cout << "New " << __func__ << " as " << (isInput ? "input" : "output") << std::endl;
}

NlAudioAlsa::~NlAudioAlsa()
{
	close();
}

/// Error Handling
void NlAudioAlsa::throwOnAlsaError(int e, const std::string &function) const
{
	if (e < 0) {
		throw NlAudioAlsaException(e, function + ": " + snd_strerror(e));
	}
}

void NlAudioAlsa::throwOnDeviceClosed() const
{
	if (!m_deviceOpen)
		throw(NlAudioAlsaException(0, "Device is not opened, yet."));
}

void NlAudioAlsa::throwOnDeviceRunning() const
{
	if (!m_audioThread)
		throw(NlAudioAlsaException(0, "Device is not opened, yet."));
}

/// Open / Close
void NlAudioAlsa::openCommon()
{
	throwOnAlsaError(snd_pcm_open(&m_handle, m_deviceName.c_str(), m_isInput ? SND_PCM_STREAM_CAPTURE : SND_PCM_STREAM_PLAYBACK, SND_PCM_ASYNC), __func__);
	throwOnAlsaError(snd_pcm_hw_params_malloc(&m_hwParams), __func__);
	throwOnAlsaError(snd_pcm_hw_params_any(m_handle, m_hwParams), __func__);
	throwOnAlsaError(snd_pcm_hw_params_set_access(m_handle, m_hwParams, SND_PCM_ACCESS_RW_INTERLEAVED), __func__);

	throwOnAlsaError(snd_pcm_hw_params_set_channels(m_handle, m_hwParams, 2), __func__);


	m_deviceOpen = true;
}

void NlAudioAlsa::close()
{
	throwOnAlsaError(snd_pcm_close(m_handle), __func__);
	snd_pcm_hw_params_free(m_hwParams);
	m_hwParams = nullptr;

	m_deviceOpen = false;
}

/// Buffersize
void NlAudioAlsa::setBuffersize(unsigned int buffersize)
{
	// Hm not sure about that
	// PeriodeSize, is the latency, and thats what I expect to set by this value. Therefore double buffersize and use value for periodeSize
	throwOnAlsaError(snd_pcm_hw_params_set_buffer_size(m_handle, m_hwParams, static_cast<snd_pcm_uframes_t>(buffersize*2)), __func__);
	throwOnAlsaError(snd_pcm_hw_params_set_period_size(m_handle, m_hwParams, static_cast<snd_pcm_uframes_t>(buffersize), 0), __func__);
}

unsigned int NlAudioAlsa::getBuffersize()
{
	snd_pcm_uframes_t buffersize;

	throwOnAlsaError(snd_pcm_hw_params_get_buffer_size(m_hwParams, &buffersize), __func__);

	return static_cast<unsigned int>(buffersize);
}



/// Sample Rate
samplerate_t NlAudioAlsa::getSamplerate() const
{
	int dir = 0;
	samplerate_t rate = 0;

	throwOnAlsaError(snd_pcm_hw_params_get_rate(m_hwParams, &rate, &dir), __func__);

	return rate;
}

void NlAudioAlsa::setSamplerate(samplerate_t rate)
{
	throwOnDeviceClosed();

	int dir = 0;
	throwOnAlsaError(snd_pcm_hw_params_set_rate_near(m_handle, m_hwParams, &rate, &dir), __func__);
}

/// Sample Format
std::list<sampleformat_t> NlAudioAlsa::getAvailableSampleformats() const
{
	throwOnDeviceClosed();

	snd_pcm_format_mask_t *formatMask = nullptr;
	snd_pcm_format_mask_malloc(&formatMask);
	snd_pcm_hw_params_get_format_mask(m_hwParams, formatMask);

	std::list<sampleformat_t> ret;
	for (int format=0; format<=SND_PCM_FORMAT_LAST; ++format) {
		if (snd_pcm_format_mask_test(formatMask, static_cast<snd_pcm_format_t>(format)))
			//ret.push_back(snd_pcm_format_name(static_cast<snd_pcm_format_t>(format)));
			ret.insert(ret.end(), snd_pcm_format_name(static_cast<snd_pcm_format_t>(format)));
	}

	snd_pcm_format_mask_free(formatMask);
	return ret;
}

void NlAudioAlsa::setSampleFormat(sampleformat_t format)
{
	throwOnDeviceClosed();

	snd_pcm_format_t alsaFormat = snd_pcm_format_value(format.c_str());

	printf("alsaFormat=%i\n", alsaFormat);
	printf("alsaFormat=%s\n", snd_pcm_format_name(alsaFormat));

	throwOnAlsaError(snd_pcm_hw_params_set_format(m_handle, m_hwParams, alsaFormat), __func__);
}


///Channels
void NlAudioAlsa::setChannelCount(channelcount_t n)
{
	throwOnAlsaError(snd_pcm_hw_params_set_channels(m_handle, m_hwParams, n), __func__);
}

channelcount_t NlAudioAlsa::getChannelCount()
{
	channelcount_t channels = 0;
	throwOnAlsaError(snd_pcm_hw_params_get_channels(m_hwParams, &channels), __func__);
	return channels;
}



std::ostream& operator<<(std::ostream& lhs, const Statistics& rhs)
{
	lhs << "  Bytes Read From Buffer:   " << rhs.bytesReadFromBuffer << std::endl
		<< "  Bytes Written To Buffer:  " << rhs.bytesWrittenToBuffer << std::endl
		<< "  XRun Count:               " << rhs.xrunCount << std::endl;

	return lhs;
}

std::ostream& operator<<(std::ostream& lhs, const SampleSpecs& rhs)
{
	lhs << "Buffersize:                 " << rhs.buffersize << std::endl <<
		   "Bytes Per Frame:            " << rhs.bytesPerFrame << std::endl <<
		   "Bytes Per Sample:           " << rhs.bytesPerSample << std::endl <<
		   "Bytes Per Sample Stored:    " << rhs.bytesPerSampleStored << std::endl <<
		   "Channels:                   " << rhs.channels << std::endl;

	return lhs;
}

Statistics NlAudioAlsa::getStats()
{
	Statistics ret;
	m_audioBuffer->getStat(&ret.bytesReadFromBuffer, &ret.bytesWrittenToBuffer);
	ret.xrunCount = m_xrunRecoveryCounter;

	return ret;
}



///Callback
void NlAudioAlsa::process(float *in, float *out, unsigned int count)
{
	static int counter = 0;
	if (++counter % 1000000 == 0)
		std::cout << __func__ << " counter=" << counter << std::endl;

	//memcpy(out, in, count);
}

//Der input muss einen Callback rufen und dann muss ich sagen, welcher callback, falls es eine kette ist, den schreibenden weckt.
//Der schlaeft dann wiedderum  kurz wegen dem writei im idealfall, ansonsten dauert die verarbeitung zu lange.


///Callback
void NlAudioAlsa::process(char *in, char *out, unsigned int count)
{
	static int counter = 0;
	if (++counter % 1000000 == 0)
		std::cout << (in ? "In  " : "Out ") << counter << std::endl;

	//memcpy(out, in, count);
}



///Static
std::list<devicename_t> NlAudioAlsa::getAvailableDevices()
{
	int card = -1;

	std::list<devicename_t> ret;

	while (snd_card_next(&card) >= 0 && card >= 0) {

		char *name;

		if (snd_card_get_name(card, &name) == 0) {
			ret.insert(ret.end(), std::string(name));
		}
	}

	return ret;
}

///Static
int NlAudioAlsa::xrunRecovery(NlAudioAlsa *ptr, int err)
{
	//Atomic
	ptr->m_xrunRecoveryCounter++;

	if (err == -EPIPE) {    /* under-run */
		err = snd_pcm_prepare(ptr->m_handle);
		if (err < 0)
			printf("Can't recovery from underrun, prepare failed: %s\n", snd_strerror(err));
		return 0;
	} else if (err == -ESTRPIPE) {
		while ((err = snd_pcm_resume(ptr->m_handle)) == -EAGAIN)
			sleep(1);       /* wait until the suspend flag is released */
		if (err < 0) {
			err = snd_pcm_prepare(ptr->m_handle);
			if (err < 0)
				printf("Can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
		}
		return 0;
	}
	return err;
}
