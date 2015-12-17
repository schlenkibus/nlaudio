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
