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

		std::string audioOutDevice = "hw:1,0,0";
		std::string audioInDevice = "hw:1,0,0";
		std::string midiInDevice = "hw:2,0";
		const int buffersize = 512;
		const int samplerate = 48000;

		//auto handle = Nl::Examples::midiSine(audioOutDevice, midiInDevice, buffersize, samplerate);
		auto handle = Nl::Examples::inputToOutput(audioInDevice, audioOutDevice, buffersize, samplerate);

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

