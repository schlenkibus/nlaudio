#pragma once

#include <alsa/asoundlib.h>
#include <string>
#include <thread>
#include <list>
#include <atomic>

#include "midi.h"
#include "blockingcircularbuffer.h"

namespace Nl {

class RawMidiDeviceException : std::exception
{
public:
	RawMidiDeviceException(int errorNumber, std::string what) : m_errno(errorNumber), m_msg(what) {}
	virtual const char* what() const noexcept { return m_msg.c_str(); }
private:
	int m_errno;
	std::string m_msg;
};

enum MidiDeviceDirection {
	IN,
	OUT,
	IO
};
std::ostream& operator<<(std::ostream& lhs, const MidiDeviceDirection& rhs);

struct MidiDevice {
	int device;
	int subdevice;
	MidiDeviceDirection direction;
	devicename_t name;
	devicename_t subName;
};
std::ostream& operator<<(std::ostream& lhs, const MidiDevice& rhs);

struct MidiCard {
	int card;
	std::list<struct MidiDevice> devices;
};
std::ostream& operator<<(std::ostream& lhs, const MidiCard& rhs);


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
