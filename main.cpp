#include <iostream>

#include "nlaudioalsainput.h"
#include "nlaudioalsaoutput.h"
#include "sampleformatconverter.h"

#include <stdio.h>
#include <sched.h>

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

	std::cout << "Seting up Realtime Scheduling..." << std::endl;

	struct sched_param param;
	param.sched_priority = 49;
	int ret;
	if((ret = sched_setscheduler(0, SCHED_FIFO, &param)) < 0) {
		std::cout << "sched_setscheduler failed (" << ret << ")" << std::endl;
	} else {
		std::cout << "RT Initialized!" << std::endl;
	}

	try {

		std::shared_ptr<AudioBuffer> buffer(new AudioBuffer(4096));

		const int buffersize = 512;
		std::string sampleFormat = "S24_3LE";
		//std::string sampleFormat = "S16_LE";

		NlAudioAlsaInput alsainput("hw:1,0", buffer);
		alsainput.open();
		printInfos(alsainput);

		NlAudioAlsaOutput alsaoutput("hw:1,0", buffer);
		alsaoutput.open();
		printInfos(alsaoutput);

		alsainput.setSampleFormat(sampleFormat);
		alsainput.setBuffersize(buffersize);
		alsainput.setSamplerate(48000);
		alsainput.setChannelCount(2);

		alsaoutput.setSampleFormat(sampleFormat);
		alsaoutput.setBuffersize(buffersize);
		alsaoutput.setSamplerate(48000);
		alsaoutput.setChannelCount(2);

		std::cout << "### INPUT ###" << std::endl;
		std::cout << "Buffersize=" << alsainput.getBuffersize() << std::endl;
		std::cout << "### OUTPUT ###" << std::endl;
		std::cout << "Buffersize=" << alsaoutput.getBuffersize() << std::endl;

		std::cout << "snd_pcm_state(alsainput.m_handle): " << snd_pcm_state(alsainput.m_handle) << std::endl;
		std::cout << "snd_pcm_state(alsaoutput.m_handle): " << snd_pcm_state(alsaoutput.m_handle) << std::endl;

		alsainput.start();
		alsaoutput.start();

		char exitKey = 0;

		while(exitKey != 'q') {

			std::cout << "### Input ###" << std::endl;
			std::cout << alsainput.getStats();
			std::cout << "### Output ###" << std::endl;
			std::cout << alsaoutput.getStats();

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
