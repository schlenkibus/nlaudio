#pragma once

#include "nlaudioalsa.h"

class NlAudioAlsaInput : public NlAudioAlsa
{
public:
	typedef NlAudioAlsa basetype;

	NlAudioAlsaInput(const devicename_t &name, std::shared_ptr<CircularAudioBuffer<char>> buffer);

	virtual void open();
	virtual void start();
	virtual void stop();

	static void worker(SampleSpecs specs, NlAudioAlsaInput *ptr);

};
