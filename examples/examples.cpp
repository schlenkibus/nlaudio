#include "examples.h"

#include "common/stopwatch.h"
#include "audio/audioalsainput.h"
#include "audio/audioalsaoutput.h"
#include "midi/rawmididevice.h"
#include "common/tools.h"

extern Nl::StopWatch sw;

namespace Nl {
namespace Examples {

ExamplesHandle jackInputToOutput(const AlsaCardIdentifier &inCard,
								 const AlsaCardIdentifier &outCard,
								 unsigned int buffersize,
								 unsigned int samplerate)
{
	ExamplesHandle ret;

	ret.inBuffer = createBuffer("InputBuffer");
	ret.audioInput = createJackInputDevice(inCard, ret.inBuffer, buffersize);
	//ret.audioInput->setSamplerate(samplerate);

	ret.outBuffer = createBuffer("OutputBuffer");
	//ret.audioOutput = createJackOutputDevice(outCard, ret.outBuffer, buffersize);
	//ret.audioOutput->setSamplerate(samplerate);

	// DANGER!!!!
	// TODO: Check sync mechanism here. If registerInOutCallbackOnBuffer called before
	//		 input/output->start(), we seem to have a deadlock!
	// TODO: Consider implementing something like autostart for the threads in the
	//		 audio chain. Eg. Reading/Writing threads on BlockingCircularBuffer !!!
	//ret.audioOutput->start();
	ret.audioInput->start();

	ret.workingThreadHandle = registerAutoDrainOnBuffer(ret.inBuffer);
	//ret.workingThreadHandle = registerInOutCallbackOnBuffer(ret.inBuffer, ret.outBuffer, inToOutCallback);

	return ret;
}

void inToOutCallbackWithMidi(u_int8_t *in, u_int8_t *out,
							 const SampleSpecs &sampleSpecs __attribute__ ((unused)),
							 SharedUserPtr ptr __attribute__ ((unused)))
{
	static int counter = 0;
	//StopBlockTime sft(&sw, "val" + std::to_string(counter++));
	sw.stop();
	sw.start("val" + std::to_string(counter++));

	// Midi Stuff
	static float curVolumeFactor = 1.f;

#if 0
	auto midiBuffer = getBufferForName("MidiBuffer");

	if (midiBuffer) {
		unsigned char midiByteBuffer[3];
		while (midiBuffer->availableToRead() >= 3) {
			midiBuffer->get(midiByteBuffer, 3);
			//printf("%02X %02X %02X\n", midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);
			if (midiByteBuffer[1] == 2) {
				curVolumeFactor = static_cast<float>(midiByteBuffer[2]) / static_cast<float>(std::numeric_limits<unsigned char>::max() / 2);
			}
		}
	}
#endif
	for (unsigned int frameIndex=0; frameIndex<sampleSpecs.buffersizeInFramesPerPeriode; ++frameIndex) {
		for (unsigned int channelIndex=0; channelIndex<sampleSpecs.channels; ++channelIndex) {
			float currentSample = getSample(in, frameIndex, channelIndex, sampleSpecs);
			currentSample *= curVolumeFactor;
			setSample(out, currentSample, frameIndex, channelIndex, sampleSpecs);
		}
	}
}

ExamplesHandle inputToOutputWithMidi(   const AlsaCardIdentifier &audioInCard,
										const AlsaCardIdentifier &audioOutCard,
										const AlsaCardIdentifier &midiInCard,
										unsigned int buffersize,
										unsigned int samplerate)
{
	ExamplesHandle ret;

	ret.inBuffer = createBuffer("InputBuffer");
	ret.outBuffer = createBuffer("OutputBuffer");

	ret.audioInput = createAlsaInputDevice(audioInCard, ret.inBuffer, buffersize);
	ret.audioInput->setSamplerate(samplerate);

	ret.audioOutput = createAlsaOutputDevice(audioOutCard, ret.outBuffer, buffersize);
	ret.audioOutput->setSamplerate(samplerate);

	ret.inMidiBuffer = createBuffer("MidiBuffer");
	ret.midiInput = createRawMidiDevice(midiInCard, ret.inMidiBuffer);

	ret.audioOutput->start();
	ret.audioInput->start();
	ret.midiInput->start();

	SharedUserPtr ptr(new UserPtr("unused", nullptr));

	ret.workingThreadHandle = registerInOutCallbackOnBuffer(ret.inBuffer, ret.outBuffer, inToOutCallbackWithMidi, ptr);

	return ret;
}

} // namespace Nl
} // namespace Examples
