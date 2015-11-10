#include "audioalsa.h"
#include "audio.h"
#include "tools.h"

#include <iostream>

namespace Nl {

AudioAlsa::AudioAlsa(const devicename_t &device, std::shared_ptr<BlockingCircularBuffer<u_int8_t>> buffer, bool isInput) :
	m_handle(nullptr),
	m_hwParams(nullptr),
	m_audioBuffer(buffer),
	m_deviceName(device),
	m_deviceOpen(false),
	m_isInput(isInput)
{
	std::cout << "New " << __func__ << " as " << (isInput ? "input" : "output") << std::endl;
}

AudioAlsa::~AudioAlsa()
{
	close();
}

/// Error Handling
#define THROW_ON_ALSA_ERROR(x) { throwOnAlsaError(__FILE__, __func__, __LINE__, x); }
void AudioAlsa::throwOnAlsaError(const std::string& file, const std::string& func, int line, int e) const
{
	if (e < 0) {
		throw AudioAlsaException(func, file, line, e, snd_strerror(e));
	}
}

void AudioAlsa::throwOnDeviceClosed(const std::string& file, const std::string& func, int line) const
{
	if (!m_deviceOpen)
		throw(AudioAlsaException(func, file, line, -1, "Device is not opened, yet."));
}

void AudioAlsa::throwOnDeviceRunning(const std::string &file, const std::string &func, int line) const
{
	if (!m_audioThread)
		throw(AudioAlsaException(func, file, line, -1, "Device is not opened, yet."));
}

/// Open / Close
void AudioAlsa::openCommon()
{
	THROW_ON_ALSA_ERROR(snd_pcm_open(&m_handle, m_deviceName.c_str(), m_isInput ? SND_PCM_STREAM_CAPTURE : SND_PCM_STREAM_PLAYBACK, SND_PCM_ASYNC));
	THROW_ON_ALSA_ERROR(snd_pcm_hw_params_malloc(&m_hwParams));
	THROW_ON_ALSA_ERROR(snd_pcm_hw_params_any(m_handle, m_hwParams));
	THROW_ON_ALSA_ERROR(snd_pcm_hw_params_set_access(m_handle, m_hwParams, SND_PCM_ACCESS_RW_INTERLEAVED));

	m_deviceOpen = true;
}

void AudioAlsa::close()
{
	throwOnAlsaError(__FILE__, __func__, __LINE__, snd_pcm_close(m_handle));
	snd_pcm_hw_params_free(m_hwParams);
	m_hwParams = nullptr;

	m_deviceOpen = false;
}

/// Buffersize
void AudioAlsa::setBuffersize(unsigned int buffersize)
{
	THROW_ON_ALSA_ERROR(snd_pcm_hw_params_set_buffer_size(m_handle, m_hwParams, static_cast<snd_pcm_uframes_t>(buffersize)));
}

unsigned int AudioAlsa::getBuffersize()
{
	snd_pcm_uframes_t buffersize;
	throwOnAlsaError(__FILE__, __func__, __LINE__, snd_pcm_hw_params_get_buffer_size(m_hwParams, &buffersize));

	return static_cast<unsigned int>(buffersize);
}

/// Periode Size
void AudioAlsa::setBufferCount(unsigned int buffercount)
{
	throwOnAlsaError(__FILE__, __func__, __LINE__, snd_pcm_hw_params_set_periods(m_handle, m_hwParams, buffercount, 0));
}

unsigned int AudioAlsa::getBufferCount()
{
	int dir = 0;
	unsigned int buffercount = 0;
	throwOnAlsaError(__FILE__, __func__, __LINE__, snd_pcm_hw_params_get_periods(m_hwParams, &buffercount, &dir));

	return buffercount;
}



/// Sample Rate
samplerate_t AudioAlsa::getSamplerate() const
{
	int dir = 0;
	samplerate_t rate = 0;

	throwOnAlsaError(__FILE__, __func__, __LINE__, snd_pcm_hw_params_get_rate(m_hwParams, &rate, &dir));

	return rate;
}

void AudioAlsa::setSamplerate(samplerate_t rate)
{
	throwOnDeviceClosed(__FILE__, __func__, __LINE__);

	int dir = 0;
	throwOnAlsaError(__FILE__, __func__, __LINE__, snd_pcm_hw_params_set_rate_near(m_handle, m_hwParams, &rate, &dir));
}

/// Sample Format
std::list<sampleformat_t> AudioAlsa::getAvailableSampleformats() const
{
	throwOnDeviceClosed(__FILE__, __func__, __LINE__);

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

sampleformat_t AudioAlsa::getSampleFormat() const
{
	throwOnDeviceClosed(__FILE__, __func__, __LINE__);

	snd_pcm_format_t val;

	throwOnAlsaError(__FILE__, __func__, __LINE__, snd_pcm_hw_params_get_format(m_hwParams, &val));

	return snd_pcm_format_name(val);
}

void AudioAlsa::setSampleFormat(sampleformat_t format)
{
	throwOnDeviceClosed(__FILE__, __func__, __LINE__);

	snd_pcm_format_t alsaFormat = snd_pcm_format_value(format.c_str());
	throwOnAlsaError(__FILE__, __func__, __LINE__, snd_pcm_hw_params_set_format(m_handle, m_hwParams, alsaFormat));
}


///Channels
void AudioAlsa::setChannelCount(channelcount_t n)
{
	throwOnAlsaError(__FILE__, __func__, __LINE__, snd_pcm_hw_params_set_channels(m_handle, m_hwParams, n));
}

channelcount_t AudioAlsa::getChannelCount()
{
	channelcount_t channels = 0;
	throwOnAlsaError(__FILE__, __func__, __LINE__, snd_pcm_hw_params_get_channels(m_hwParams, &channels));
	return channels;
}

///SampleSpecs
SampleSpecs_t AudioAlsa::getSpecs()
{
	snd_pcm_format_t sampleFormat;
	snd_pcm_hw_params_get_format(m_hwParams, &sampleFormat);

	SampleSpecs_t specs;
	specs.samplerate = getSamplerate();
	specs.isSigned = snd_pcm_format_signed(sampleFormat) == 1;
	specs.isLittleEndian = snd_pcm_format_little_endian(sampleFormat) == 1;
	specs.isFloat = snd_pcm_format_float(sampleFormat);

	specs.channels = getChannelCount();

	// 1 Frame in Bytes: Channels * BytesPerSample
	specs.buffersizeInFrames = getBuffersize();
	specs.buffersizeInFramesPerPeriode = specs.buffersizeInFrames / getBufferCount();

	// 1 Sample in Bytes: BytesPerSample
	specs.buffersizeInSamples = specs.buffersizeInFrames * getChannelCount();
	specs.buffersizeInSamplesPerPeriode = specs.buffersizeInSamples / getBufferCount();

	specs.bytesPerSample = snd_pcm_hw_params_get_sbits(m_hwParams) / 8;
	specs.bytesPerSamplePhysical = snd_pcm_format_physical_width(sampleFormat) / 8;
	specs.bytesPerFrame = specs.bytesPerSample * specs.channels;

	specs.buffersizeInBytes = specs.bytesPerSample * specs.channels * specs.buffersizeInFrames;
	specs.buffersizeInBytesPerPeriode = specs.buffersizeInBytes / getBufferCount();

	return specs;
}

std::ostream& operator<<(std::ostream& lhs, const Statistics& rhs)
{
	lhs << "  Bytes Read From Buffer:   " << rhs.bytesReadFromBuffer << std::endl
		<< "  Bytes Written To Buffer:  " << rhs.bytesWrittenToBuffer << std::endl
		<< "  XRun Count:               " << rhs.xrunCount << std::endl;

	return lhs;
}

Statistics AudioAlsa::getStats()
{
	Statistics ret;
	m_audioBuffer->getStat(&ret.bytesReadFromBuffer, &ret.bytesWrittenToBuffer);
	ret.xrunCount = m_xrunRecoveryCounter;

	return ret;
}

///Static
std::list<devicename_t> AudioAlsa::getAvailableDevices()
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
int AudioAlsa::xrunRecovery(AudioAlsa *ptr, int err)
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
				printf("Can't recover from suspend, prepare failed: %s\n", snd_strerror(err));
		}
		return 0;
	}
	return err;
}

} // namespace Nl
