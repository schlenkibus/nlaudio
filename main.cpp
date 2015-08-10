#include <iostream>

#include "nlaudioalsainput.h"
#include "nlaudioalsaoutput.h"

#include <stdio.h>

using namespace std;

void printInfos(const NlAudioAlsa& device)
{
	auto sampleFormats = device.getAvailableSampleformats();

	std::cout << "Sample Formats[" << sampleFormats.size() << "]:" << std::endl;

	for (auto it = sampleFormats.begin(); it != sampleFormats.end(); ++it)
		std::cout << "  " << *it << std::endl;

}

int main()
{
	auto deviceList = NlAudioAlsa::getAvailableDevices();
	for (auto it = deviceList.begin(); it != deviceList.end(); ++it)
		std::cout << *it << std::endl;

	std::cout << std::endl;

	try {

		NlAudioAlsaInput alsainput("hw:0,0");
		alsainput.open();
		alsainput.setSampleFormat("S16_LE");
		alsainput.setBuffersize(512);
		alsainput.setSamplerate(44100);

		NlAudioAlsaOutput alsaoutput("hw:0,0");
		alsaoutput.open();
		alsaoutput.setSampleFormat("S16_LE");
		alsaoutput.setBuffersize(512);
		alsaoutput.setSamplerate(44100);

		std::cout << "### INPUT ###" << std::endl;
		printInfos(alsainput);
		std::cout << "Buffersize=" << alsainput.getBuffersize() << std::endl;

		std::cout << "### OUTPUT ###" << std::endl;
		printInfos(alsaoutput);
		std::cout << "Buffersize=" << alsaoutput.getBuffersize() << std::endl;

		std::cout << "snd_pcm_state(alsainput.m_handle): " << snd_pcm_state(alsainput.m_handle) << std::endl;
		std::cout << "snd_pcm_state(alsaoutput.m_handle): " << snd_pcm_state(alsaoutput.m_handle) << std::endl;

		alsainput.start();
		alsaoutput.start();

		char exitKey = 0;

		while(exitKey != 'q') {

			std::cout << "running..." << std::endl;

			std::cout << "snd_pcm_state(alsainput.m_handle): " << snd_pcm_state(alsainput.m_handle) << std::endl;
			std::cout << "snd_pcm_state(alsaoutput.m_handle): " << snd_pcm_state(alsaoutput.m_handle) << std::endl;

			exitKey = getchar();
		}

		std::cout << "NEVER GET HERE!!!" << std::endl;

		alsainput.stop();
		alsaoutput.stop();

	} catch (const NlAudioAlsaException& exception) {
		std::cout << "GrepMe1 " << exception.what() << std::endl;
	}

	return 0;
}
