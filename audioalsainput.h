#pragma once

#include "audioalsa.h"

namespace Nl {

class AudioAlsaInput : public AudioAlsa
{
public:
	typedef AudioAlsa basetype;

	AudioAlsaInput(const devicename_t &name, std::shared_ptr<BlockingCircularBuffer<char>> buffer);

	virtual void open();
	virtual void start();
	virtual void stop();

	static void worker(SampleSpecs specs, AudioAlsaInput *ptr);
};

} // Namespace Nl
