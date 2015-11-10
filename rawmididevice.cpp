#include "rawmididevice.h"
#include <thread>
#include <iostream>
#include <sstream>

namespace Nl {

/** \ingroup Midi
 *
 * \brief Constructor
 * \param device Alsa device id such as "hw:0,1"
 * \param buffer Buffer to store midi data to
 *
 * Constructor for RawMidiDevice
 *
*/
RawMidiDevice::RawMidiDevice(const devicename_t &device, std::shared_ptr<BlockingCircularBuffer<uint8_t>> buffer) :
	m_handle(nullptr),
	m_params(nullptr),
	m_deviceName(device),
	m_buffersize(0),
	m_thread(nullptr),
	m_buffer(buffer)
{
}

/** \ingroup Midi
 *
 * \brief Open the interface
 * \param device Alsa device id such as "hw:0,1"
 * \param buffer Buffer to store midi data to
 *
 * Open the interface. Note, that the interface has to be opened,
 * before any other operation is performed.
 *
*/
void RawMidiDevice::open()
{
	throwOnAlsaError(snd_rawmidi_open(&m_handle, NULL, m_deviceName.c_str(), 0), __func__);
	// A Midi message has 3 bytes, so we get every Message imediately
	setAlsaMidiBufferSize(3);
    // Maybe we miss some, so make circular buffer a little bigger (10 Messages -> 30 Bytes)
    m_buffer->init(30);
}

/** \ingroup Midi
 *
 * \brief Close the interface
 *
 * Closes the interface. Should be called,
 * before deleting the object.
 *
*/
void RawMidiDevice::close()
{
	snd_rawmidi_close(m_handle);
}

/** \ingroup Midi
 *
 * \brief Start the interface
 *
 * This starts the worker thread on the interface
 *
*/
void RawMidiDevice::start()
{
	m_requestTerminate = false;
	m_thread = new std::thread(RawMidiDevice::worker, this);
}

/** \ingroup Midi
 *
 * \brief Stop the interface
 *
 * This stops the worker thread on the interface
 *
*/
void RawMidiDevice::stop()
{
	m_requestTerminate = true;
	m_thread->join();
	delete m_thread;
}

/** \ingroup Midi
 *
 * \brief Static worker thread iomplementation
 *
 * This function is static and run within its own thread/context since the interface might block,
 * if there is nothing to read.
 *
*/
void RawMidiDevice::worker(RawMidiDevice *ptr)
{
	const int buffersize = ptr->m_buffersize;

	uint8_t buffer[buffersize];

	snd_rawmidi_drain(ptr->m_handle);

	while(!ptr->m_requestTerminate) {
		if (snd_rawmidi_read(ptr->m_handle, buffer, buffersize) < 0) {
			std::cout << "TODO:" << __func__ << ":" << __LINE__ << " -- " << "Fix errorhandling in reader thread!" << std::endl;
		} else {
			ptr->m_buffer->set(buffer, buffersize);
		}
	}
}

/** \ingroup Midi
 *
 * \brief Deconstructor
 *
 * Nothing special here, so far.
 *
*/
RawMidiDevice::~RawMidiDevice()
{
}

/** \ingroup Midi
 *
 * \brief Set Alsa's midi buffer size
 * \param size Buffersize in bytes
 *
 * If you set size to 3, you will get every midi message immediately,
 * since a midi message has 3 bytes. Setting it to lower values will
 * add overhead, but no real advantage. Setting it to a bigger value
 * delays the delivery of the values until this value is reached.
 *
*/
//TODO: This might ne usefull for the user. For now, it will be private, however
void RawMidiDevice::setAlsaMidiBufferSize(unsigned int size)
{
	throwOnAlsaError(snd_rawmidi_params_set_buffer_size(m_handle, m_params, size), __func__);
	m_buffersize = size;
}

/** \ingroup Midi
 *
 * \brief Checks return values of alsa calls
 * \throws RawMidiDeviceException
 * \param e Alsa error number
 * \param function name of the calling function
 *
 * Helper function, that checks return values of alsa calls and throws an exception if it
 * was an error code.
 *
*/
void RawMidiDevice::throwOnAlsaError(int e, const std::string &function) const
{
	if (e < 0) {
		throw RawMidiDeviceException(e, function + ": " + snd_strerror(e));
	}
}

/** \ingroup Midi
 *
 * \brief Static function that lists alsa midi devices
 * \return List of available MidiCard elements
 *
 * Lists all available midi devices
 *
*/
std::list<MidiCard> RawMidiDevice::getAvailableDevices()
{
	std::list<MidiCard> ret;
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

		MidiCard card;
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

/** \ingroup Midi
 *
 * \brief Static function that returns first available alsa midi device
 * \return First available MidiCard element
 *
 * Return first available alsa midi device
 *
*/
devicename_t RawMidiDevice::getFirstDevice()
{
	auto devices = getAvailableDevices();

	devicename_t ret;
	if (devices.size()) {
		MidiCard firstCard = devices.front();
		MidiDevice firstDevice = firstCard.devices.front();


		std::stringstream ss;
		ss << "hw:" << firstCard.card << "," << firstDevice.device << "," << firstDevice.subdevice;
		return ss.str();
	}
	return "";
}

/** \ingroup Midi
 *
 * \brief Print MidiDeviceDirection using operator<<
 * \param lhs Left hand Side
 * \param rhs Right hand Side
 * \return std::ostream with string data
 *
 * Helper overload of operator<< for MidiDeviceDirection
 * so it can be used as:
 *
 * \code{.cpp}
 *	std::cout << "Direction: " << std::endl << myMidiDeviceDirection << std::endl;
 * \endcode
*/
std::ostream& operator<<(std::ostream& lhs, const MidiDeviceDirection& rhs)
{
	if (rhs == IN)
		lhs << "IN ";
	else if (rhs == OUT)
		lhs << "OUT";
	else if (rhs == IO)
		lhs << "IO ";

	return lhs;
}

/** \ingroup Midi
 *
 * \brief Print MidiDevice using operator<<
 * \param lhs Left hand Side
 * \param rhs Right hand Side
 * \return std::ostream with string data
 *
 * Helper overload of operator<< for MidiDevice
 * so it can be used as:
 *
 * \code{.cpp}
 *	std::cout << "MidiDevice: " << std::endl << midiDevice << std::endl;
 * \endcode
*/
std::ostream& operator<<(std::ostream& lhs, const MidiDevice& rhs)
{
	lhs <<  ":" << rhs.device << ":" << rhs.subdevice << "\t\t" << rhs.name /* << "\t" << rhs.subName */ << "\t" << rhs.direction;
	return lhs;
}

/** \ingroup Midi
 *
 * \brief Print MidiCard using operator<<
 * \param lhs Left hand Side
 * \param rhs Right hand Side
 * \return std::ostream with string data
 *
 * Helper overload of operator<< for MidiCard
 * so it can be used as:
 *
 * \code{.cpp}
 *	std::cout << "MidiCard: " << std::endl << midiCard << std::endl;
 * \endcode
*/
std::ostream& operator<<(std::ostream& lhs, const MidiCard& rhs)
{
	auto iter = rhs.devices.begin();
	while (iter != rhs.devices.end()) {
		lhs << rhs.card << *iter << std::endl;
		++iter;
	}
	return lhs;
}

}  // namespace Nl
