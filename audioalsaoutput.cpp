#include "audioalsaoutput.h"

namespace Nl {

AudioAlsaOutput::AudioAlsaOutput(const devicename_t &name, std::shared_ptr<BlockingCircularBuffer<u_int8_t> > buffer) :
	basetype(name, buffer, false)
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

	SampleSpecs_t specs = basetype::getSpecs();

	basetype::m_audioBuffer->init(specs);
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

//static
void AudioAlsaOutput::worker(SampleSpecs_t specs, AudioAlsaOutput *ptr)
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
			std::cout << "This should only happen, when stopping the device!" << std::endl;
	}

	snd_pcm_abort(ptr->m_handle);
	delete[] buffer;
}

} // namespace Nl
