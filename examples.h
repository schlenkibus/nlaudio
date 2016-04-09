#pragma once

#include "audiofactory.h"

namespace Nl {
namespace Examples {

struct ExamplesHandle {
	WorkingThreadHandle workingThreadHandle;
	SharedAudioHandle audioInput;
	SharedAudioHandle audioOutput;
	SharedRawMidiDeviceHandle midiInput;
	SharedRawMidiDeviceHandle midiOutput;
	SharedBufferHandle inBuffer;
	SharedBufferHandle outBuffer;
	SharedBufferHandle inMidiBuffer;
};

void vampPluginCallback(uint8_t *in, size_t size, const SampleSpecs &sampleSpecs);
ExamplesHandle vampPlugin(const AlsaCardIdentifier &inCard, unsigned int channels,
						 unsigned int buffersize,
						 unsigned int samplerate,
						 const std::string &libraryName,
						 const std::string &pluginName);


void onsetDetectionCallback(u_int8_t *in, size_t size, const SampleSpecs &sampleSpecs);
ExamplesHandle onsetDetection(const AlsaCardIdentifier &inCard, unsigned int buffersize, unsigned int samplerate);

ExamplesHandle inputToOutput(const AlsaCardIdentifier &audioInCard,
							   const AlsaCardIdentifier &audioOutCard,
							   unsigned int buffersize,
							   unsigned int samplerate);

ExamplesHandle jackInputToOutput(const AlsaCardIdentifier &inCard, const AlsaCardIdentifier &outCard, unsigned int buffersize, unsigned int samplerate);


ExamplesHandle inputToOutputWithMidi(   const AlsaCardIdentifier &audioInCard,
                                        const AlsaCardIdentifier &audioOutCard,
                                        const AlsaCardIdentifier &midiInCard,
                                        unsigned int buffersize,
                                        unsigned int samplerate);

ExamplesHandle midiSine(const AlsaCardIdentifier &audioOutCard,
						  const AlsaCardIdentifier &midiInCard,
						  unsigned int buffersize,
						  unsigned int samplerate);

ExamplesHandle silence(const AlsaCardIdentifier &audioOutCard,
                        unsigned int buffersize,
                        unsigned int samplerate);

}
}
