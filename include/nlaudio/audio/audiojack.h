/***
  Copyright (c) 2018 Nonlinear Labs GmbH

  Authors: Pascal Huerst <pascal.huerst@gmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
***/

#pragma once

#include "audio/audio.h"
#include "common/bufferstatistics.h"
#include "common/alsa/alsacardidentifier.h"
#include "common/blockingcircularbuffer.h"

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

	virtual BufferStatistics getStats();

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
