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

namespace Nl {

/** \ingroup Audio
 *
 * \brief Exception object, used to throw alsa exceptions
 *
 * Exception object, which is used to throw alsa exception by:
 *  - AudioAlsa
 *  - AudioAlsaInput
 *  - AudioAlsaOutput
 *
*/
class AudioAlsaException : public std::exception
{
public:
	AudioAlsaException(std::string func, std::string file, int line, int errorNumber, std::string what);
	virtual const char* what() const noexcept;

private:
	std::string m_func;
	std::string m_file;
	std::string m_msg;
	int m_line;
	int m_errno;
};

} // namespace Nl
