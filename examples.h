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
};

ExamplesHandle inputToOutput(const std::string& deviceInName,
							   const std::string& deviceOutName,
							   unsigned int buffersize,
							   unsigned int samplerate);

ExamplesHandle midiSine(const std::string& audioOutDeviceName,
						  const std::string& midiInDeviceName,
						  unsigned int buffersize,
						  unsigned int samplerate);

ExamplesHandle silence(const std::string& audioOutDeviceName,
                        unsigned int buffersize,
                        unsigned int samplerate);

}
}
