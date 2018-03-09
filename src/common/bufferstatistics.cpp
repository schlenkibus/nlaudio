/***
  Copyright (c) 2018 Nonlinear Labs GmbH

  Authors: Pascal Huerst <pascal.huerst@gmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
***/

#include "common/bufferstatistics.h"
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
