#include "inputtooutput.h"
#include "examples.h"

#include <audio/audioalsainput.h>
#include <audio/audioalsaoutput.h>
#include <audio/audioalsaexception.h>

#include <common/stopwatch.h>
#include <common/tools.h>

Nl::StopWatch sw("AudioCallback");

int main()
{
	try
	{
		auto availableDevices = Nl::getDetailedCardInfos();
		for(auto it=availableDevices.begin(); it!=availableDevices.end(); ++it)
			std::cout << *it << std::endl;

		auto availableDevs = Nl::AlsaCardIdentifier::getCardIdentifiers();
		for (auto it=availableDevs.begin(); it!=availableDevs.end(); ++it)
			std::cout << *it << std::endl;

		Nl::AlsaCardIdentifier audioIn(0,0,0, "USB Device");
		Nl::AlsaCardIdentifier audioOut(0,0,0, "USB Device");

		const int buffersize = 256;
		const int samplerate = 48000;

		auto handle = Nl::Examples::inputToOutput(audioIn, audioOut, buffersize, samplerate);

		while(getchar() != 'q')
		{
			std::cout << sw << std::endl;

			if (handle.audioOutput) std::cout << "Audio: Output Statistics:" << std::endl
											  << handle.audioOutput->getStats() << std::endl;
			if (handle.audioInput) std::cout << "Audio: Input Statistics:" << std::endl
											 << handle.audioInput->getStats() << std::endl;

			if (handle.inMidiBuffer) {
				unsigned long rxBytes, txBytes;
				handle.inMidiBuffer->getStat(&rxBytes, &txBytes);
				std::cout << "Midi: Input Statistics:" << std::endl
						  << "rxBytes=" << rxBytes << "  txBytes=" << txBytes << std::endl;
			}

			std::cout << "BufferCount: " << handle.audioInput->getBufferCount() << std::endl;
		}

		// Tell worker thread to cleanup and quit
		Nl::terminateWorkingThread(handle.workingThreadHandle);
		if (handle.audioOutput) handle.audioOutput->stop();
		if (handle.audioInput) handle.audioInput->stop();

	} catch (Nl::AudioAlsaException& e) {
		std::cout << "### Exception ###" << std::endl << "  " << e.what() << std::endl;
	} catch (std::exception& e) {
		std::cout << "### Exception ###" << std::endl << "  " << e.what() << std::endl;
	} catch(...) {
		std::cout << "### Exception ###" << std::endl << "  default" << std::endl;
	}

	return 0;
}

namespace Nl {
namespace Examples {

// In to out example
void inToOutCallback(u_int8_t *in,
					 u_int8_t *out,
					 const SampleSpecs &sampleSpecs __attribute__ ((unused)),
					 SharedUserPtr ptr __attribute__ ((unused)))
{
	static int counter = 0;
	StopBlockTime sft(&sw, "val" + std::to_string(counter++));

	memcpy(out, in, sampleSpecs.buffersizeInBytes);
}

ExamplesHandle inputToOutput(const AlsaCardIdentifier &inCard, const AlsaCardIdentifier &outCard, unsigned int buffersize, unsigned int samplerate)
{
	// In this example, we just copy data from input to output
	// Samplerate and buffersize can be set. A handle to stop the
	// working threads is returned
	// To terminate this example, call:
	// Nl::terminateWorkingThread(handle)
	ExamplesHandle ret;

	ret.inBuffer = createBuffer("InputBuffer");
	ret.audioInput = createAlsaInputDevice(inCard, ret.inBuffer, buffersize);
	ret.audioInput->setSamplerate(samplerate);

	ret.outBuffer = createBuffer("OutputBuffer");
	ret.audioOutput = createAlsaOutputDevice(outCard, ret.outBuffer, buffersize);
	ret.audioOutput->setSamplerate(samplerate);

	// DANGER!!!!
	// TODO: Check sync mechanism here. If registerInOutCallbackOnBuffer called before
	//		 input/output->start(), we seem to have a deadlock!
	// TODO: Consider implementing something like autostart for the threads in the
	//		 audio chain. Eg. Reading/Writing threads on BlockingCircularBuffer !!!
	ret.audioOutput->start();
	ret.audioInput->start();

	SharedUserPtr ptr(new UserPtr("unused", nullptr));
	ret.workingThreadHandle = registerInOutCallbackOnBuffer(ret.inBuffer, ret.outBuffer, inToOutCallback, ptr);

	return ret;
}

} // namespace Nl
} // namespace Examples
