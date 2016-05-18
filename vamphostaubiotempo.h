#ifndef VAMPHOSTAUBIOTEMPO_H
#define VAMPHOSTAUBIOTEMPO_H

#include "vamphost.h"



class VampHostAubioTempo : public VampHost
{
public:
	typedef VampHost basetype;

	VampHostAubioTempo(const std::string &libraryName, const std::string &pluginName, unsigned int samplerate, unsigned int printIndex);
	virtual void process(uint8_t *samples, const Nl::SampleSpecs &specs);
};

#endif // VAMPHOSTAUBIOTEMPO_H
