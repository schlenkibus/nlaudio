#pragma once

#include "audioalsa.h"

namespace Nl {

class AudioAlsaInput : public AudioAlsa
{
public:
	typedef AudioAlsa basetype;

	AudioAlsaInput(const AlsaCardIdentifier& card, SharedBufferHandle buffer);

	virtual void open();
	virtual void start();
	virtual void stop();

	static void worker(SampleSpecs specs, AudioAlsaInput *ptr);
};

/*! A shared handle to a \ref AudioAlsaInput */
typedef std::shared_ptr<AudioAlsaInput> SharedAudioAlsaInputHandle;

} // Namespace Nl
