#pragma once

#include "audiofactory.h"

namespace Nl {
namespace Examples {

struct ExamplesHandle_t {
	WorkingThreadHandle_t workingThreadHandle;
	AudioAlsaInput_t audioInput;
	AudioAlsaOutput_t audioOutput;
};


ExamplesHandle_t inputToOutput(unsigned int buffersize, unsigned int samplerate);

}
}
