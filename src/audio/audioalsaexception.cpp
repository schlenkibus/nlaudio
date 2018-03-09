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

#include "audio/audioalsaexception.h"

#include <sstream>

namespace Nl {

/** \ingroup Audio
 *
 * \brief Constructor
 * \param func Function name
 * \param file File name
 * \param line Line number
 * \param errorNumber Alsa error number
 * \param what Descriptive string
 *
*/
AudioAlsaException::AudioAlsaException(std::string func, std::string file, int line, int errorNumber, std::string what) :
	m_func(func),
	m_file(file),
	m_msg(what),
	m_line(line),
	m_errno(errorNumber) {}

/** \ingroup Audio
 *
 * \brief Returns the desciptive string of the exception
 * \return A deciptive string of the exception
*/
const char* AudioAlsaException::what() const noexcept
{
	std::stringstream ss;
	ss << m_file << ":" << m_func << ":" << m_line << ": " << m_msg;
	return ss.str().c_str();
}

} // namespace Nl
