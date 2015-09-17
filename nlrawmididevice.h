#pragma once

#include <alsa/asoundlib.h>
#include <string>
#include <thread>
#include <list>
#include <atomic>

#include "nlmidi.h"
#include "blockingcircularbuffer.h"

class NlRawMidiDeviceException : std::exception
{
public:
	NlRawMidiDeviceException(int errorNumber, std::string what) : m_errno(errorNumber), m_msg(what) {}
	virtual const char* what() const throw() { return m_msg.c_str(); }
private:
	int m_errno;
	std::string m_msg;
};

enum NlMidiDeviceDirection {
	IN,
	OUT,
	IO
};
std::ostream& operator<<(std::ostream& lhs, const NlMidiDeviceDirection& rhs);

struct NlMidiDevice {
	int device;
	int subdevice;
	NlMidiDeviceDirection direction;
	devicename_t name;
	devicename_t subName;
};
std::ostream& operator<<(std::ostream& lhs, const NlMidiDevice& rhs);

struct NlMidiCard {
	int card;
	std::list<struct NlMidiDevice> devices;
};
std::ostream& operator<<(std::ostream& lhs, const NlMidiCard& rhs);


class NlRawMidiDevice : NlMidi
{
public:
	NlRawMidiDevice(const devicename_t& device, std::shared_ptr<BlockingCircularBuffer<unsigned char>> buffer);
	~NlRawMidiDevice();

	static std::list<NlMidiCard> getAvailableDevices();
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
	std::shared_ptr<BlockingCircularBuffer<unsigned char>> m_buffer;

	void throwOnAlsaError(int e, const std::string& function) const;
	void setAlsaMidiBufferSize(unsigned int size);


	static void worker(NlRawMidiDevice *ptr);

};
