#include <iostream>

#include "audioalsainput.h"
#include "audioalsaoutput.h"

#include "midi.h"
#include "rawmididevice.h"

#include <stdio.h>
#include <sched.h>

#include <cmath>

#include "blockingcircularbuffer.h"
#include "audiofactory.h"

using namespace std;
using namespace Nl;

void printInfos(const AudioAlsa& device)
{
	auto sampleFormats = device.getAvailableSampleformats();

	std::cout << "Sample Formats[" << sampleFormats.size() << "]:" << std::endl;

	for (auto it = sampleFormats.begin(); it != sampleFormats.end(); ++it)
		std::cout << "  " << *it << std::endl;

}


int main()
{


	try {

		auto buffer = Nl::getBuffer(512);
		auto input = Nl::getInputDevice("hw:1,0", buffer);
		auto output = Nl::getOutputDevice("hw:1,0", buffer);

		output->start();
		input->start();

		// Wait for user to exit by pressing 'q'
		char exitKey = 0;
		while(exitKey != 'q') {
			std::cout << "Alsa Output Statistics:" << std::endl;
			std::cout << output->getStats();

			std::cout << "Alsa Input Statistics:" << std::endl;
			std::cout << input->getStats();

			exitKey = getchar();
		}


	} catch (AudioAlsaException &e) {
		std::cout << "### Exception: " << std::endl << "  " << e.what();
	}


#if 0
#define DEBUG_MIDI 1

	try {
		// MIDI
		std::string firstDevice = NlRawMidiDevice::getFirstDevice();

		if (firstDevice.size()) {
			std::cout << "Opening first midi device: " << firstDevice << std::endl;
		} else {
			std::cout << "Please connect a midi device first." << std::endl;
			exit(-1);
		}

		std::shared_ptr<BlockingCircularBuffer<unsigned char>> midiBuffer(new BlockingCircularBuffer<unsigned char>(30));
		NlRawMidiDevice *midi = new NlRawMidiDevice(firstDevice, midiBuffer);
		midi->open();

		auto readMidi = [](std::shared_ptr<BlockingCircularBuffer<unsigned char>> buffer) {

				const int buffersize = 3;
				unsigned char b[buffersize];

				while(1) {
						buffer->get(b, buffersize);
						for (int i=0; i<buffersize; i++)
								printf("%02X ", (unsigned char)b[i]);

						printf("\n");
				}
		};
		std::thread midiThread(readMidi, midiBuffer);
		midi->start();

		char *nextBuffer = nullptr;

		// Audio
		std::shared_ptr<BlockingCircularBuffer<char>> audioBuffer(new BlockingCircularBuffer<char>(4096));
		NlAudioAlsaOutput alsaoutput("hw:0,0", audioBuffer);
		alsaoutput.open();
		alsaoutput.setSampleFormat("S16_LE");
		alsaoutput.setBufferCount(2);
		alsaoutput.setBuffersize(128);
		alsaoutput.setChannelCount(2);
		alsaoutput.start();

		auto writeAudio = [](std::shared_ptr<BlockingCircularBuffer<char>> audioBuffer) {

			const int samplerate = 44100;
			const int buffersize = 128;
			char silenceBuffer[buffersize];
			char sineBuffer[buffersize];
			char midiData[3];

			memset(silenceBuffer, 0, buffersize);

			while(1) {
				//if (midiBuffer->availableToRead() >= 3) {
					//midiBuffer->get(midiData, 3);
					//if (midiData[0] == 0x90) { //NoteOn
						generateSin(sineBuffer, buffersize, 440, samplerate, 2);
					//	nextBuffer = sineBuffer;
					//} else if (midiData[1] == 0x80) { //NoteOff
					//	nextBuffer = silenceBuffer;
					//}
				//} else {
				//	nextBuffer = silenceBuffer;
				//}

				// Blocks until there is storage.
				audioBuffer->set(sineBuffer, buffersize);
			}
		};

		std::thread audioThread(writeAudio, audioBuffer);



		// Wait for user to exit by pressing 'q'
		char exitKey = 0;
		while(exitKey != 'q') {
			std::cout << "Alsa Output Statistics:" << std::endl;
			//std::cout << alsaoutput.getStats();
			exitKey = getchar();
		}

	} catch (const std::exception& exception) {
		std::cout << "GrepMe1 " << exception.what() << std::endl;
	}
#endif

	return 0;
}

