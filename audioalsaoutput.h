#pragma once

#include "audioalsa.h"

namespace Nl {

class AudioAlsaOutput : public AudioAlsa
{
public:
	typedef AudioAlsa basetype;

	AudioAlsaOutput(const devicename_t &name, std::shared_ptr<BlockingCircularBuffer<u_int8_t> > buffer);

	virtual void open();
	virtual void start();
	virtual void stop();

	static void worker(SampleSpecs_t specs, AudioAlsaOutput *ptr);
};

} // Namespace Nl
