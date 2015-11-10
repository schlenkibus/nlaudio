#pragma once

/** \defgroup Midi
 *
 * \brief Midi subsystem for NlAudio Framework
 *
*/
namespace Nl {

typedef std::string devicename_t;

class Midi
{
public:
	virtual void open() = 0;
	virtual void close() = 0;

	virtual void start() = 0;
	virtual void stop() = 0;
};
} // namespace Nl
