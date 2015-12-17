#include "alsasubdeviceinfo.h"
#include <ostream>

namespace Nl {

std::ostream& operator<<(std::ostream& lhs, const AlsaSubdeviceInfo& rhs)
{
	lhs << "Subdevice[" << rhs.subdeviceId << "] "
		   "Name: " << rhs.name;
	return lhs;
}

} // namespace Nl
