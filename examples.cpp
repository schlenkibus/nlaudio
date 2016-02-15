#include "examples.h"

#include "stopwatch.h"
#include "audioalsainput.h"
#include "audioalsaoutput.h"
#include "rawmididevice.h"
#include "tools.h"

extern Nl::StopWatch sw;

namespace Nl {
namespace Examples {

// In to out example
void inToOutCallback(u_int8_t *in, u_int8_t *out, size_t size, const SampleSpecs &sampleSpecs __attribute__ ((unused)))
{
	static int counter = 0;
	StopBlockTime sft(&sw, "val" + std::to_string(counter++));

	memcpy(out, in, size);
}

ExamplesHandle inputToOutput(const AlsaCardIdentifier &inCard, const AlsaCardIdentifier &outCard, unsigned int buffersize, unsigned int samplerate)
{
	// In this example, we just copy data from input to output
	// Samplerate and buffersize can be set. A handle to stop the
	// working threads is returned
	// To terminate this example, call:
	// Nl::terminateWorkingThread(hamdle)
	ExamplesHandle ret;

	ret.inBuffer = createBuffer("InputBuffer");

	ret.audioInput = createInputDevice(inCard, ret.inBuffer, buffersize);
	ret.audioInput->setSamplerate(samplerate);

	ret.outBuffer = createBuffer("OutputBuffer");
	ret.audioOutput = createOutputDevice(outCard, ret.outBuffer, buffersize);
	ret.audioOutput->setSamplerate(samplerate);

	// DANGER!!!!
	// TODO: Check sync mechanism here. If registerInOutCallbackOnBuffer called before
	//		 input/output->start(), we seem to have a deadlock!
	// TODO: Consider implementing something like autostart for the threads in the
	//		 audio chain. Eg. Reading/Writing threads on BlockingCircularBuffer !!!
	ret.audioInput->start();
	ret.audioOutput->start();

	ret.workingThreadHandle = registerInOutCallbackOnBuffer(ret.inBuffer, ret.outBuffer, inToOutCallback);

	return ret;
}



void midiSineCallback(u_int8_t *out, size_t size, const SampleSpecs &sampleSpecs)
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
		memset(out, 0, size);
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
	ret.audioOutput = createOutputDevice(audioOutCard, ret.outBuffer, buffersize);

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
	ret.workingThreadHandle = registerOutputCallbackOnBuffer(ret.outBuffer, midiSineCallback);

	return ret;
}

// Silence Example
void silenceCallback(u_int8_t *out, size_t size, const SampleSpecs &sampleSpecs __attribute__ ((unused)))
{
	memset(out, 0, size);
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
	ret.audioOutput = createOutputDevice(audioOutCard, ret.outBuffer, buffersize);

	// Configure audio device
	ret.audioOutput->setSamplerate(samplerate);

	// Start audio Thread
	ret.audioOutput->start();

	// Register a Callback
	ret.workingThreadHandle = registerOutputCallbackOnBuffer(ret.outBuffer, silenceCallback);

	return ret;
}




////////////////// Daniels Test

void inToOutCallbackWithMidi(u_int8_t *in, u_int8_t *out, size_t size, const SampleSpecs &sampleSpecs __attribute__ ((unused)))
{
	static int counter = 0;
	//StopBlockTime sft(&sw, "val" + std::to_string(counter++));
	sw.stop();
	sw.start("val" + std::to_string(counter++));

	// Midi Stuff
#if 0
    // Fader to Volume with Input and Output - is working!
    static float curVolumeFactor = 0.f;
	auto midiBuffer = getBufferForName("MidiBuffer");

	if (midiBuffer) {
		unsigned char midiByteBuffer[3];
		while (midiBuffer->availableToRead() >= 3) {
			midiBuffer->get(midiByteBuffer, 3);
            printf("%02X %02X %02X\n", midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);
			if (midiByteBuffer[1] == 2) {
				curVolumeFactor = static_cast<float>(midiByteBuffer[2]) / static_cast<float>(std::numeric_limits<unsigned char>::max() / 2);
			}
		}
	}

    for (unsigned int frameIndex=0; frameIndex<sampleSpecs.buffersizeInFramesPerPeriode; ++frameIndex) {
        for (unsigned int channelIndex=0; channelIndex<sampleSpecs.channels; ++channelIndex) {
            float currentSample = getSample(in, frameIndex, channelIndex, sampleSpecs);
            currentSample *= curVolumeFactor;
            setSample(out, currentSample, frameIndex, channelIndex, sampleSpecs);
        }
    }
#endif


#if 0
    //Sawtooth Wave Generation for output - is working!
    unsigned int fs = sampleSpecs.samplerate;
    //Amplitude
    static float currentSample = -1;

    //für einen 500hz Sägezahn
    unsigned int f = 500;
    float resolution = 1./static_cast<float>(fs/f);

        for (unsigned int frameIndex=0; frameIndex<sampleSpecs.buffersizeInFramesPerPeriode; ++frameIndex) {

            //calculate samplewise
            currentSample += resolution;
            if(currentSample > 1)
                currentSample = -1;

            for (unsigned int channelIndex=0; channelIndex<sampleSpecs.channels; ++channelIndex) {
                setSample(out, currentSample, frameIndex, channelIndex, sampleSpecs);
            }

         }
#endif

#if 1
    // Sine Player for output
    static uint8_t velocity = 0;
    static float frequency = 0.;
    static int32_t notesOn = 0;
    bool reset = false;

    auto midiBuffer = getBufferForName("MidiBuffer");
    if(midiBuffer){
        unsigned char midiByteBuffer[3];
        while(midiBuffer ->availableToRead()>=3){
            midiBuffer->get(midiByteBuffer, 3);
            printf("%02X %02X %02X\n", midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);
            if(midiByteBuffer[0] == 0x90){
                velocity = midiByteBuffer[2];
                if(velocity){
                    notesOn++;
                    reset = true;
                    frequency = pow(2.f, static_cast<float>((midiByteBuffer[1]-69)/12.f))*440.f;
                }
                else{
                    notesOn--;
                }
            }else if (midiByteBuffer[0] == 0x80){
                notesOn--;
            }
        }
    }
    if(notesOn > 0){
        float sineSamples[sampleSpecs.buffersizeInFramesPerPeriode];
        sinewave<float>(sineSamples, frequency, reset, sampleSpecs);
        // store<float>(sineSamples, sampleSpecs.buffersizeInFramesPerPeriode, "sineTestData.txt");
        for (unsigned int frameIndex=0; frameIndex<sampleSpecs.buffersizeInFramesPerPeriode; ++frameIndex) {
            for (unsigned int channelIndex=0; channelIndex<sampleSpecs.channels; ++channelIndex) {
                setSample(out, sineSamples[frameIndex], frameIndex, channelIndex, sampleSpecs);
            }
        }


    }else{
        memset(out,0, size);
    }

#endif
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

	ret.audioInput = createInputDevice(audioInCard, ret.inBuffer, buffersize);
	ret.audioInput->setSamplerate(samplerate);

	ret.audioOutput = createOutputDevice(audioOutCard, ret.outBuffer, buffersize);
	ret.audioOutput->setSamplerate(samplerate);

	ret.inMidiBuffer = createBuffer("MidiBuffer");
	ret.midiInput = createRawMidiDevice(midiInCard, ret.inMidiBuffer);

	ret.audioOutput->start();
	ret.audioInput->start();
	ret.midiInput->start();

	ret.workingThreadHandle = registerInOutCallbackOnBuffer(ret.inBuffer, ret.outBuffer, inToOutCallbackWithMidi);

	return ret;
}

} // namespace Nl
} // namespace Examples
