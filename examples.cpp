#include "examples.h"
#include "audiofactory.h"

namespace Nl {
namespace Examples {

// Just copy samples from in- to output
void callback(void *in, void *out, size_t size)
{
		memcpy(out, in, size);
}

ExamplesHandle_t inputToOutput(unsigned int buffersize, unsigned int samplerate)
{
		// In this example, we just copy data from input to output
		// Samplerate and buffersize can be set. A handle to stop the
		// working threads is returned
		// To terminate this example, call:
		// Nl::terminateWorkingThread(hamdle)
		ExamplesHandle_t ret;

		auto inBuffer = Nl::getBuffer("InputBuffer");
		ret.audioInput = Nl::getInputDevice("hw:1,0", inBuffer, buffersize);
		ret.audioInput->setSamplerate(samplerate);

		auto outBuffer = Nl::getBuffer("OutputBuffer");
		ret.audioOutput = Nl::getOutputDevice("hw:1,0", outBuffer, buffersize);
		ret.audioOutput->setSamplerate(samplerate);

		ret.workingThreadHandle.terminateRequest = Nl::getTerminateFlag();
		ret.workingThreadHandle.thread = Nl::registerInOutCallbackOnBuffer(inBuffer, outBuffer, callback, ret.workingThreadHandle.terminateRequest);

		ret.audioInput->start();
		ret.audioOutput->start();

		return ret;
}




} // namespace Nl
} // namespace Examples
