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

#include <string>
#include <iosfwd>

namespace Nl {

/** \ingroup Audio
 *
 * \brief Class that can store identificationdata for a device on the platform
 *
*/
struct AlsaSubdeviceInfo {
	unsigned int subdeviceId;
	std::string name;
};
std::ostream& operator<<(std::ostream& lhs, const AlsaSubdeviceInfo& rhs);

} // namespace Nl
