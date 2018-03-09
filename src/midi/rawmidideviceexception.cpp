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

#include "midi/rawmidideviceexception.h"
#include <string>

namespace Nl {

/** \ingroup Midi
 *
 * \brief Constructor
 * \param errorNumber Alsa error number
 * \param what Descriptive string
 *
*/
RawMidiDeviceException::RawMidiDeviceException(int errorNumber, std::string what) :
	m_errno(errorNumber),
	m_msg(what) {}

/** \ingroup Midi
 *
 * \brief Returns the desciptive string of the exception
 * \return A deciptive string of the exception
*/
const char* RawMidiDeviceException::what() const noexcept
{
	return m_msg.c_str();
}

} // namepsace Nl
