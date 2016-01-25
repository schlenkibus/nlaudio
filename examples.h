#pragma once

#include "audiofactory.h"

namespace Nl {
namespace Examples {

struct ExamplesHandle {
	WorkingThreadHandle workingThreadHandle;
	SharedAudioAlsaInputHandle audioInput;
	SharedAudioAlsaOutputHandle audioOutput;
	SharedRawMidiDeviceHandle midiInput;
	SharedRawMidiDeviceHandle midiOutput;
	SharedBufferHandle inBuffer;
	SharedBufferHandle outBuffer;
	SharedBufferHandle inMidiBuffer;
};

ExamplesHandle inputToOutput(const AlsaCardIdentifier &audioInCard,
							   const AlsaCardIdentifier &audioOutCard,
							   unsigned int buffersize,
							   unsigned int samplerate);

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

//My Own shit
ExamplesHandle inputToOutputWithMidi(const AlsaCardIdentifier &audioInCard,
                               const AlsaCardIdentifier &audioOutCard, const AlsaCardIdentifier &midiIn,
                               unsigned int buffersize,
                               unsigned int samplerate);


ExamplesHandle midiSineWithMidi(const AlsaCardIdentifier &audioOutCard,
                          const AlsaCardIdentifier &midiInCard,
                          unsigned int buffersize,
                          unsigned int samplerate);


}
}
