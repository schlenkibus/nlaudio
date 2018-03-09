/***
  Copyright (c) 2018 Nonlinear Labs GmbH

  Authors: Pascal Huerst <pascal.huerst@gmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
***/

#include <iostream>

#include "audio/audioalsa.h"
#include "audio/audioalsaexception.h"

namespace Nl {

/** \ingroup Audio
 *
 * \brief Constructor
 * \param device Alsa device id such as "hw:0,1"
 * \param buffer
 * \param isInput If true, the device is opened as input, otherwise as output.
 *
 * Constructor for AudioAlsa
 *
*/
AudioAlsa::AudioAlsa(const AlsaCardIdentifier& card, SharedBufferHandle buffer, bool isInput) :
	m_handle(nullptr),
	m_hwParams(nullptr),
	m_xrunRecoveryCounter(0),
	m_audioBuffer(buffer),
	m_card(card),
	m_deviceOpen(false),
	m_isInput(isInput)

{
	std::cout << "New " << __func__ << " as " << (isInput ? "input" : "output") << std::endl;
}

/** \ingroup Audio
 *
 * \brief Destructor
 *
 * Destructor for AudioAlsa
 *
*/
AudioAlsa::~AudioAlsa()
{
	close();
}

/** \ingroup Audio
 *
 * \brief Throws a \ref AudioAlsaException on error
 * \param x Call to alsa library function
 *
 * This macro adds the file name, the function name and the line number to the \ref throwOnAlsaError() call.
 *
 * \code
 * THROW_ON_ALSA_ERROR(snd_pcm_hw_params_set_access(m_handle, m_hwParams, SND_PCM_ACCESS_RW_INTERLEAVED));
 * \endcode
 *
 *
 */
#define THROW_ON_ALSA_ERROR(x) { throwOnAlsaError(__FILE__, __func__, __LINE__, x); }

/** \ingroup Audio
 *
 * \brief Generates and throws a \ref AudioAlsaException, if \a e < 0
 * \param file File name where the error happened
 * \param func Function name where the error happened
 * \param line Line number of where the error happened
 * \param e Error number
 * \throw AudioAlsaException is thrown if \a e < 0
 *
 * This function checks the return value of an alsa library call and throws an \ref AudioAlsaException, if
 * the return value, which has to be passed to the function in \a e, is < 0
 *
 * See helper macro \ref THROW_ON_ALSA_ERROR about usage.
 *
 */
void AudioAlsa::throwOnAlsaError(const std::string& file, const std::string& func, int line, int e) const
{
	if (e < 0) {
		throw AudioAlsaException(func, file, line, e, snd_strerror(e));
	}
}

/** \ingroup Audio
 *
 * \brief Generates and throws a \ref AudioAlsaException, if the device has not been opened.
 * \param file File name where the error happened
 * \param func Function name where the error happened
 * \param line Line number of where the error happened
 * \throw AudioAlsaException is thrown if the device has not beend opened
 *
 * This function checks if the device has been opened and throws a \ref AudioAlsaException if not.
 *
 */
void AudioAlsa::throwOnDeviceClosed(const std::string& file, const std::string& func, int line) const
{
	if (!m_deviceOpen)
		throw(AudioAlsaException(func, file, line, -1, "Device is not opened, yet."));
}

/** \ingroup Audio
 *
 * \brief Generates and throws a \ref AudioAlsaException, if the device has not been started.
 * \param file File name where the error happened
 * \param func Function name where the error happened
 * \param line Line number of where the error happened
 * \throw AudioAlsaException is thrown if the device has not beend opened
 *
 * This function checks if the device has been started and throws a \ref AudioAlsaException if not.
 *
 */
void AudioAlsa::throwOnDeviceRunning(const std::string &file, const std::string &func, int line) const
{
	if (!m_audioThread)
		throw(AudioAlsaException(func, file, line, -1, "Device is not opened, yet."));
}

/** \ingroup Audio
 *
 * \brief Common open function, used by AudioAlsaInput::open() and AudioAlsaOutput::open()
 * \throw AudioAlsaException is thrown on error.
 *
 * This function is called by the input/output derivates in their open() function. See:
 *  - AudioAlsaInput::open()
 *  - AudioAlsaOutput::open()
 *
 */
void AudioAlsa::openCommon()
{
	THROW_ON_ALSA_ERROR(snd_pcm_open(&m_handle, m_card.getCardString().c_str(), m_isInput ? SND_PCM_STREAM_CAPTURE : SND_PCM_STREAM_PLAYBACK, SND_PCM_ASYNC));
	THROW_ON_ALSA_ERROR(snd_pcm_hw_params_malloc(&m_hwParams));
	THROW_ON_ALSA_ERROR(snd_pcm_hw_params_any(m_handle, m_hwParams));
	THROW_ON_ALSA_ERROR(snd_pcm_hw_params_set_access(m_handle, m_hwParams, SND_PCM_ACCESS_RW_INTERLEAVED));

	m_deviceOpen = true;
}

/** \ingroup Audio
 *
 * \brief Closes the device
 * \throw AudioAlsaException is thrown on error.
 *
 * This function closed the audio device. On error a \ref throwOnAlsaError() is thrown
 *
 */
void AudioAlsa::close()
{
	throwOnAlsaError(__FILE__, __func__, __LINE__, snd_pcm_close(m_handle));
	snd_pcm_hw_params_free(m_hwParams);
	m_hwParams = nullptr;

	m_deviceOpen = false;
}

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

/** \ingroup Audio
 *
 * \brief Common open function, used by AudioAlsaInput::open() and AudioAlsaOutput::open()
 * \throw AudioAlsaException is thrown on error.
 *
 * This function is called by the input/output derivates in their open() function. See:
 *  - AudioAlsaInput::open()
 *  - AudioAlsaOutput::open()
 *
 */
SampleSpecs AudioAlsa::getSpecs()
{
	//TODO: snd_pcm_hw_params_get_sbits might return negative errorcode, which leads to weird segfaults

	snd_pcm_format_t sampleFormat;
	snd_pcm_hw_params_get_format(m_hwParams, &sampleFormat);

	SampleSpecs specs;
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

	//std::cout << snd_pcm_hw_params_get_sbits(m_hwParams)  << " " << specs.bytesPerSample << "   " << specs.channels << "   " << specs.buffersizeInFrames << std::endl;

	specs.buffersizeInBytes = specs.bytesPerSample * specs.channels * specs.buffersizeInFrames;
	specs.buffersizeInBytesPerPeriode = specs.buffersizeInBytes / getBufferCount();

	specs.latency = static_cast<double>(specs.buffersizeInFramesPerPeriode) /
			static_cast<double>(specs.samplerate) * 1000.0;

	return specs;
}

/** \ingroup Audio
 *
 * \brief Returns current buffer statistics on interface
 *
 * This function returns a \ref BufferStatistics object. This can be used to print
 * infromation on buffer access and Over-/Underruns:
 * \code{.cpp}
 *  std::cout << "Statistics: " << std::endl << handle->getStats() << std::endl;
 * \endcode
 */
BufferStatistics AudioAlsa::getStats()
{
	BufferStatistics ret;
	m_audioBuffer->getStat(&ret.bytesReadFromBuffer, &ret.bytesWrittenToBuffer);
	ret.xrunCount = m_xrunRecoveryCounter;

	return ret;
}

/** \ingroup Audio
 *
 * \brief Static function, that recovers the interface from buffer over-/underruns
 * \param ptr A pointer to an AudioAlsa instance
 * \param err The error number, which indicates what has to be fixed.
 * \return An error number
 *
 * Static function, that recovers from errors such as over-/underflows and returns an error code.
 *
 */
int AudioAlsa::xrunRecovery(AudioAlsa *ptr, int err)
{
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
