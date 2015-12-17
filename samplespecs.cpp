
#include <iostream>

#include "samplespecs.h"

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
