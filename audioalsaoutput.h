#pragma once

#include "audioalsa.h"

namespace Nl {

class AudioAlsaOutput : public AudioAlsa
{
public:
	typedef AudioAlsa basetype;

	AudioAlsaOutput(const AlsaCardIdentifier& card, SharedBufferHandle buffer);

	virtual void open();
	virtual void start();
	virtual void stop();

	static void worker(SampleSpecs specs, AudioAlsaOutput *ptr);
};

/*! A shared handle to a \ref AudioAlsaOutput */
typedef std::shared_ptr<AudioAlsaOutput> SharedAudioAlsaOutputHandle;

} // Namespace Nl
