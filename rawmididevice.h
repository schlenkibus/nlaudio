#pragma once

#include <alsa/asoundlib.h>
#include <string>
#include <thread>
#include <list>
#include <atomic>

#include "midi.h"
#include "blockingcircularbuffer.h"

namespace Nl {

/** \ingroup Midi
 *
 * \class RawMidiDeviceException
 * \brief Exception object for raw midi
 * \param errorNumber Error number given by alsa
 * \param what Error description
*/
class RawMidiDeviceException : std::exception
{
public:
	RawMidiDeviceException(int errorNumber, std::string what) : m_errno(errorNumber), m_msg(what) {}
	virtual const char* what() const noexcept { return m_msg.c_str(); }
private:
	int m_errno;
	std::string m_msg;
};

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
	devicename_t name;				///< Alsa device name
	devicename_t subName;			///< Alsa subdevice number
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
	RawMidiDevice(const devicename_t& device, std::shared_ptr<BlockingCircularBuffer<uint8_t>> buffer);
	~RawMidiDevice();

	static std::list<MidiCard> getAvailableDevices();
	static devicename_t getFirstDevice();

	virtual void open();
	virtual void close();

	virtual void start();
	virtual void stop();

protected:
	std::atomic<bool> m_requestTerminate;

private:
	snd_rawmidi_t *m_handle;
	snd_rawmidi_params_t *m_params;
	devicename_t m_deviceName;
	int m_buffersize;
	std::thread *m_thread;
	std::shared_ptr<BlockingCircularBuffer<uint8_t>> m_buffer;

	void throwOnAlsaError(int e, const std::string& function) const;
	void setAlsaMidiBufferSize(unsigned int size);


	static void worker(RawMidiDevice *ptr);

};

} // namespace Nl
