/*! \mainpage NlAudio - A lightwight C++ wrapper for alsa and jack
 *
 * \section intro_sec This documentation is split into the following subsections:
 * \ref Audio\n
 * \ref Midi\n
 * \ref Tools\n
 * \ref Factory\n
 * \n
 * More has to be done, but first a want to make a proper library from that
 *
 * \section install_sec Installation
 *
 * sudo make install blub und so
 *
 * \subsection step1 Step 1: Opening the box
 *
 * This could be a subsection...
 *
 */

#include <iostream>
#include <ostream>

#include "audioalsainput.h"
#include "audioalsaoutput.h"

#include "midi.h"
#include "rawmididevice.h"

#include <stdio.h>

#include "audiofactory.h"
#include "examples.h"

#include "tools.h"

#include "audioalsa.h"

#include "stopwatch.h"

#include "alsacardidentifier.h"

#include "blockingcircularbuffer.h"

#include "audioalsaexception.h"

#include "alsacardidentifier.h"

#include "samplespecs.h"


#include "audiojack.h"
#include "audiojackinput.h"

using namespace std;

//TODO: Glbal Variables are bad (even in a namespace)
Nl::StopWatch sw("AudioCallback");

int main()
{
	try {

		/*
		auto availableDevices = Nl::getDetailedCardInfos();
		for(auto it=availableDevices.begin(); it!=availableDevices.end(); ++it)
			std::cout << *it << std::endl;

		auto availableDevs = Nl::AlsaCardIdentifier::getCardIdentifiers();
		for (auto it=availableDevs.begin(); it!=availableDevs.end(); ++it)
			std::cout << *it << std::endl;
		*/

		Nl::AlsaCardIdentifier audioOut(8,0,0, "USB Device");
		Nl::AlsaCardIdentifier audioIn(1,0,0, "USB Device");
		Nl::AlsaCardIdentifier midiIn(2,0,0, "Midi In");
#if 1
//Vamp

#endif


#if 0
		//Jack Tests
		auto jackBuffer = Nl::createBuffer("JackBuffer");
		Nl::Audio *jackOut = new Nl::AudioJack(audioIn, jackBuffer, true);

		jackOut->start();
#endif
#if 0

		// Try to get realtime prio for this process
		int ret = Nl::requestRealtime();
		if (ret != 0) {
			std::cout << "Could not anable realtime. (" << ret  << ")" << std::endl;
			exit(-1);
		} else {
			std::cout << "Realtime: Up and running" << std::endl;
		}
#endif

		const int buffersize = 256;
		const int samplerate = 48000;
		const int channels = 2;

		//last param: fixedtempo, percussiononsets
		auto handle = Nl::Examples::vampPlugin(audioIn, channels, buffersize, samplerate, "vamp-aubio", "aubiotempo");
		//auto handle = Nl::Examples::onsetDetection(audioIn, buffersize, samplerate);
		//auto handle = Nl::Examples::jackInputToOutput(audioIn, audioOut, buffersize, samplerate);
		//auto handle = Nl::Examples::inputToOutput(audioIn, audioOut, buffersize, samplerate);
		//auto handle = Nl::Examples::inputToOutputWithMidi(audioIn, audioOut, midiIn, buffersize, samplerate);
		//auto handle = Nl::Examples::silence(audioOutDevice, buffersize, samplerate);
		//auto handle = Nl::Examples::midiSine(audioOut, midiIn, buffersize, samplerate);

		// Wait for user to exit by pressing 'q'
		// Print buffer statistics on other keys
		// TODO: We might have a deadlock here:
		//		 sw.printSummary() holds a lock
		//		 audioXX->getStats() holds a lock
		//		 inMidiBuffer->getStats() holds a lock
		//	     The calls should happen in this order. Otherwise we trigger
		//		 a deadlock with the audio callback.

		std::cout << "#### GGGG ####" << std::endl;

		//while(true) {
		while(getchar() != 'q') {
			std::cout << sw << std::endl;

//			if (handle.audioOutput) std::cout << "Audio: Output Statistics:" << std::endl
//											  << handle.audioOutput  ->getStats() << std::endl;
//			if (handle.audioInput) std::cout << "Audio: Input Statistics:" << std::endl
//											 << handle.audioInput->getStats() << std::endl;

			if (handle.inMidiBuffer) {
				unsigned long rxBytes, txBytes;
				handle.inMidiBuffer->getStat(&rxBytes, &txBytes);
				std::cout << "Midi: Input Statistics:" << std::endl
						  << "rxBytes=" << rxBytes << "  txBytes=" << txBytes << std::endl;
			}

			std::cout << "BufferCount: " << handle.audioInput->getBufferCount() << std::endl;

			//std::this_thread::sleep_for(std::chrono::milliseconds(250));
		}

		// Tell worker thread to cleanup and quit
		Nl::terminateWorkingThread(handle.workingThreadHandle);
		if (handle.audioInput) handle.audioInput->stop();
		if (handle.audioOutput) handle.audioOutput->stop();

	} catch (Nl::AudioAlsaException& e) {
		std::cout << "### Exception ###" << std::endl << "  " << e.what() << std::endl;
	} catch (std::exception& e) {
		std::cout << "### Exception ###" << std::endl << "  " << e.what() << std::endl;
	} catch(...) {
		std::cout << "### Exception ###" << std::endl << "  default" << std::endl;
	}

	return 0;
}

