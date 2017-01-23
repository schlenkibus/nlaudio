#pragma once

#include "examples.h"

namespace Nl {
namespace Examples {

void silenceCallback(u_int8_t *out, const SampleSpecs &sampleSpecs __attribute__ ((unused)));

ExamplesHandle silence(const AlsaCardIdentifier &audioOutCard,
					   unsigned int buffersize,
					   unsigned int samplerate);

}
}
