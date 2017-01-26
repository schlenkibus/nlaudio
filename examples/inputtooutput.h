#pragma once

#include "examples.h"

using namespace Nl;

void inToOutCallback(u_int8_t *in,
					 u_int8_t *out,
					 const SampleSpecs &sampleSpecs __attribute__ ((unused)),
					 SharedUserPtr ptr __attribute__ ((unused)));

ExamplesHandle inputToOutput(const AlsaCardIdentifier &audioInCard,
							 const AlsaCardIdentifier &audioOutCard,
							 unsigned int buffersize,
							 unsigned int samplerate);
