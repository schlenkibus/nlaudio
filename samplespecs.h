#pragma once

#include <iosfwd>

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

} // namespace Nl
