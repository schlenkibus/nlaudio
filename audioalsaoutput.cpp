#include "audioalsaoutput.h"

namespace Nl {

AudioAlsaOutput::AudioAlsaOutput(const devicename_t &name, std::shared_ptr<BlockingCircularBuffer<char> > buffer) :
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

	snd_pcm_format_t sampleFormat;
	snd_pcm_hw_params_get_format(m_hwParams, &sampleFormat);

	unsigned int channels = 0;
	throwOnAlsaError(__FILE__, __func__, __LINE__, snd_pcm_hw_params_get_channels(m_hwParams, &channels));

	SampleSpecs specs;
	specs.channels = channels;
	specs.buffersizeInFrames = getBuffersize();
	specs.buffersizeInFramesPerPeriode = getBuffersize() / getBufferCount();
	specs.bytesPerSample = snd_pcm_hw_params_get_sbits(m_hwParams) / 8;
	specs.bytesPerFrame = specs.bytesPerSample * specs.channels;
	specs.buffersizeInBytes = specs.bytesPerSample * specs.channels * specs.buffersizeInFrames;
	specs.buffersizeInBytesPerPeriode = specs.buffersizeInBytes / getBufferCount();
	//std::cout << "NlAudioAlsaOutput Specs: " << std::endl << specs;

	basetype::m_audioBuffer->resize(specs.buffersizeInBytes);

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
void AudioAlsaOutput::worker(SampleSpecs specs, AudioAlsaOutput *ptr)
{
	char *buffer = new char[specs.buffersizeInBytesPerPeriode];
	memset(buffer, 0, specs.buffersizeInBytesPerPeriode);

	//std::cout << __func__ << " Output in" << std::endl;

	while(!ptr->getTerminateRequest()) {
		// Might block, if nothing to read
		ptr->basetype::m_audioBuffer->get(buffer, specs.buffersizeInBytesPerPeriode);
		int ret = snd_pcm_writei(ptr->m_handle, buffer, specs.buffersizeInFramesPerPeriode);
		if (ret < 0)
			ptr->basetype::xrunRecovery(ptr, ret);
		else if (ret != specs.buffersizeInFramesPerPeriode)
			std::cout << "### FIXME ###" << std::endl;
	}

	snd_pcm_abort(ptr->m_handle);

	delete[] buffer;
	//std::cout << __func__ << " Output out" << std::endl;
}

} // namespace Nl
