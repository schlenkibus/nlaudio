#include "nlaudioalsaoutput.h"

NlAudioAlsaOutput::NlAudioAlsaOutput(const devicename_t &name) :
	basetype(name, false)
{
}

void NlAudioAlsaOutput::open()
{
	basetype::openCommon();
}

void NlAudioAlsaOutput::start()
{
	throwOnDeviceClosed();
	resetTerminateRequest();
	throwOnAlsaError(snd_pcm_hw_params(m_handle, m_hwParams));

	m_audioThread = new std::thread(NlAudioAlsaOutput::worker, this);
}

void NlAudioAlsaOutput::stop()
{
	throwOnDeviceClosed();
	setTerminateRequest();

	m_audioThread->join();
	delete m_audioThread;
	m_audioThread = nullptr;
}

//static
void NlAudioAlsaOutput::worker(NlAudioAlsaOutput *ptr)
{
	const unsigned int buffersize = ptr->getBuffersize();
	char *buffer = new char[buffersize];

	for (int i=0; i<buffersize; i++)
		buffer[i] = 0;

	std::cout << __func__ << " Output in" << std::endl;

	while(!ptr->getTerminateRequest()) {
		ptr->getBuffer(buffer, buffersize);
		//ptr->process(nullptr, buffer, buffersize);
		int ret = snd_pcm_writei(ptr->m_handle, buffer, buffersize);
		if (ret < 0)
			ptr->basetype::xrunRecovery(ptr->m_handle, ret);
	}

	delete[] buffer;

	std::cout << __func__ << " Output out" << std::endl;
}


