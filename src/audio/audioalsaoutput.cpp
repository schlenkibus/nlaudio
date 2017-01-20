#include "audio/audioalsaoutput.h"

namespace Nl {

AudioAlsaOutput::AudioAlsaOutput(const AlsaCardIdentifier &card, SharedBufferHandle buffer) :
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
