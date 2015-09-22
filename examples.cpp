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

	memset(out, 0, size);
	return;

	int32_t samples[sampleSpecs.buffersizeInSamples];
	sinewave<int32_t>(samples, sampleSpecs, 440);

	u_int8_t *orig = out;

	for (int sample=0; sample<sampleSpecs.buffersizeInSamples; sample++) {
		for (int byte=0; byte<sampleSpecs.bytesPerSample; byte++) {
			*out = static_cast<u_int8_t>((samples[sample] >> (byte*8)) & 0xFF);
			out++;
		}
	}

	for(int i=0; i<sampleSpecs.buffersizeInSamples; i++) {
		printf("%08X %02X %02X %02X\n", samples[i], orig[i*3+2], orig[i*3+1], orig[i*3+0]);
	}




#if 0
	for (int byte=0; byte<size; byte++) {
		int currentByteSample = byte / (sampleSpecs.bytesPerSample * sampleSpecs.channels);
		int currentChannel = (byte % (sampleSpecs.bytesPerSample * sampleSpecs.channels)) / 3;
		int currentSample = byte / sampleSpecs.bytesPerFrame;

		samples[currentChannel][currentSample] |=
		//std::cout << (currentChannel ? "L" : "R") << currentSample << "\t" << std::endl;

	}

	unsigned int currentSample;

	if (sampleSpecs.isLittleEndian) {
		for (int i=0; i<sampleSpecs.bytesPerSample; i++)
			*(static_cast<unsigned char*>(&currentSample) + i) = (res >> i * 8) & 0xff;
	} else {
		for (int i=0; i<sampleSpecs.bytesPerSample; i++)
			*(samples[chn] + phys_bps - 1 - i) = (res >> i * 8) & 0xff;

	}



	for (int i=0; i<size; i++) {


	}


	for (int byte = 0; byte < sampleSpecs.bytesPerSample; byte++) {

	}



	for (unsigned int i=0; i<size; i++)
		(static_cast<char*>(out))[i] = 0;

#endif
}

ExamplesHandle_t midiSine(const std::string &deviceOutName, unsigned int buffersize, unsigned int samplerate)
{
	ExamplesHandle_t ret;

	ret.inBuffer = nullptr;
	ret.audioInput = nullptr;

	ret.outBuffer = Nl::createBuffer("OutputBuffer");
	ret.audioOutput = Nl::createOutputDevice(deviceOutName, ret.outBuffer, buffersize);
	ret.audioOutput->setSamplerate(samplerate);

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
