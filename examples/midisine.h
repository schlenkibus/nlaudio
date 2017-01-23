#pragma once

#include "examples.h"

namespace Nl {
namespace Examples {

void silenceCallback(u_int8_t *out,
					 const SampleSpecs &sampleSpecs __attribute__ ((unused)),
					 SharedUserPtr ptr __attribute__ ((unused)));

ExamplesHandle midiSine(const AlsaCardIdentifier &audioOutCard,
						const AlsaCardIdentifier &midiInCard,
						unsigned int buffersize,
						unsigned int samplerate);

}
}
