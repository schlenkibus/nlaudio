
#include <iostream>

#include "samplespecs.h"

namespace Nl {

unsigned int getByteIndex(unsigned int frameIndex, unsigned int channel, unsigned int byte, const SampleSpecs &sampleSpecs)
{
	return	// Index of current Frame
			(frameIndex*sampleSpecs.channels*sampleSpecs.bytesPerSample) +
			// Index of current channel in Frame (= Sample)
			(channel*sampleSpecs.bytesPerSample) +
			// Index of current byte in Sample
			byte;
}

float getSample(u_int8_t* in, u_int32_t frameIndex, u_int32_t channelIndex, const SampleSpecs& sampleSpecs)
{
	// Protect against segfault
	if (frameIndex > sampleSpecs.buffersizeInFramesPerPeriode)
		return 0.f;
	if (channelIndex > sampleSpecs.channels)
		return 0.f;

	if (sampleSpecs.isSigned) {

		signed int currentSample = 0;
		signed int currentMask = 0;

		if (sampleSpecs.isLittleEndian) {
			for (unsigned int byte=0; byte<sampleSpecs.bytesPerSample; byte++) {
				currentSample |= in[getByteIndex(frameIndex, channelIndex, byte, sampleSpecs)] << (byte*8);
				currentMask |= (0xFF << (byte*8));
			}
		} else {
			for (unsigned int byte=0; byte<sampleSpecs.bytesPerSample; byte++) {
				currentSample |= in[getByteIndex(frameIndex, channelIndex, byte, sampleSpecs)] << ((sampleSpecs.bytesPerSample-byte-1)*8);
				currentMask |= (0xFF << (byte*8));
			}
		}
		// If MSB is 1 we deal with negative numbers.
		if (currentSample & (1 << (sampleSpecs.bytesPerSample*8-1)))
			currentSample |= ~currentMask;

		return static_cast<float>(currentSample) / static_cast<float>(currentMask);

	} else { // UNSIGNED
		//TODO: implement me
		return 0.f;
	}
}

void setSample(u_int8_t* out, float sample, u_int32_t frameIndex, u_int32_t channelIndex, const SampleSpecs& sampleSpecs)
{
	// Protect against segfault
	if (frameIndex > sampleSpecs.buffersizeInFramesPerPeriode)
		return;
	if (channelIndex > sampleSpecs.channels)
		return;

	if (sampleSpecs.isSigned) {

		int32_t currentMask = 0;

		for (unsigned int byte=0; byte<sampleSpecs.bytesPerSample; byte++)
			currentMask |= (0xFF << (byte*8));

		int32_t currentSample = static_cast<int32_t>(sample * currentMask);

		for (unsigned int byte=0; byte<sampleSpecs.bytesPerSample; byte++) {
			// TODO: if isLittleEndian...
			if (sampleSpecs.isLittleEndian) {
				out[getByteIndex(frameIndex, channelIndex, byte, sampleSpecs)] =
						(currentSample) >> (byte*8);
			} else {
				out[getByteIndex(frameIndex, channelIndex, byte, sampleSpecs)] =
						(currentSample) >> ((sampleSpecs.bytesPerSample-byte-1)*8);
			}
		}
	} else { // UNSIGNED
		//TODO: implement me
	}
}


/** \ingroup Tools
 *
 * \brief Print buffer information using operator<<
 * \param lhs Left hand Side
 * \param rhs Right hand Side
 *
 * Helper overload of operator<< for SampleSpecs_t
 * so it can be used as:
 *
 * \code{.cpp}
 *	std::cout << "SampleSpecs: " << std::endl << mySampleSpecs << std::endl;
 * \endcode
*/
std::ostream& operator<<(std::ostream& lhs, const SampleSpecs& rhs)
{
	lhs << "Samplerate:                        " << rhs.samplerate << std::endl <<
		   "Buffersize in Samples:             " << rhs.buffersizeInSamples << std::endl <<
		   "Buffersize in Samples Per Periode: " << rhs.buffersizeInSamplesPerPeriode << std::endl <<
		   "Buffersize in Frames:              " << rhs.buffersizeInFrames << std::endl <<
		   "Buffersize in Frames Per Periode:  " << rhs.buffersizeInFramesPerPeriode << std::endl <<
		   "Buffersize in Bytes:               " << rhs.buffersizeInBytes << std::endl <<
		   "Buffersize in Bytes Per Periode:   " << rhs.buffersizeInBytesPerPeriode << std::endl <<
		   "Bytes Per Frame:                   " << rhs.bytesPerFrame << std::endl <<
		   "Bytes Per Sample:                  " << rhs.bytesPerSample << std::endl <<
		   "Bytes Per Sample Physical:         " << rhs.bytesPerSamplePhysical << std::endl <<
		   "Channels:                          " << rhs.channels << std::endl <<
		   "isLittleEndian:                    " << rhs.isLittleEndian << std::endl <<
		   "isFloat                            " << rhs.isFloat << std::endl <<
		   "isSigned:                          " << rhs.isSigned << std::endl;
	return lhs;
}

} // namespace Nl
