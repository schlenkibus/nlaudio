#pragma once

#include <iostream>
#include <cmath>
#include <limits>

#define NOT_IMPLEMENTED { std::cout << "ALERT: " << __func__ << " not implemented, yet. Aborting!"; exit(-1); }


namespace Nl {

struct SampleSpecs_t {
	unsigned int samplerate;		/// Samplerate
	unsigned int channels;			/// Channels
	unsigned int bytesPerFrame;		/// = channels * bytesPerSample
	unsigned int buffersizeInFrames;		/// Size of buffer. This is relevant for latency
	unsigned int buffersizeInFramesPerPeriode;
	unsigned int buffersizeInBytes; ///
	unsigned int buffersizeInBytesPerPeriode;
	unsigned int buffersizeInSamples;
	unsigned int buffersizeInSamplesPerPeriode;
	unsigned int bytesPerSample;	/// with 24_BE3 this would be 3, with S16 this would be 2
	unsigned int bytesPerSamplePhysical; /// Sometimes 24_BE3 or so can be stored in 4Bytes
	bool isFloat;
	bool isLittleEndian;
	bool isSigned;
};
std::ostream& operator<<(std::ostream& lhs, const SampleSpecs_t& rhs);

/// Signal generators

template<typename T>
void sinewave(T *buffer, const SampleSpecs_t &specs, double frequency)
{
	double ramp_increment = frequency / static_cast<double>(specs.samplerate);
	static double ramp = 0.f;

	for (unsigned int i=0; i<specs.buffersizeInFramesPerPeriode; i=i+specs.channels) {
		ramp += ramp_increment;
		if (ramp >= 1.f)
			ramp -= 1.f;

		for (unsigned int channel=0; channel<specs.channels; channel++) {
			buffer[i+channel] = sin(2.f * M_PI * ramp) * 100000; //std::numeric_limits<T>::max();
		}
	}
}

} // namespace Nl
