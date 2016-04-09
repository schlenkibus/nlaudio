#pragma once

#include "audio.h"
#include "bufferstatistics.h"
#include "alsacardidentifier.h"
#include "blockingcircularbuffer.h"

#include <jack/jack.h>

namespace Nl {

class AudioJack : public Audio
{
public:
	typedef Audio basetype;

	AudioJack(const AlsaCardIdentifier& card, SharedBufferHandle buffer, bool isInput);
	virtual ~AudioJack();

	virtual void open(); // Might throw, therefore not in constructor
	virtual void close();

	virtual void start();
	virtual void stop();

	virtual void init();

	//TODO: Changing buffer size, while playback is running might have strange sideeffects!
	virtual void setBuffersize(unsigned int buffersize);
	virtual unsigned int getBuffersize();

	virtual void setBufferCount(unsigned int buffercount);
	virtual unsigned int getBufferCount();

	virtual samplerate_t getSamplerate() const;
	virtual void setSamplerate(samplerate_t rate);

	virtual std::list<sampleformat_t> getAvailableSampleformats() const;
	virtual sampleformat_t getSampleFormat() const;
	virtual void setSampleFormat(sampleformat_t format);

	virtual void setChannelCount(channelcount_t n);
	virtual channelcount_t getChannelCount();

	BufferStatistics getStats();

protected:
	bool m_isInput;
	SharedBufferHandle m_audioBuffer;
	jack_port_t *m_jackPort;
	jack_client_t *m_jackClient;
	AlsaCardIdentifier m_card;

private:
	static int worker(jack_nframes_t nframes, void *arg);

}; // namespace Nl

/*! A shared handle to a \ref AudioJack */
typedef std::shared_ptr<AudioJack> SharedAudioJackHandle;

}
