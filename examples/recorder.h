#pragma once

#include <audio/audiofactory.h>

namespace Nl {
namespace Examples {

void silenceCallback(u_int8_t *out, const SampleSpecs &sampleSpecs __attribute__ ((unused)));

JobHandle recorder(const AlsaCardIdentifier &audioInCard,
                        unsigned int buffersize,
                        unsigned int samplerate,
                        int fd);

} // namespace Examples
} // namespace Nl
