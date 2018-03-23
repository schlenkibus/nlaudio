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

#include "audio/audioalsaoutput.h"

namespace Nl {

AudioAlsaOutput::AudioAlsaOutput(const AlsaAudioCardIdentifier &card, SharedBufferHandle buffer) :
	basetype(card, buffer, false)
{
}

void AudioAlsaOutput::open()
{
	basetype::openCommon();
}

void AudioAlsaOutput::start()
{
	throwOnDeviceClosed(__FILE__, __func__, __LINE__);
	resetTerminateRequest();
	throwOnAlsaError(__FILE__, __func__, __LINE__, snd_pcm_hw_params(m_handle, m_hwParams));

	init();

	SampleSpecs specs = basetype::getSpecs();
	std::cout << "NlAudioAlsaOutput Specs: " << std::endl << specs;

	m_audioThread = new std::thread(AudioAlsaOutput::worker, specs, this);
}

void AudioAlsaOutput::stop()
{
	throwOnDeviceClosed(__FILE__, __func__, __LINE__);
	setTerminateRequest();

	m_audioThread->join();
	delete m_audioThread;
	m_audioThread = nullptr;
}

void AudioAlsaOutput::init()
{
	throwOnDeviceClosed(__FILE__, __func__, __LINE__);
	throwOnAlsaError(__FILE__, __func__, __LINE__, snd_pcm_hw_params(m_handle, m_hwParams));

	basetype::m_audioBuffer->init(basetype::getSpecs());
}

//static
void AudioAlsaOutput::worker(SampleSpecs specs, AudioAlsaOutput *ptr)
{
	u_int8_t *buffer = new u_int8_t[specs.buffersizeInBytesPerPeriode];
	memset(buffer, 0, specs.buffersizeInBytesPerPeriode);

	while(!ptr->getTerminateRequest()) {
		// Might block, if nothing to read
		ptr->basetype::m_audioBuffer->get(buffer, specs.buffersizeInBytesPerPeriode);
		int ret = snd_pcm_writei(ptr->m_handle, buffer, specs.buffersizeInFramesPerPeriode);
		if (ret < 0)
			ptr->basetype::xrunRecovery(ptr, ret);
		else if (ret != static_cast<int>(specs.buffersizeInFramesPerPeriode))
			std::cout << "Only wrote " << ret << " of " << specs.buffersizeInFramesPerPeriode << " from output device" << std::endl;
	}

	snd_pcm_abort(ptr->m_handle);
	delete[] buffer;

	std::cout << "void AudioAlsaOutput::worker(SampleSpecs specs, AudioAlsaInput *ptr)" << std::endl;
}

} // namespace Nl
