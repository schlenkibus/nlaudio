#pragma once

#include <audio/audiofactory.h>

namespace Nl {
namespace Examples {

void silenceCallback(u_int8_t *out,
                     const SampleSpecs &sampleSpecs __attribute__ ((unused)),
                     SharedUserPtr ptr __attribute__ ((unused)));

JobHandle silence(const AlsaAudioCardIdentifier &audioOutCard,
                  unsigned int buffersize,
                  unsigned int samplerate);

}
}
