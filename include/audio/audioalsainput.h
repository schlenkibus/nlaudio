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
	virtual void init();

	static void worker(SampleSpecs specs, AudioAlsaInput *ptr);
};

typedef std::shared_ptr<AudioAlsaInput> SharedAudioAlsaInputHandle;

} // Namespace Nl
