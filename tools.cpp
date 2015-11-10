#include "tools.h"
#include <cmath>

namespace Nl {

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
std::ostream& operator<<(std::ostream& lhs, const SampleSpecs_t& rhs)
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


/*
auto getTypeForAudioSpec(const SampleSpecs_t &specs)
{

	if (specs.isFloat)
		return 1.f;

	// Signed
	if (specs.isSigned) {
		if (specs.bytesPerSample == 1)
			return (int8_t)0;
		else if (specs.bytesPerFrame == 2)
			return (int16_t)0;
		else if (specs.bytesPerSample == 3 || specs.bytesPerSample == 4)
			return (int32_t)0;
	// Unsigned
	} else {
		if (specs.bytesPerSample == 1)
			return (uint8_t)0;
		else if (specs.bytesPerFrame == 2)
			return (uint16_t)0;
		else if (specs.bytesPerFrame == 3 || specs.bytesPerFrame == 4)
			return (uint32_t)0;
	}
}
*/



} // namespace Nl
