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

// Just copy samples from in- to output
void callback(void *in, void *out, size_t size)
{
	memcpy(out, in, size);
}


int main()
{

	// Changing priority only works as root
	int which = PRIO_PROCESS;
	id_t pid;
	int priority = -20;

	pid = getpid();
	int ret = setpriority(which, pid, priority);

	try {

		auto handle = Nl::Examples::inputToOutput(512, 44100);


		// Wait for user to exit by pressing 'q'
		// Print buffer statistics on other keys
		while(getchar() != 'q');

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

	std::cout << std::endl;

	return 0;
}

