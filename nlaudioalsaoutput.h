#pragma once

#include "nlaudioalsa.h"

class NlAudioAlsaOutput : public NlAudioAlsa
{
public:
	typedef NlAudioAlsa basetype;

	NlAudioAlsaOutput(const devicename_t &name);

	virtual void open();
	virtual void start();
	virtual void stop();

	static void worker(NlAudioAlsaOutput *ptr);
};
