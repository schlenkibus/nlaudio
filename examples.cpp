#include "examples.h"
#include "audiofactory.h"
#include "blockingcircularbuffer.h"

namespace Nl {
namespace Examples {

// In to out example
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

	// DANGER!!!!
	// TODO: Check sync mechanism here. If registerInOutCallbackOnBuffer called before
	//		 input/output->start(), we seem to have a deadlock!
	// TODO: Consider implementing something like autostart for the threads in the
	//		 audio chain. Eg. Reading/Writing threads on BlockingCircularBuffer !!!
	ret.audioInput->start();
	ret.audioOutput->start();

	ret.workingThreadHandle = Nl::registerInOutCallbackOnBuffer(ret.inBuffer, ret.outBuffer, inToOutCallback);

	return ret;
}

// Midi Sine example
void midiSineCallback(u_int8_t *out, size_t size, const SampleSpecs_t &sampleSpecs)
{
	unsigned char midiByteBuffer[3];
	static uint8_t velocity = 0;
	static double frequency = 0;
	bool reset = false;

	// We can get a buffer by its name, to access its data:
	auto midiBuffer = Nl::getBufferForName("MidiBuffer");
	if (midiBuffer) {
		while (midiBuffer->availableToRead() >= 3) {
			midiBuffer->get(midiByteBuffer, 3);
			if (midiByteBuffer[0] == 0x90) {
				velocity = midiByteBuffer[2];
				double newFrequency = pow(2.f, static_cast<double>((midiByteBuffer[1]-69)/12.f)) * 440.f;
				if (newFrequency != frequency) {
					frequency = newFrequency;
					reset = true;
					printf("Reseting...\n");
				}
			} else {
				velocity = 0;
			}
		}
	}

	if (velocity) {
		int32_t samples[sampleSpecs.buffersizeInFramesPerPeriode];
		Nl::sinewave<int32_t>(samples, frequency, reset, sampleSpecs);
		//Nl::store<int32_t>(samples, sampleSpecs.buffersizeInFramesPerPeriode, "lalala.txt");
		for (unsigned int byte=0; byte<sampleSpecs.buffersizeInBytesPerPeriode; byte++) {
			unsigned int currentSample = (byte / (sampleSpecs.channels * sampleSpecs.bytesPerSample));
			unsigned int byteIndex = (byte % sampleSpecs.bytesPerSample);

			if (sampleSpecs.isLittleEndian) {
				*out++ = static_cast<uint8_t>(uint32_t(samples[currentSample] >> ((byteIndex)*8)) & 0xFF);
			} else {
				*out++ = static_cast<uint8_t>(uint32_t(samples[currentSample] >> ((sampleSpecs.bytesPerSample-byteIndex-1)*8)) & 0xFF);
			}
		}
	} else {
		memset(out, 0, size);
	}
}

// Midi Sine example
ExamplesHandle_t midiSine(const std::string& audioOutDeviceName,
						  const std::string& midiInDeviceName,
						  unsigned int buffersize,
						  unsigned int samplerate)
{
	ExamplesHandle_t ret;

	// Not needed, since we only playback here
	ret.inBuffer = nullptr;
	ret.audioInput = nullptr;

	// Lets create a buffer, which we have to pass to the output soundcard
	ret.outBuffer = Nl::createBuffer("AudioOutput");
	// Open soundcard, using above buffer
	ret.audioOutput = Nl::createOutputDevice(audioOutDeviceName, ret.outBuffer, buffersize);

	// Configure Audio (if needed, or use default)
    //ret.audioOutput->setSampleFormat(...);
	ret.audioOutput->setSamplerate(samplerate);

	// We want midi as well
	auto midiBuffer = Nl::createBuffer("MidiBuffer");
	auto midiInput = Nl::createRawMidiDevice(midiInDeviceName, midiBuffer);

	// Start Audio and Midi Thread
	ret.audioOutput->start();
	midiInput->start();

	// Register a Callback
	ret.workingThreadHandle = Nl::registerOutputCallbackOnBuffer(ret.outBuffer, midiSineCallback);

	return ret;
}

// Silence Example
void silenceCallback(u_int8_t *out, size_t size, const SampleSpecs_t &sampleSpecs)
{
    memset(out, 0, size);
}

ExamplesHandle_t silence(const std::string& audioOutDeviceName,
                        unsigned int buffersize,
                        unsigned int samplerate)
{
    ExamplesHandle_t ret;

    // Not nedded, since we only playback here w/o midi
    ret.inBuffer = nullptr;
    ret.audioInput = nullptr;

    // Create an output buffer and an output device
    ret.outBuffer = Nl::createBuffer("AudioOutput");
    ret.audioOutput = Nl::createOutputDevice(audioOutDeviceName, ret.outBuffer, buffersize);

    // Configure audio device
    ret.audioOutput->setSamplerate(samplerate);

    // Start audio Thread
    ret.audioOutput->start();

    // Register a Callback
    ret.workingThreadHandle = Nl::registerOutputCallbackOnBuffer(ret.outBuffer, silenceCallback);

    return ret;
}


} // namespace Nl
} // namespace Examples
