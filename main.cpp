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

using namespace std;

int main()
{
	try {


//		auto availableDevices = Nl::AudioAlsa::getAvailableDevices();
//		for(auto it=availableDevices.begin(); it!=availableDevices.end(); ++it) {
//			std::cout << *it << std::endl;
//		}


		exit(0);

		std::string audioOutDevice = "hw:1,0,1";
		std::string audioInDevice = "hw:1,0,0";
		std::string midiInDevice = "hw:2,0";
		const int buffersize = 5120;
		const int samplerate = 48000;

		//auto handle = Nl::Examples::inputToOutput(audioInDevice, audioOutDevice, buffersize, samplerate);
		//auto handle = Nl::Examples::silence(audioOutDevice, buffersize, samplerate);
		auto handle = Nl::Examples::midiSine(audioOutDevice, midiInDevice, buffersize, samplerate);


		// Wait for user to exit by pressing 'q'
		// Print buffer statistics on other keys
		while(getchar() != 'q') {
			if (handle.audioOutput) std::cout << "Output Statistics:" << std::endl
											  << handle.audioOutput->getStats() << std::endl;
			if (handle.audioInput) std::cout << "Input Statistics:" << std::endl
											  << handle.audioInput->getStats() << std::endl;
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

