#include "tools.h"
#include <cmath>

namespace Nl {

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





} // namespace Nl
