#include "nlaudioalsainput.h"

NlAudioAlsaInput::NlAudioAlsaInput(const devicename_t &name, std::shared_ptr<AudioBuffer> buffer) :
	basetype(name, buffer, true)
{
}

void NlAudioAlsaInput::open()
{
	basetype::openCommon();
}

void NlAudioAlsaInput::start()
{
	throwOnDeviceClosed();
	resetTerminateRequest();
	throwOnAlsaError(snd_pcm_hw_params(m_handle, m_hwParams), __func__);

	SampleSpecs specs;
	specs.buffersize = getBuffersize();
	specs.bytesPerFrame = snd_pcm_frames_to_bytes(m_handle, 1);
	specs.bytesPerSample = snd_pcm_hw_params_get_sbits(m_hwParams) / 8;

	snd_pcm_format_t sampleFormat;
	snd_pcm_hw_params_get_format(m_hwParams, &sampleFormat);

	specs.bytesPerSampleStored = 4;//snd_pcm_format_physical_width(sampleFormat);

	unsigned int channels = 0;
	throwOnAlsaError(snd_pcm_hw_params_get_channels(m_hwParams, &channels), __func__);
	specs.channels = channels;

	std::cout << "NlAudioAlsaInput: " << std::endl << specs;

	m_audioThread = new std::thread(NlAudioAlsaInput::worker, specs, this);
}

void NlAudioAlsaInput::stop()
{
	throwOnDeviceClosed();
	setTerminateRequest();

	m_audioThread->join();
	delete m_audioThread;
	m_audioThread = nullptr;
}

//static
void NlAudioAlsaInput::worker(SampleSpecs specs, NlAudioAlsaInput *ptr)
{
	// Alsa: 1 Sample = 1-4 Bytes, depending on Sampleformat
	//		 1 Frame  = nChannels * Sample
	//		 1 Buffer = with size 5120 has 512*Frame size

	// I have: S24_3LE, which is 3 Bytes per Samples (Stored in 4!)
	// I have 2 Channels, which leads to 2 Samples per Frame
	// I have 512 BufferSize, which ends up:
	// 3 Bytes / Sample * 2 Samples(Channels) / Frame * 512 = 3072

	//snd_pcm_hw_params_get_sbits() liefert wieviele bits verwendet werden fuer sampleformat
	// Ok, ich will immer float!

	// Buffersize in terms of bytes must be multiplied by bytes per sample.
	const int buffersizeInBytes = specs.buffersize * specs.channels * specs.bytesPerSampleStored;
	char *buffer = new char[buffersizeInBytes];

	std::cout << __func__ << " Input in" << std::endl;

	while(!ptr->getTerminateRequest()) {
		int ret = snd_pcm_readi(ptr->m_handle, buffer, specs.buffersize);

		if (ret < 0)
			ptr->basetype::xrunRecovery(ptr, ret);

		// Might block, if read in progress
		ptr->basetype::m_audioBuffer->set(buffer, buffersizeInBytes);

	}

	snd_pcm_abort(ptr->m_handle);

	delete[] buffer;
	std::cout << __func__ << " Input out" << std::endl;
}
