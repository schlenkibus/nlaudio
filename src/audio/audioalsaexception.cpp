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
