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
