#include "alsadeviceinfo.h"

#include <ostream>

namespace Nl {

std::ostream& operator<<(std::ostream& lhs, const AlsaDeviceInfo& rhs)
{
	lhs << "Device[" << rhs.deviceId << "] "
		   "Name: " << rhs.name << ", " <<
		   "Id: " << rhs.id << ", " <<
		   "Subdevices: " << rhs.subdevices.size();

	return lhs;
}

}
