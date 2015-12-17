#include "bufferstatistics.h"
#include <ostream>

namespace Nl {

/** \ingroup Audio
 *
 * \brief Print BufferStatistics using operator<<
 * \param lhs Left hand Side
 * \param rhs Right hand Side
 * \return std::ostream with string data
 *
 * Helper overload of operator<< for BufferStatistics
 * so it can be used as:
 *
 * \code{.cpp}
 *	std::cout << "Statistics: " << std::endl << handle->getStats() << std::endl;
 * \endcode
*/
std::ostream& operator<<(std::ostream& lhs, const BufferStatistics& rhs)
{
	lhs << "  Bytes Read From Buffer:   " << rhs.bytesReadFromBuffer << std::endl
		<< "  Bytes Written To Buffer:  " << rhs.bytesWrittenToBuffer << std::endl
		<< "  Over-/Underrun Count:     " << rhs.xrunCount << std::endl;

	return lhs;
}

}
