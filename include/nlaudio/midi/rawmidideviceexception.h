#pragma once

#include <exception>
#include <string>

namespace Nl {

/** \ingroup Midi
 *
 * \class RawMidiDeviceException
 * \brief Exception object for raw midi
 * \param errorNumber Error number given by alsa
 * \param what Error description
*/
class RawMidiDeviceException : std::exception
{
public:
	RawMidiDeviceException(int errorNumber, std::string what);
	virtual const char* what() const noexcept;
private:
	int m_errno;
	std::string m_msg;
};

} // namespace Nl
