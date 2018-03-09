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
