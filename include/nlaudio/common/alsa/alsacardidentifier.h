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
#include <vector>
#include <iosfwd>

namespace Nl {

class AlsaCardIdentifier {
public:
    AlsaCardIdentifier(unsigned int card, unsigned int device, unsigned int subdevice, const std::string& name);

    std::string getCardString() const;
    std::string getCardStringExtended() const;

    //virtual std::vector<AlsaCardIdentifier> getIdentifiers() = 0;

private:
    unsigned int m_card;
    unsigned int m_device;
    unsigned int m_subdevice;
    std::string m_cardString;
    std::string m_name;
};


class AlsaAudioCardIdentifier : public AlsaCardIdentifier {
public:
    typedef AlsaCardIdentifier basetype;

    AlsaAudioCardIdentifier(unsigned int card, unsigned int device, unsigned int subdevice, const std::string& name);

    //virtual std::vector<AlsaCardIdentifier> getIdentifiers();

    static std::vector<AlsaAudioCardIdentifier> getCardIdentifiers();
};

std::ostream& operator<<(std::ostream& lhs, const AlsaAudioCardIdentifier& rhs);

class AlsaMidiCardIdentifier : public AlsaCardIdentifier {
public:
    typedef AlsaCardIdentifier basetype;

    AlsaMidiCardIdentifier(unsigned int card, unsigned int device, unsigned int subdevice, const std::string& name);

    //virtual std::vector<AlsaMidiCardIdentifier> getIdentifiers();
};

std::ostream& operator<<(std::ostream& lhs, const AlsaMidiCardIdentifier& rhs);

} // namespace Nl

