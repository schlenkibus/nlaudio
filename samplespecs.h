#pragma once

#include <iosfwd>
#include <limits>
#include <stdint.h>

namespace Nl {

/** \ingroup Audio
 * \struct SampleSpecs - Stores Sampleinformation for an audio interface
 *
*/
struct SampleSpecs {
	unsigned int samplerate;				///< Samplerate in Hz
	unsigned int channels;					///< Channels
	unsigned int bytesPerSample;			///< How many bytes does one sample have. 24_BE3 = 3, S16 = 2, ...
	unsigned int bytesPerSamplePhysical;	///< Sometimes 24_BE3 can be stored in 4Bytes, then this would be 4. Usually same as bytesPerSample
	unsigned int bytesPerFrame;				///< How many bytes does one frame have. Same as channels * bytesPerSample
	unsigned int buffersizeInFrames;		///< Buffersize in Frames
	unsigned int buffersizeInFramesPerPeriode;	///< Buffersize in Frames per Periode. Same as buffersizeInFrames / periodes
	unsigned int buffersizeInBytes;				///< Buffersize in Bytes
	unsigned int buffersizeInBytesPerPeriode;	///< Buffersize in Bytes per Periode. Same as buffersizeInBytes / periodes
	unsigned int buffersizeInSamples;			///< Buffersize in Samples. Same as buffersizeInBytes / bytesPerSample
	unsigned int buffersizeInSamplesPerPeriode; ///< Buffersize in Samples per Periode. Same as buffersizeInSamples / periodes
	bool isFloat;								///< Are we working with floating point samples?
	bool isLittleEndian;						///< Are we working in little endian?
	bool isSigned;								///< Are we using a sample format with signed values?
	// bool isInterleaved
};

std::ostream& operator<<(std::ostream& lhs, const SampleSpecs& rhs);

unsigned int getByteIndex(unsigned int frameIndex, unsigned int channel, unsigned int byte, const SampleSpecs &sampleSpecs);
float getSample(u_int8_t* in, u_int32_t frameIndex, u_int32_t channelIndex, const SampleSpecs& sampleSpecs);
void setSample(u_int8_t* out, float sample, u_int32_t frameIndex, u_int32_t channelIndex, const SampleSpecs& sampleSpecs);



#if 0
//TODO: Implement little/bigendian
template <typename T>
T getSample(u_int8_t* in, u_int32_t frameIndex, u_int32_t channelIndex, const SampleSpecs& sampleSpecs)
{

	//if (std::numeric_limits<T>::is_signed != sampleSpecs.isSigned)
	//	std::cout << __func__ << " signed/unsigned missmatch" << std::endl;

	T currentSample = 0;
	T currentMask = 0;

	for (unsigned int byte=0; byte<sampleSpecs.bytesPerSample; byte++) {
		currentSample |= in[getByteIndex(frameIndex, channelIndex, byte, sampleSpecs)] << ((sampleSpecs.bytesPerSample-byte-1)*8);
		currentMask |= (0xFF << (byte*8));
	}

	if (sampleSpecs.isSigned) {
		// If MSB is 1 we deal with negative numbers.
		if (currentSample & (1 << (sampleSpecs.bytesPerSample*8-1)))
			currentSample |= ~currentMask;
	}

	return currentSample;
}

template <>
float getSample(u_int8_t* in, u_int32_t frameIndex, u_int32_t channelIndex, const SampleSpecs& sampleSpecs)
{
	int32_t currentMask = 0;
	for (unsigned int byte=0; byte<sampleSpecs.bytesPerSample; byte++) {
		currentMask |= (0xFF << (byte*8));
	}

	int32_t currentSample = getSample<int32_t>(in, frameIndex, channelIndex, sampleSpecs);
	return static_cast<float>(currentSample) / static_cast<float>(currentMask);
}
#endif


} // namespace Nl
