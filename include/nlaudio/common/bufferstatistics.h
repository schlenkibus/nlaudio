#pragma once

// Input Output forward declaration
#include <iosfwd>

namespace Nl {

/** \ingroup Audio
 * \struct BufferStatistics - Stores information on buffer access and un-/overflow counts.
 *
 * This struct can be printed using operator<< to std::out
 *
*/
struct BufferStatistics {
	unsigned long bytesReadFromBuffer; ///< Number of bytes that have been read from the buffer
	unsigned long bytesWrittenToBuffer; ///< Number of bytes that have been written to the buffer
	unsigned int xrunCount; ///< Number of over-/underflows
};

std::ostream& operator<<(std::ostream& lhs, const BufferStatistics& rhs);

} // namespace Nl
