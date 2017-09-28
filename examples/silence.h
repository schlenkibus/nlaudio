#pragma once

#include <audio/audiofactory.h>

namespace Nl {
namespace Examples {

void recorderCallback(u_int8_t *out, const SampleSpecs &sampleSpecs __attribute__ ((unused)));

JobHandle recorder(const AlsaCardIdentifier &audioOutCard,
                       unsigned int buffersize,
                       unsigned int samplerate, int fd);

}
}
