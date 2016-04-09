#ifndef VAMPHOSTAUBIOTEMPO_H
#define VAMPHOSTAUBIOTEMPO_H

#include "vamphost.h"



class VampHostAubioTempo : public VampHost
{
public:
	typedef VampHost basetype;

	VampHostAubioTempo(unsigned int samplerate);
	virtual void process(uint8_t *samples, const Nl::SampleSpecs &specs);
};

#endif // VAMPHOSTAUBIOTEMPO_H
