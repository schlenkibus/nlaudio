#include "rawmididevice.h"
#include <thread>
#include <iostream>
#include <sstream>

namespace Nl {

NlRawMidiDevice::NlRawMidiDevice(const devicename_t &device, std::shared_ptr<BlockingCircularBuffer<unsigned char> > buffer) :
	m_handle(nullptr),
	m_params(nullptr),
	m_deviceName(device),
	m_buffersize(0),
	m_thread(nullptr),
	m_buffer(buffer)
{
}

void NlRawMidiDevice::open()
{
	throwOnAlsaError(snd_rawmidi_open(&m_handle, NULL, m_deviceName.c_str(), 0), __func__);
	setAlsaMidiBufferSize(3);
}

void NlRawMidiDevice::close()
{
	snd_rawmidi_close(m_handle);
}

void NlRawMidiDevice::start()
{
	m_requestTerminate = false;
	m_thread = new std::thread(NlRawMidiDevice::worker, this);
}

void NlRawMidiDevice::stop()
{
	m_requestTerminate = true;
	m_thread->join();
	delete m_thread;
}

//Static
void NlRawMidiDevice::worker(NlRawMidiDevice *ptr)
{
	const int buffersize = ptr->m_buffersize;

	unsigned char buffer[buffersize];

	snd_rawmidi_drain(ptr->m_handle);

	while(!ptr->m_requestTerminate) {
		if (snd_rawmidi_read(ptr->m_handle, buffer, buffersize) < 0) {
			std::cout << "TODO:" << __func__ << ":" << __LINE__ << " -- " << "Fix errorhandling in reader thread!" << std::endl;
		} else {
			buffer[0] = 0;
			buffer[1] = 1;
			buffer[2] = 2;


			ptr->m_buffer->set(buffer, buffersize);
		}
	}
}

NlRawMidiDevice::~NlRawMidiDevice()
{


}

//TODO: This might ne usefull for the user. For now, it will be private, however
void NlRawMidiDevice::setAlsaMidiBufferSize(unsigned int size)
{
	//throwOnAlsaError(snd_rawmidi_params_set_buffer_size(m_handle, m_params, size), __func__);
	m_buffersize = size;
}

/// Error Handling
void NlRawMidiDevice::throwOnAlsaError(int e, const std::string &function) const
{
	if (e < 0) {
		throw RawMidiDeviceException(e, function + ": " + snd_strerror(e));
	}
}

///Static
std::list<NlMidiCard> NlRawMidiDevice::getAvailableDevices()
{
	std::list<NlMidiCard> ret;
	int currentCard = -1;
	int currentDevice = -1;

	int err = snd_card_next(&currentCard);
	if (err < 0) {
		std::cout << "Error! " << __func__ << ":" << __LINE__ << std::endl;
		return ret;
	}

	// No cards available
	if (currentCard < 0)
		return ret;

	// Iterate over all cards available
	do {
		// Get Details for current card
		snd_ctl_t *ctl;
		char name[32];

		sprintf(name, "hw:%d", currentCard);

		err = snd_ctl_open(&ctl, name, 0);
		if (err < 0) {
			std::cout << "Error! " << __func__ << ":" << __LINE__ << std::endl;
			return ret;
		}

		NlMidiCard card;
		card.card = currentCard;


		// Iterate over all devices available
		for (;;) {
			err = snd_ctl_rawmidi_next_device(ctl, &currentDevice);
			if (err < 0) {
				std::cout << "Error! " << __func__ << ":" << __LINE__ << std::endl;
				return ret;
			}

			// No Devices Available
			if (currentDevice < 0)
				break;

			snd_rawmidi_info_t *info;
			int subs, subsIn, subsOut;
			int sub;
			int err;

			snd_rawmidi_info_alloca(&info);
			snd_rawmidi_info_set_device(info, currentDevice);

			snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);
			err = snd_ctl_rawmidi_info(ctl, info);
			if (err >= 0)
				subsIn = snd_rawmidi_info_get_subdevices_count(info);
			else
				subsIn = 0;

			snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_OUTPUT);
			err = snd_ctl_rawmidi_info(ctl, info);
			if (err >= 0)
				subsOut = snd_rawmidi_info_get_subdevices_count(info);
			else
				subsOut = 0;

			subs = subsIn > subsOut ? subsIn : subsOut;
			if (!subs)
				break;

			for (sub=0; sub<subs; ++sub) {
				snd_rawmidi_info_set_stream(info, sub < subsIn ?
												SND_RAWMIDI_STREAM_INPUT :
												SND_RAWMIDI_STREAM_OUTPUT);

				snd_rawmidi_info_set_subdevice(info, sub);

				err = snd_ctl_rawmidi_info(ctl, info);

				MidiDevice device;
				device.name = snd_rawmidi_info_get_name(info);
				device.subName = snd_rawmidi_info_get_subdevice_name(info);
				if (sub < subsIn && sub < subsOut)
					device.direction = IO;
				else if (sub < subsIn)
					device.direction = IN;
				else if (sub < subsOut)
					device.direction = OUT;
				else
					std::cout << "Error! " << __func__ << ":" << __LINE__ << std::endl;

				device.device = currentDevice;
				device.subdevice = sub;
				card.devices.insert(card.devices.end(), device);
			}

			ret.insert(ret.end(), card);
		}
		snd_ctl_close(ctl);

		if ((err = snd_card_next(&currentCard)) < 0) {
			std::cout << "Error! " << __func__ << ":" << __LINE__ << std::endl;
			break;
		}

	} while (currentCard >= 0);

	return ret;
}

///Static
devicename_t NlRawMidiDevice::getFirstDevice()
{
	auto devices = getAvailableDevices();

	devicename_t ret;
	if (devices.size()) {
		NlMidiCard firstCard = devices.front();
		MidiDevice firstDevice = firstCard.devices.front();


		std::stringstream ss;
		ss << "hw:" << firstCard.card << "," << firstDevice.device << "," << firstDevice.subdevice;
		return ss.str();
	}
	return "";
}

/// Debugging Helpers
std::ostream& operator<<(std::ostream& lhs, const NlMidiDeviceDirection& rhs)
{
	if (rhs == IN)
		lhs << "IN ";
	else if (rhs == OUT)
		lhs << "OUT";
	else if (rhs == IO)
		lhs << "IO ";

	return lhs;
}

std::ostream& operator<<(std::ostream& lhs, const MidiDevice& rhs)
{
	lhs <<  ":" << rhs.device << ":" << rhs.subdevice << "\t\t" << rhs.name /* << "\t" << rhs.subName */ << "\t" << rhs.direction;
	return lhs;
}

std::ostream& operator<<(std::ostream& lhs, const NlMidiCard& rhs)
{
	auto iter = rhs.devices.begin();
	while (iter != rhs.devices.end()) {
		lhs << rhs.card << *iter << std::endl;
		++iter;
	}
	return lhs;
}

}  // namespace Nl
