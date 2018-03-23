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

#include <alsa/asoundlib.h>
#include <string>
#include <list>
#include <atomic>
#include <memory>
#include <thread>

#include "midi/midi.h"
#include "common/alsa/alsacardidentifier.h"
#include "common/blockingcircularbuffer.h"

namespace Nl {



///< Definition of dataflow direction
enum MidiDeviceDirection {
	IN,		///< Midi input only
	OUT,	///< Midi output only
	IO		///< Midi Input and Output
};
std::ostream& operator<<(std::ostream& lhs, const MidiDeviceDirection& rhs);

/// Definition of MidiDevice
struct MidiDevice {
	int device;						///< Alsa device number
	int subdevice;					///< Alsa subdevice number
	MidiDeviceDirection direction;	///< Alsa dataflow direction
	std::string name;				///< Alsa device name
	std::string subName;			///< Alsa subdevice number
};
std::ostream& operator<<(std::ostream& lhs, const MidiDevice& rhs);

/// Definition of MidiCard
struct MidiCard {
	int card;						///< Alsa card number
	std::list<struct MidiDevice> devices; ///< List of MidiDevices
};
std::ostream& operator<<(std::ostream& lhs, const MidiCard& rhs);

/** \ingroup Midi
 *
 * \class RawMidiDevice
 * \brief Midi implementation for alsa raw midi
 * \param device Alsa device id such as "hw:0,1"
 * \param buffer Buffer to store midi data to
*/
class RawMidiDevice : public Midi
{
public:
	RawMidiDevice(const AlsaCardIdentifier &card, std::shared_ptr<BlockingCircularBuffer<uint8_t>> buffer);
	~RawMidiDevice();

	static std::list<MidiCard> getAvailableDevices();
	static std::string getFirstDevice();

	virtual void open();
	virtual void close();

	virtual void start();
	virtual void stop();

	void setAlsaMidiBufferSize(unsigned int size);
	unsigned int getAlsaMidiBufferSize();

protected:
	std::atomic<bool> m_requestTerminate;

private:
	snd_rawmidi_t *m_handle;
	snd_rawmidi_params_t *m_params;
	AlsaCardIdentifier m_card;
	int m_buffersize;
	std::thread *m_thread;
	std::shared_ptr<BlockingCircularBuffer<uint8_t>> m_buffer;

	void throwOnAlsaError(int e, const std::string& function) const;

	static void worker(RawMidiDevice *ptr);

};

/*! A shared handle to a \ref RawMidiDevice */
typedef std::shared_ptr<RawMidiDevice> SharedRawMidiDeviceHandle;

} // namespace Nl
