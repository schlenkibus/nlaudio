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

#include "audioalsa.h" //AlsaCardIdentifier -> TODO: Put this in extra file

#include "stopwatch.h"

using namespace std;


Nl::StopWatch sw;

int main()
{
	try {


		auto availableDevices = Nl::AudioAlsa::getDetailedCardInfos();
		for(auto it=availableDevices.begin(); it!=availableDevices.end(); ++it) {
			std::cout << *it << std::endl;
		}

		auto availableDevs = Nl::AudioAlsa::getCardIdentifiers();
		for (auto it=availableDevs.begin(); it!=availableDevs.end(); ++it)
			std::cout << *it << std::endl;

		Nl::AlsaCardIdentifier audioOut(2,0,0, "USB Device");
		Nl::AlsaCardIdentifier midiIn(1,0,0, "Midi In");


		const int buffersize = 256;
		const int samplerate = 48000;

		//auto handle = Nl::Examples::inputToOutput(audioInDevice, audioOutDevice, buffersize, samplerate);
		//auto handle = Nl::Examples::silence(audioOutDevice, buffersize, samplerate);
		auto handle = Nl::Examples::midiSine(audioOut, midiIn, buffersize, samplerate);

		// Wait for user to exit by pressing 'q'
		// Print buffer statistics on other keys
		while(getchar() != 'q') {
			if (handle.audioOutput) std::cout << "Audio: Output Statistics:" << std::endl
											  << handle.audioOutput->getStats() << std::endl;
			if (handle.audioInput) std::cout << "Audio: Input Statistics:" << std::endl
											  << handle.audioInput->getStats() << std::endl;

			unsigned long rxBytes, txBytes;
			handle.inMidiBuffer->getStat(&rxBytes, &txBytes);

			//if (handle.inMidiBuffer) std::cout << "Midi: Input Statistics:" << std::endl
			//								  << "rxBytes=" << rxBytes << "  txBytes=" << txBytes << std::endl;

			sw.printSummary();
			//sw.clear();
		}

		// Tell worker thread to cleanup and quit
		Nl::terminateWorkingThread(handle.workingThreadHandle);



	} catch (Nl::AudioAlsaException& e) {
		std::cout << "### Exception ###" << std::endl << "  " << e.what() << std::endl;
	} catch (std::exception& e) {
		std::cout << "### Exception ###" << std::endl << "  " << e.what() << std::endl;
	} catch(...) {
		std::cout << "### Exception ###" << std::endl << "  default" << std::endl;
	}

	return 0;
}

