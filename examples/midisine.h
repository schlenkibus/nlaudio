#pragma once

#include <audio/audiofactory.h>

namespace Nl {
namespace Examples {

void silenceCallback(u_int8_t *out,
                     const SampleSpecs &sampleSpecs __attribute__ ((unused)),
                     SharedUserPtr ptr __attribute__ ((unused)));

JobHandle midiSine(const AlsaAudioCardIdentifier &audioOutCard,
                   const AlsaMidiCardIdentifier &midiInCard,
                   unsigned int buffersize,
                   unsigned int samplerate);

}
}
