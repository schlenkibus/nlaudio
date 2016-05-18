#include "examples.h"

#include "stopwatch.h"
#include "audioalsainput.h"
#include "audioalsaoutput.h"
#include "rawmididevice.h"
#include "vamphostaubiotempo.h"
#include "tools.h"

#include <aubio/aubio.h>

#include "vamp-hostsdk/PluginHostAdapter.h"
#include "vamp-hostsdk/PluginInputDomainAdapter.h"
#include "vamp-hostsdk/PluginLoader.h"

extern Nl::StopWatch sw;

namespace Nl {
namespace Examples {

using Vamp::Plugin;
using Vamp::PluginHostAdapter;
using Vamp::RealTime;
using Vamp::HostExt::PluginLoader;
using Vamp::HostExt::PluginWrapper;
using Vamp::HostExt::PluginInputDomainAdapter;// Vamp plugins


PluginLoader *loader = nullptr;
Plugin *plugin = nullptr;


ExamplesHandle vampPlugin(const AlsaCardIdentifier &inCard,
						  unsigned int channels,
						  unsigned int buffersize,
						  unsigned int samplerate)
{

	//Remider: This leaks
	VampHostAubioTempo *bpmDetect = new VampHostAubioTempo("vamp-aubio", "beatroot", samplerate, 0);
	bpmDetect->initialize(channels, 480, buffersize);

	ExamplesHandle ret;
	ret.inBuffer = createBuffer("InputBuffer");
	ret.audioInput = createAlsaInputDevice(inCard, ret.inBuffer, buffersize);
	ret.audioInput->setSamplerate(samplerate);
	ret.audioInput->setChannelCount(channels);

	SharedUserPtr ptr(new UserPtr("VampHostAubioTemp", static_cast<void*>(bpmDetect)));

	ret.audioInput->init();
	ret.workingThreadHandle = registerInputCallbackOnBuffer(ret.inBuffer, vampPluginCallback, ptr);
	ret.audioInput->start();

	return ret;
}

void vampPluginCallback(uint8_t *in, const SampleSpecs &sampleSpecs, SharedUserPtr ptr)
{
	if (ptr->info != "VampHostAubioTemp") {
		std::cerr << __func__ << ": UserPtr seems to be wrong type!" << std::endl;
	}

	VampHostAubioTempo* handle = static_cast<VampHostAubioTempo*>(ptr->ptr);
	handle->process(in, sampleSpecs);

}

//Onset Detection
aubio_onset_t *o;
fvec_t *onset;
fvec_t *input;
smpl_t is_onset;

void onsetDetectionCallback(u_int8_t *in, const SampleSpecs &sampleSpecs, SharedUserPtr ptr)
{
	static int counterA = 0;
	static int counter = 0;
	StopBlockTime sft(&sw, "val" + std::to_string(counterA++));

	//printf("size=%i\n", size);

	for (unsigned int frameIndex=0; frameIndex<sampleSpecs.buffersizeInFramesPerPeriode; frameIndex=frameIndex+sampleSpecs.channels) {

		//double sum = 0.f;
		//for (unsigned int channelIndex=0; channelIndex<sampleSpecs.channels; ++channelIndex) {
		//	sum += getSample(in, frameIndex, channelIndex, sampleSpecs);
		//}

		//sum /= sampleSpecs.channels;



		fvec_set_sample(input, getSample(in, frameIndex, 0, sampleSpecs), frameIndex);
	}

	aubio_onset_do(o, input, onset);
	is_onset = fvec_get_sample(onset, 0);

	if (is_onset > 0.8) {
		printf("%i\n", counter++);

	}
}

ExamplesHandle onsetDetection(const AlsaCardIdentifier &inCard, unsigned int buffersize, unsigned int samplerate)
{
	ExamplesHandle ret;

	ret.inBuffer = createBuffer("InputBuffer");
	ret.audioInput = createAlsaInputDevice(inCard, ret.inBuffer, buffersize);
	ret.audioInput->setSamplerate(samplerate);

	ret.audioInput->start();
	ret.workingThreadHandle = registerInputCallbackOnBuffer(ret.inBuffer, onsetDetectionCallback, nullptr);

	o = new_aubio_onset("default", buffersize, buffersize/2, samplerate);
	input = new_fvec(buffersize);
	onset = new_fvec(1);

	//aubio_onset_set_threshold(o, 1.1);
	//aubio_onset_set_silence(o, 0.001);

	// Set this to about 400
	// 60000/400 ~= 150 BPM or slower
	aubio_onset_set_minioi_ms(o,400);
	std::cout << "min interval between onsets=" << aubio_onset_get_minioi_ms(o) << std::endl;

	return ret;
}

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

void midiSineCallback(u_int8_t *out,
					  const SampleSpecs &sampleSpecs,
					  SharedUserPtr ptr __attribute__ ((unused)))
{
	static int counter = 0;
	StopBlockTime sft(&sw, "val" + std::to_string(counter++));

	static uint8_t velocity = 0;
	static double frequency = 0;
	static int32_t notesOn = 0;

	unsigned char midiByteBuffer[3];
	bool reset = false;

	// We can get a buffer by its name, to access its data:
	auto midiBuffer = getBufferForName("MidiBuffer");

	if(midiBuffer) {
		while(midiBuffer->availableToRead() >= 3) {
			midiBuffer->get(midiByteBuffer, 3);
			if(midiByteBuffer[0] == 0x90)
			{
				velocity = midiByteBuffer[2];
				if(velocity) {
					notesOn++;
					reset = true;   // nur vorläufig
					frequency = pow(2.f, static_cast<double>((midiByteBuffer[1]-69)/12.f)) * 440.f;
				} else {
					notesOn--;
				}
			} else if(midiByteBuffer[0] == 0x80) {
				notesOn--;
			}
		}
	}

	if(notesOn > 0) {
		int32_t samples[sampleSpecs.buffersizeInFramesPerPeriode];
		sinewave<int32_t>(samples, frequency, reset, sampleSpecs);

		for (unsigned int byte=0; byte<sampleSpecs.buffersizeInBytesPerPeriode; byte++) {
			unsigned int currentSample = (byte / (sampleSpecs.channels * sampleSpecs.bytesPerSample));
			unsigned int byteIndex = (byte % sampleSpecs.bytesPerSample);

			if (sampleSpecs.isLittleEndian) {
				*out++ = static_cast<uint8_t>(uint32_t(samples[currentSample] >> ((byteIndex)*8)) & 0xFF);
			} else {
				*out++ = static_cast<uint8_t>(uint32_t(samples[currentSample] >> ((sampleSpecs.bytesPerSample-byteIndex-1)*8)) & 0xFF);
			}
		}
	}
	else {
		memset(out, 0, sampleSpecs.buffersizeInBytesPerPeriode);
	}
}

// Midi Sine example
ExamplesHandle midiSine(const AlsaCardIdentifier &audioOutCard,
						const AlsaCardIdentifier &midiInCard,
						unsigned int buffersize,
						unsigned int samplerate)
{
	ExamplesHandle ret;

	// Not needed, since we only playback here
	ret.inBuffer = nullptr;
	ret.audioInput = nullptr;

	// Lets create a buffer, which we have to pass to the output soundcard
	ret.outBuffer = createBuffer("AudioOutput");
	// Open soundcard, using above buffer
	ret.audioOutput = createAlsaOutputDevice(audioOutCard, ret.outBuffer, buffersize);

	// Configure Audio (if needed, or use default)
	//ret.audioOutput->setSampleFormat(...);
	//ret.audioOutput->setSamplerate(samplerate);
	ret.audioOutput->setChannelCount(2);
	//ret.audioOutput->setSampleFormat("S16_LE");

	// We want midi as well
	ret.inMidiBuffer = createBuffer("MidiBuffer");
	auto midiInput = createRawMidiDevice(midiInCard, ret.inMidiBuffer);

	// Start Audio and Midi Thread
	ret.audioOutput->start();
	midiInput->start();

	std::cout << "MidiBufferSize: " << midiInput->getAlsaMidiBufferSize() << std::endl;

	// Register a Callback
	ret.workingThreadHandle = registerOutputCallbackOnBuffer(ret.outBuffer, midiSineCallback, nullptr);

	return ret;
}

// Silence Example
void silenceCallback(u_int8_t *out, const SampleSpecs &sampleSpecs __attribute__ ((unused)))
{
	memset(out, 0, sampleSpecs.buffersizeInBytesPerPeriode);
}

ExamplesHandle silence(const AlsaCardIdentifier &audioOutCard,
					   unsigned int buffersize,
					   unsigned int samplerate)
{
	ExamplesHandle ret;

	// Not nedded, since we only playback here w/o midi
	ret.inBuffer = nullptr;
	ret.audioInput = nullptr;

	// Create an output buffer and an output device
	ret.outBuffer = createBuffer("AudioOutput");
	ret.audioOutput = createAlsaOutputDevice(audioOutCard, ret.outBuffer, buffersize);

	// Configure audio device
	ret.audioOutput->setSamplerate(samplerate);

	// Start audio Thread
	ret.audioOutput->start();

	// Register a Callback
//	ret.workingThreadHandle = registerOutputCallbackOnBuffer(ret.outBuffer, silenceCallback, nullptr);

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
