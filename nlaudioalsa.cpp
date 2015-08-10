#include "nlaudioalsa.h"
#include "nlaudiointerface.h"

#include <iostream>

NlAudioAlsa::NlAudioAlsa(const devicename_t &device, bool isInput) :
	m_handle(nullptr),
	m_hwParams(nullptr),
	m_deviceName(device),
	m_deviceOpen(false),
	m_isInput(isInput)
{
	std::cout << "New " << __func__ << " as " << (isInput ? "input" : "output") << std::endl;
}

NlAudioAlsa::~NlAudioAlsa()
{
	close();
}

/// Error Handling
void NlAudioAlsa::throwOnAlsaError(int e) const
{
	if (e < 0) {
		throw NlAudioAlsaException(e, snd_strerror(e));
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
	throwOnAlsaError(snd_pcm_open(&m_handle, m_deviceName.c_str(), m_isInput ? SND_PCM_STREAM_CAPTURE : SND_PCM_STREAM_PLAYBACK, SND_PCM_ASYNC));
	throwOnAlsaError(snd_pcm_hw_params_malloc(&m_hwParams));
	throwOnAlsaError(snd_pcm_hw_params_any(m_handle, m_hwParams));
	throwOnAlsaError(snd_pcm_hw_params_set_access(m_handle, m_hwParams, SND_PCM_ACCESS_RW_INTERLEAVED));

	throwOnAlsaError(snd_pcm_hw_params_set_channels(m_handle, m_hwParams, 2));


	m_deviceOpen = true;
}

void NlAudioAlsa::close()
{
	throwOnAlsaError(snd_pcm_close(m_handle));
	snd_pcm_hw_params_free(m_hwParams);
	m_hwParams = nullptr;

	m_deviceOpen = false;
}

/// Buffersize
void NlAudioAlsa::setBuffersize(unsigned int buffersize)
{
	throwOnAlsaError(snd_pcm_hw_params_set_buffer_size(m_handle, m_hwParams, static_cast<snd_pcm_uframes_t>(buffersize)));
}

unsigned int NlAudioAlsa::getBuffersize()
{
	snd_pcm_uframes_t buffersize;

	throwOnAlsaError(snd_pcm_hw_params_get_buffer_size(m_hwParams, &buffersize));

	return static_cast<unsigned int>(buffersize);
}



/// Sample Rate
samplerate_t NlAudioAlsa::getSamplerate() const
{
	int dir = 0;
	samplerate_t rate = 0;

	throwOnAlsaError(snd_pcm_hw_params_get_rate(m_hwParams, &rate, &dir));

	return rate;
}

void NlAudioAlsa::setSamplerate(samplerate_t rate)
{
	throwOnDeviceClosed();

	int dir = 0;
	throwOnAlsaError(snd_pcm_hw_params_set_rate_near(m_handle, m_hwParams, &rate, &dir));
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

	//throwOnAlsaError(snd_pcm_hw_params_set_format(m_handle, m_hwParams, alsaFormat));
	throwOnAlsaError(snd_pcm_hw_params_set_format(m_handle, m_hwParams, SND_PCM_FORMAT_S16));


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

void NlAudioAlsa::createBuffer(unsigned int size)
{
	m_bufferMutex.lock();
	m_buffer = new char[size];
	memset(m_buffer, 0, size);
	m_bufferMutex.unlock();
}

void NlAudioAlsa::destroyBuffer()
{
	//TODO: The buffer is deleted, even if alsa needs it a little longer!

	delete[] m_buffer;
	m_buffer = nullptr;
}

void NlAudioAlsa::getBuffer(char *buffer, unsigned int size)
{
	m_bufferMutex.lock();
	m_bufferReadStat++;
	memcpy(buffer, m_buffer, size);
	m_bufferMutex.unlock();
}

void NlAudioAlsa::setBuffer(char *buffer, unsigned int size)
{
	m_bufferMutex.lock();
	m_bufferWriteStat++;
	memcpy(m_buffer, buffer, size);
	m_bufferMutex.unlock();
}

void NlAudioAlsa::getBufferStat(unsigned int *reades, unsigned int writes)
{
	m_bufferMutex.lock();
	*reades = m_bufferReadStat;
	*writes = m_bufferWriteStat;
	m_bufferMutex.unlock();
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
int NlAudioAlsa::xrunRecovery(snd_pcm_t *handle, int err)
{

	std::cout << "Recovery! " << std::endl;

	if (err == -EPIPE) {    /* under-run */
		err = snd_pcm_prepare(handle);
		if (err < 0)
			printf("Can't recovery from underrun, prepare failed: %s\n", snd_strerror(err));
		return 0;
	} else if (err == -ESTRPIPE) {
		while ((err = snd_pcm_resume(handle)) == -EAGAIN)
			sleep(1);       /* wait until the suspend flag is released */
		if (err < 0) {
			err = snd_pcm_prepare(handle);
			if (err < 0)
				printf("Can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
		}
		return 0;
	}
	return err;
}
