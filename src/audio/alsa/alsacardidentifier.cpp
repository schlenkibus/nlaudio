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

#include "audio/alsa/alsacardidentifier.h"
#include "audio/alsa/alsacardinfo.h"
#include <string>
#include <sstream>

namespace Nl {

/** \ingroup Audio
 *
 * \brief Constructor
 * \param card Card identification number
 * \param device Device identification number
 * \param subdevice Subdevice identification number
 * \param name Card name
 *
 * Initializes a \ref AlsaDeviceIdentifier object for one device on the plattform
 *
*/
AlsaCardIdentifier::AlsaCardIdentifier(unsigned int card,
									   unsigned int device,
									   unsigned int subdevice,
									   const std::string& name) :
	m_card(card),
	m_device(device),
	m_subdevice(subdevice),
	m_name(name)
{
	std::stringstream ss;
	ss << "hw:" << card << "," << device << "," << subdevice;
	m_cardString = ss.str();
}

std::string AlsaCardIdentifier::getCardString() const
{
	return m_cardString;
}

std::string AlsaCardIdentifier::getCardStringExtended() const
{
	return m_name + " " + m_cardString;
}

//Static
std::vector<Nl::AlsaCardIdentifier> AlsaCardIdentifier::getCardIdentifiers()
{
    std::vector<Nl::AlsaCardIdentifier> ret;

	auto cardInfoList = Nl::getDetailedCardInfos();

	for (auto cardIt=cardInfoList.begin(); cardIt!=cardInfoList.end(); ++cardIt)
		for (auto deviceIt=cardIt->devices.begin(); deviceIt!=cardIt->devices.end(); ++deviceIt)
			for (auto subdeviceIt=deviceIt->subdevices.begin(); subdeviceIt!=deviceIt->subdevices.end(); ++subdeviceIt) {
				int card = cardIt->cardId;
				int device = deviceIt->deviceId;
				int subdevice = subdeviceIt->subdeviceId;
				std::string name = cardIt->name;
				ret.push_back(Nl::AlsaCardIdentifier(card, device, subdevice, name));
			}

	return ret;
}

std::ostream& operator<<(std::ostream& lhs, const AlsaCardIdentifier& rhs)
{
	lhs << rhs.getCardStringExtended();
	return lhs;
}

} // namespace Nl


