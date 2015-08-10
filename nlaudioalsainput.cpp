#include "nlaudioalsainput.h"

NlAudioAlsaInput::NlAudioAlsaInput(const devicename_t &name) :
	basetype(name, true)
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
	throwOnAlsaError(snd_pcm_hw_params(m_handle, m_hwParams));

	m_audioThread = new std::thread(NlAudioAlsaInput::worker, this);
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
void NlAudioAlsaInput::worker(NlAudioAlsaInput *ptr)
{
	const unsigned int buffersize = ptr->getBuffersize();
	char *buffer = new char[buffersize];


	ptr->createBuffer(buffersize);

	std::cout << __func__ << " Input in" << std::endl;

	// TODO: Hier muss de processierte buffer verwendet werden
	while(!ptr->getTerminateRequest()) {
		int ret = snd_pcm_readi(ptr->m_handle, buffer, buffersize);

		if (ret < 0)
			ptr->basetype::xrunRecovery(ptr->m_handle, ret);

		ptr->setBuffer(buffer, buffersize);
		//ptr->process(buffer, nullptr, buffersize);
	}

	ptr->destroyBuffer();

	std::cout << __func__ << " Input out" << std::endl;
}
