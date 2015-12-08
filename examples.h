#pragma once

#include "audiofactory.h"

namespace Nl {
namespace Examples {

struct ExamplesHandle {
	WorkingThreadHandle workingThreadHandle;
	SharedAudioAlsaInputHandle audioInput;
	SharedAudioAlsaOutputHandle audioOutput;
	SharedBufferHandle inBuffer;
	SharedBufferHandle outBuffer;
	SharedBufferHandle inMidiBuffer;
};

ExamplesHandle inputToOutput(const AlsaCardIdentifier &audioInCard,
							   const AlsaCardIdentifier &audioOutCard,
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
