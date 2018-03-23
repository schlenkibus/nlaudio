#pragma once

#include <audio/audiofactory.h>

using namespace Nl;

void inToOutCallback(u_int8_t *in,
					 u_int8_t *out,
					 const SampleSpecs &sampleSpecs __attribute__ ((unused)),
					 SharedUserPtr ptr __attribute__ ((unused)));

JobHandle inputToOutput(const AlsaAudioCardIdentifier &audioInCard,
                             const AlsaAudioCardIdentifier &audioOutCard,
                             unsigned int buffersize,
                             unsigned int samplerate);
