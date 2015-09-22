#include <iostream>

#include "audioalsainput.h"
#include "audioalsaoutput.h"

#include "midi.h"
#include "rawmididevice.h"

#include <stdio.h>
#include <sched.h>

#include <cmath>
#include <sys/resource.h> // Nicelevel

#include "blockingcircularbuffer.h"
#include "audiofactory.h"
#include "examples.h"

using namespace std;
using namespace Nl;

int main()
{

#if 0
	// Changing priority only works as root
	int which = PRIO_PROCESS;
	id_t pid;
	int priority = -20;

	pid = getpid();
	int ret = setpriority(which, pid, priority);
#endif

	try {

		//Nl::Examples::ExamplesHandle_t handle = Nl::Examples::inputToOutput("hw:1,0", "hw:1,0", 512, 44100);
		Nl::Examples::ExamplesHandle_t handle = Nl::Examples::midiSine("hw:1,0", 1024, 48000);

		// Wait for user to exit by pressing 'q'
		// Print buffer statistics on other keys
		while(getchar() != 'q') {
			if (handle.audioInput) std::cout << "Input:" << std::endl
											 << handle.audioInput->getStats() << std::endl;
			if (handle.audioOutput) std::cout << "Output:" << std::endl
											  << handle.audioOutput->getStats() << std::endl;
		}

		std::cout << "Reached End.." << std::endl;

		// Tell worker thread to cleanup and quit
		Nl::terminateWorkingThread(handle.workingThreadHandle);

		std::cout << "Cleaned up and done..." << std::endl;

	} catch (AudioAlsaException& e) {
		std::cout << "### Exception ###" << std::endl << "  " << e.what() << std::endl;
	} catch (std::exception& e) {
		std::cout << "### Exception ###" << std::endl << "  " << e.what() << std::endl;
	} catch(...) {
		std::cout << "### Exception ###" << std::endl << "  default" << std::endl;
	}

	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	std::cout << std::endl;

	return 0;
}

