#include "examples.h"
#include "audiofactory.h"
#include "blockingcircularbuffer.h"

namespace Nl {
namespace Examples {

// Just copy samples from in- to output
void inToOutCallback(u_int8_t *in, u_int8_t *out, size_t size, const SampleSpecs_t &sampleSpecs)
{
	memcpy(out, in, size);
}

ExamplesHandle_t inputToOutput(const std::string &deviceInName, const std::string &deviceOutName, unsigned int buffersize, unsigned int samplerate)
{
	// In this example, we just copy data from input to output
	// Samplerate and buffersize can be set. A handle to stop the
	// working threads is returned
	// To terminate this example, call:
	// Nl::terminateWorkingThread(hamdle)
	ExamplesHandle_t ret;

	ret.inBuffer = Nl::createBuffer("InputBuffer");
	//typedef decltype(getTypeForBitlenght(ret.inBuffer->sampleSpecs())) SampleType;

	ret.audioInput = Nl::createInputDevice(deviceInName, ret.inBuffer, buffersize);
	ret.audioInput->setSamplerate(samplerate);

	ret.outBuffer = Nl::createBuffer("OutputBuffer");
	ret.audioOutput = Nl::createOutputDevice(deviceOutName, ret.outBuffer, buffersize);
	ret.audioOutput->setSamplerate(samplerate);

	ret.workingThreadHandle.terminateRequest = Nl::createTerminateFlag();

	// DANGER!!!!
	// TODO: Check sync mechanism here. If registerInOutCallbackOnBuffer called before
	//		 input/output->start(), we seem to have a deadlock!
	// TODO: Consider implementing something like autostart for the threads in the
	//		 audio chain. Eg. Reading/Writing threads on BlockingCircularBuffer !!!
	ret.audioInput->start();
	ret.audioOutput->start();

	ret.workingThreadHandle.thread = Nl::registerInOutCallbackOnBuffer(ret.inBuffer, ret.outBuffer, inToOutCallback, ret.workingThreadHandle.terminateRequest);

	return ret;
}


void midiSineCallback(u_int8_t *out, size_t size, const SampleSpecs_t &sampleSpecs)
{

	double ramp = 0.0;

	int32_t samples[sampleSpecs.buffersizeInSamples];
	sinewave<int32_t>(samples, 4000);

	u_int8_t *orig = out;

	for (int sample=0; sample<sampleSpecs.buffersizeInSamples; sample++) {
		for (int byte=0; byte<sampleSpecs.bytesPerSample; byte++) {
			*out = static_cast<u_int8_t>((samples[sample] >> (byte*8)) & 0xFF);
			out++;
		}
	}

	//for(int i=0; i<sampleSpecs.buffersizeInSamples; i++) {
	//	printf("%08X %02X %02X %02X\n", samples[i], orig[i*3+2], orig[i*3+1], orig[i*3+0]);
	//}
}

ExamplesHandle_t midiSine(const std::string& audioOutDeviceName,
						  const std::string& midiInDeviceName,
						  unsigned int buffersize,
						  unsigned int samplerate)
{
	ExamplesHandle_t ret;

	// Not needed, since we only playback here
	ret.inBuffer = nullptr;
	ret.audioInput = nullptr;

	ret.outBuffer = Nl::createBuffer("OutputBuffer");
	ret.audioOutput = Nl::createOutputDevice(audioOutDeviceName, ret.outBuffer, buffersize);
	ret.audioOutput->setSamplerate(samplerate);

//	ret.midiBuffer = Nl::createBuffer("MidiBuffer");
//	ret.rawMidi = Nl::createRawMidiDevice(midiInDeviceName, ret.midiBuffer);


	ret.workingThreadHandle.terminateRequest = Nl::createTerminateFlag();

	// DANGER!!!!
	// TODO: Check sync mechanism here. If registerInOutCallbackOnBuffer called before
	//		 input/output->start(), we seem to have a deadlock!
	// TODO: Consider implementing something like autostart for the threads in the
	//		 audio chain. Eg. Reading/Writing threads on BlockingCircularBuffer !!!
	ret.audioOutput->start();

	ret.workingThreadHandle.thread = Nl::registerOutputCallbackOnBuffer(ret.outBuffer, midiSineCallback, ret.workingThreadHandle.terminateRequest);

	return ret;
}

} // namespace Nl
} // namespace Examples
