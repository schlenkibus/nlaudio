#include <iostream>
#include <ostream>

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

		//Nl::Examples::ExamplesHandle_t handle = Nl::Examples::inputToOutput("hw:1,0", "hw:1,0", 512, 4	4100);
		Nl::Examples::ExamplesHandle_t handle = Nl::Examples::midiSine("hw:1,0", "hw:1,0", 1024, 48000);


#if 0
		int32_t samples0[1024];
		Nl::sinewave<int32_t>(samples0, 1024);
		Nl::store<int32_t>(samples0, 1024, "int32_t");

		uint32_t samples1[1024];
		Nl::sinewave<uint32_t>(samples1, 1024);
		Nl::store<uint32_t>(samples1, 1024, "uint32_t");

		uint8_t samples2[1024];
		Nl::sinewave<uint8_t>(samples2, 1024);
		Nl::store<uint8_t>(samples2, 1024, "uint8_t");

		int8_t samples3[1024];
		Nl::sinewave<int8_t>(samples3, 1024);
		Nl::store<int8_t>(samples3, 1024, "int8_t");

		double samples4[1024];
		Nl::sinewave<double>(samples4, 1024);
		Nl::store<double>(samples4, 1024, "double");

		float samples5[1024];
		Nl::sinewave<float>(samples5, 1024);
		Nl::store<float>(samples5, 1024, "float");

		return 0;
#endif

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

