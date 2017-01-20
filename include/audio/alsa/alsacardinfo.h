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
