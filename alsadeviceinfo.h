#pragma once

#include <string>
#include <list>
#include <iosfwd>
#include "alsasubdeviceinfo.h"

namespace Nl {

struct AlsaDeviceInfo {
	unsigned int deviceId;
	std::string id;
	std::string name;
	std::list<AlsaSubdeviceInfo> subdevices;
};
std::ostream& operator<<(std::ostream& lhs, const AlsaDeviceInfo& rhs);

} // namespace Nl
