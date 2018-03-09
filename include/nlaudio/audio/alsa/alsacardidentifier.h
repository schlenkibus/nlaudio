#pragma once

#include <string>
#include <vector>
#include <iosfwd>

namespace Nl {

class AlsaCardIdentifier {
public:
	   AlsaCardIdentifier(unsigned int card, unsigned int device, unsigned int subdevice, const std::string& name);

	   std::string getCardString() const;
	   std::string getCardStringExtended() const;
       static std::vector<AlsaCardIdentifier> getCardIdentifiers();
private:
	   unsigned int m_card;
	   unsigned int m_device;
	   unsigned int m_subdevice;
	   std::string m_cardString;
	   std::string m_name;
};

std::ostream& operator<<(std::ostream& lhs, const Nl::AlsaCardIdentifier& rhs);

} // namespace Nl

