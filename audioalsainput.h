#pragma once

#include "audioalsa.h"

namespace Nl {

class AudioAlsaInput : public AudioAlsa
{
public:
	typedef AudioAlsa basetype;

	AudioAlsaInput(const devicename_t &name, std::shared_ptr<BlockingCircularBuffer<u_int8_t>> buffer);

	virtual void open();
	virtual void start();
	virtual void stop();

	static void worker(SampleSpecs_t specs, AudioAlsaInput *ptr);
};

} // Namespace Nl
