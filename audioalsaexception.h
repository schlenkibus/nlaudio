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
