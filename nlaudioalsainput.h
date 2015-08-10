#pragma once

#include "nlaudioalsa.h"

class NlAudioAlsaInput : public NlAudioAlsa
{
public:
	typedef NlAudioAlsa basetype;

	NlAudioAlsaInput(const devicename_t &name);

	virtual void open();
	virtual void start();
	virtual void stop();

	static void worker(NlAudioAlsaInput *ptr);

};
