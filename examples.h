#pragma once

#include "audiofactory.h"

namespace Nl {
namespace Examples {

struct ExamplesHandle_t {
	WorkingThreadHandle_t workingThreadHandle;
	AudioAlsaInput_t audioInput;
	AudioAlsaOutput_t audioOutput;
	Buffer_t inBuffer;
	Buffer_t outBuffer;
	//RawMidiDevice rawMidi;
	//Buffer_t midiBuffer;
};

ExamplesHandle_t inputToOutput(const std::string& deviceInName,
							   const std::string& deviceOutName,
							   unsigned int buffersize,
							   unsigned int samplerate);

ExamplesHandle_t midiSine(const std::string& audioOutDeviceName,
						  const std::string& midiInDeviceName,
						  unsigned int buffersize,
						  unsigned int samplerate);

}
}
