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
#include <list>
#include <iosfwd>
#include "alsadeviceinfo.h"


namespace Nl {
struct AlsaCardInfo {
	unsigned int cardId;
	std::string id;
	std::string components;
	std::string driver;
	std::string name;
	std::string longname;
	std::string mixername;
	std::list<Nl::AlsaDeviceInfo> devices;
};

std::ostream& operator<<(std::ostream& lhs, const AlsaCardInfo& rhs);
std::list<AlsaCardInfo> getDetailedCardInfos();

} // namespace Nl
