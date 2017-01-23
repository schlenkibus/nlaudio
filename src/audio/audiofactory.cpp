#include "audio/audiofactory.h"

#include "midi/rawmididevice.h"
#include "audio/audioalsainput.h"
#include "audio/audioalsaoutput.h"
#include "audio/audiojack.h"
#include "common/blockingcircularbuffer.h"
#include "audio/samplespecs.h"
#include "midi/rawmididevice.h"


#include <iostream>

namespace Nl {

const int DEFAULT_BUFFERSIZE = 128; /*!< Default buffer size in Frames */

std::map<std::string, SharedBufferHandle> BuffersDictionary; /*!< Dictionary, used to querry buffers by its names */

/** \ingroup Factory
 *
 * \brief Creates a terminate flag of type \ref TerminateFlag_t
 *
 * Creates a thread save terminate flag of type \ref TerminateFlag_t, which can
 * be used to tell a working thread to terminate.
 *
*/
SharedTerminateFlag createTerminateFlag()
{
	return SharedTerminateFlag(new std::atomic<bool>(false));
}

/** \ingroup Factory
 *
 * \brief Terminate a working thread
 * \param handle A Handle of type \ref WorkingThreadHandle to a working thread.
 *
 * Tells a working thread to terminate and waits until it is joined, using a \ref WorkingThreadHandle.
 *
*/
void terminateWorkingThread(WorkingThreadHandle handle)
{
    handle.terminateRequest->store(true);
    handle.thread->join();
}

/** \ingroup Factory
 *
 * \brief Create a buffer
 * \param name A buffer name.
 * \return A handle of type \ref SharedBuffer
 *
 * Create a buffer of type \ref SharedBuffer that can be found by its name using Nl::getBufferForName().
 *
*/
SharedBufferHandle createBuffer(const std::string& name)
{
	SharedBufferHandle newBuffer = SharedBufferHandle(new BlockingCircularBuffer<u_int8_t>(name));

    BuffersDictionary.insert(std::make_pair(name, newBuffer));
    return newBuffer;
}

//TODO: Make this operation O(1)! Using a hash.
/** \ingroup Factory
 *
 * \brief Get a buffer by its name
 * \param name A buffer name.
 * \return A handle of type \ref SharedBuffer
 *
 * Get a buffer of type \ref SharedBuffer that has been created using Nl::createBuffer() by its name.
 *
*/
SharedBufferHandle getBufferForName(const std::string& name)
{
    auto it = BuffersDictionary.find(name);

    if (it == BuffersDictionary.end())
        //TODO: proper errorhandling
        return nullptr;

    return it->second;
}

/** \ingroup Factory
 *
 * \brief Creates a handle to a RawMidiDevice device for a given \a card
 * \param card A device identifier
 * \param buffer The buffer
 * \return A handle of type \ref RawMidiDevice_t
 *
 * Factory function which creates a handle of type \ref RawMidiDevice_t to the given RawMidiDevice.\n
 * The device is automatically opened.\n
 *
*/
SharedRawMidiDeviceHandle createRawMidiDevice(const AlsaCardIdentifier &card, SharedBufferHandle buffer)
{
	SharedRawMidiDeviceHandle midi(new RawMidiDevice(card, buffer));
    midi->open();
    return midi;
}

/** \ingroup Factory
 *
 * \brief Creates a handle to the input device for a given \a card
 * \param card A device identifier
 * \param buffer The buffer
 * \param buffersize Buffersize in frames.
 * \return A handle of type \ref SharedAudioHandle
 *
 * Factory function which creates a handle of type \ref SharedAudioHandle to the given input device.\n
 * ???? Buffercount is set to 2. (See AudioAlsaOutput::setBufferCount())\n
 * The device is automatically opened.\n
 *
*/
SharedAudioHandle createJackInputDevice(const AlsaCardIdentifier& card, SharedBufferHandle buffer, unsigned int buffersize)
{
	SharedAudioHandle input(new AudioJack(card, buffer, true));
	input->open();
	//input->setBufferCount(2);
	// We want buffersize to be the latency defining parameter. Therefore we have to multiply with buffercount
	input->setBuffersize(buffersize*input->getBufferCount());

	return input;
}

/** \ingroup Factory
 *
 * \brief Creates a handle to the output device for a given \a card
 * \param card A device identifier
 * \param buffer The buffer
 * \param buffersize Buffersize in frames.
 * \return A handle of type \ref SharedAudioHandle
 *
 * Factory function which creates a handle of type \ref SharedAudioHandle to the given output device.\n
 * ???? Buffercount is set to 2. (See AudioAlsaOutput::setBufferCount())\n
 * The device is automatically opened.\n
 *
*/
SharedAudioHandle createJackOutputDevice(const AlsaCardIdentifier& card, SharedBufferHandle buffer, unsigned int buffersize)
{
	SharedAudioHandle output(new AudioJack(card, buffer, false));
	output->open();
	//input->setBufferCount(2);
	// We want buffersize to be the latency defining parameter. Therefore we have to multiply with buffercount
	output->setBuffersize(buffersize*output->getBufferCount());

	return output;
}

/** \ingroup Factory
 *
 * \brief Creates a handle to the input device for a given \a card
 * \param card A device identifier
 * \param buffer The buffer
 * \param buffersize Buffersize in frames.
 * \return A handle of type \ref SharedAudioHandle
 *
 * Factory function which creates a handle of type \ref SharedAudioHandle to the given input device.\n
 * Buffercount is set to 2. (See AudioAlsaOutput::setBufferCount())\n
 * The device is automatically opened.\n
 *
*/
SharedAudioHandle createAlsaInputDevice(const AlsaCardIdentifier& card, SharedBufferHandle buffer, unsigned int buffersize)
{
	SharedAudioHandle input(new AudioAlsaInput(card, buffer));
    input->open();
    input->setBufferCount(2);
	// We want buffersize to be the latency defining parameter. Therefore we have to multiply with buffercount
	input->setBuffersize(buffersize*input->getBufferCount());

    return input;
}

/** \ingroup Factory
 *
 * \brief Creates a handle to the input device for a given \a card
 * \param card A device identifier
 * \param buffer The buffer
 * \param buffersize Buffersize in frames.
 * \return A handle of type \ref SharedAudioHandle
 *
 * Factory function which creates a handle of type \ref SharedAudioHandle to the given input device.\n
 * Buffercount is set to 2. (See AudioAlsaOutput::setBufferCount())\n
 * The device is automatically opened.\n
 *
*/
SharedAudioHandle createAlsaInputDevice(const AlsaCardIdentifier &card, SharedBufferHandle buffer)
{
	return createAlsaInputDevice(card, buffer, DEFAULT_BUFFERSIZE);
}

/** \ingroup Factory
 *
 * \brief Creates a handle to the default input device
 * \param buffer The buffer
 * \return A handle of type \ref AudioAlsaOutput_t
 *
 * Factory function which creates a handle of type \ref SharedAudioHandle to the default input device.\n
 * Buffercount is set to 2. (See AudioAlsaOutput::setBufferCount())\n
 * Buffersize is set to \ref Nl::DEFAULT_BUFFERSIZE.\n
 * The device is automatically opened.\n
 *
*/
//TODO: Fix me, card is now a class, so passing "default" does not work
//SharedAudioAlsaInputHandle createDefaultInputDevice(SharedBufferHandle buffer)
//{
//	return createInputDevice(card, buffer);
//}

/** \ingroup Factory
 *
 * \brief Creates a handle to the output device for a given \a card
 * \param card A device identifier
 * \param buffer The buffer
 * \param buffersize Buffersize in frames.
 * \return A handle of type \ref SharedAudioHandle
 *
 * Factory function which creates a handle of type \ref SharedAudioHandle to the given output device.\n
 * Buffercount is set to 2. (See AudioAlsaOutput::setBufferCount())\n
 * The device is automatically opened.\n
 *
*/
SharedAudioHandle createAlsaOutputDevice(const AlsaCardIdentifier &card, SharedBufferHandle buffer, unsigned int buffersize)
{
	SharedAudioHandle output(new AudioAlsaOutput(card, buffer));
    output->open();
    output->setBufferCount(2);
	// We want buffersize to be the latency defining parameter. Therefore we have to multiply with buffercount
	output->setBuffersize(buffersize*output->getBufferCount());

    return output;
}

/** \ingroup Factory
 *
 * \brief Creates a handle to the output device for a given \a card
 * \param card A device identifier
 * \param buffer The buffer
 * \return A handle of type \ref AudioAlsaOutput_t
 *
 * Factory function which creates a handle of type \ref AudioAlsaOutput_t to the given output device.\n
 * Buffercount is set to 2. (See AudioAlsaOutput::setBufferCount())\n
 * Buffersize is set to \ref Nl::DEFAULT_BUFFERSIZE \n
 * The device is automatically opened.\n
 *
*/
SharedAudioHandle createAlsaOutputDevice(const AlsaCardIdentifier &card, SharedBufferHandle buffer)
{
	return createAlsaOutputDevice(card, buffer, DEFAULT_BUFFERSIZE);
}

/** \ingroup Factory
 *
 * \brief Creates a handle to the default output device
 * \param buffer The buffer
 * \return A handle of type \ref AudioAlsaOutput_t
 *
 * Factory function which creates a handle of type \ref AudioAlsaOutput_t to the default output device.\n
 * Buffercount is set to 2. (See AudioAlsaOutput::setBufferCount())\n
 * Buffersize is set to \ref Nl::DEFAULT_BUFFERSIZE \n
 * The device is automatically opened.\n
 *
*/
//TODO: Fixme. "default does not work anymore, dev id is a classtype now"
//SharedAudioAlsaOutputHandle createDefaultOutputDevice(SharedBufferHandle buffer)
//{
//   return createOutputDevice("default", buffer);
//}

/** \ingroup Factory
 *
 * \brief Registers a callback on a \ref SharedBuffer for Output operations
 * \param inBuffer The input buffer
 * \param callback A callback function of type \ref audioCallbackIn
 * \return A handle of type \ref WorkingThreadHandle, which can be used to start/stop the working thread.
 *
 * Factory function which creates a reading thread to perform the blocking read
 * operations on the buffer. The \a callback is automatically called, when the buffer (\a inBuffer)
 * is ready to be processed.
 *
*/
WorkingThreadHandle registerInputCallbackOnBuffer(SharedBufferHandle inBuffer,
													AudioCallbackIn callback,
													SharedUserPtr ptr)
{
	WorkingThreadHandle handle;
	handle.terminateRequest = createTerminateFlag();
    handle.thread = std::shared_ptr<std::thread>(new std::thread(readAudioFunction,
                                                                 inBuffer,
                                                                 callback,
																 handle.terminateRequest,
																 ptr));
    return handle;
}

/** \ingroup Factory
 *
 * \brief Registers a callback on a \ref SharedBuffer for Output operations
 * \param outBuffer The output buffer
 * \param callback A callback function of type \ref audioCallbackOut
 * \return A handle of type \ref WorkingThreadHandle, which can be used to start/stop the working thread.
 *
 * Factory function which creates a writing thread to perform the blocking write
 * operations on the buffer. The \a callback is automatically called, when the buffer (\a outBuffer)
 * is ready to be processed.
 *
*/
WorkingThreadHandle registerOutputCallbackOnBuffer(SharedBufferHandle outBuffer,
												   AudioCallbackOut callback,
												   SharedUserPtr ptr)
{
	WorkingThreadHandle handle;
	handle.terminateRequest = createTerminateFlag();
    handle.thread = std::shared_ptr<std::thread>(new std::thread(writeAudioFunction,
                                                                 outBuffer,
                                                                 callback,
																 handle.terminateRequest,
																 ptr));
    return handle;
}

/** \ingroup Factory
 *
 * \brief Registers a callback on a \ref SharedBuffer for Input/Output operations
 * \param inBuffer The input buffer
 * \param outBuffer The output buffer
 * \param callback A callback function of type \ref audioCallbackInOut
 * \return A handle of type \ref WorkingThreadHandle, which can be used to start/stop the working thread.
 *
 * Factory function which creates a reading and a writing thread to perform the blocking read/write
 * operations on the buffer. The \a callback is automatically called, when the buffers (\a inBuffer, \a outBuffer)
 * are ready to be processed.
 *
*/
WorkingThreadHandle registerInOutCallbackOnBuffer(SharedBufferHandle inBuffer,
													SharedBufferHandle outBuffer,
													AudioCallbackInOut callback,
													SharedUserPtr ptr)
{
	WorkingThreadHandle handle;
	handle.terminateRequest = createTerminateFlag();
    handle.thread = std::shared_ptr<std::thread>(new std::thread(readWriteAudioFunction,
                                                                 inBuffer,
                                                                 outBuffer,
                                                                 callback,
																 handle.terminateRequest,
																 ptr));
    return handle;
}

/** \ingroup Factory
 *
 * \brief Registers a callback on a \ref SharedBuffer for Input/Output operations
 * \param inBuffer The input buffer
 * \param outBuffer The output buffer
 * \param callback A callback function of type \ref audioCallbackInOut
 * \return A handle of type \ref WorkingThreadHandle, which can be used to start/stop the working thread.
 *
 * Factory function which creates a reading and a writing thread to perform the blocking read/write
 * operations on the buffer. The \a callback is automatically called, when the buffers (\a inBuffer, \a outBuffer)
 * are ready to be processed.
 *
*/
WorkingThreadHandle registerAutoDrainOnBuffer(SharedBufferHandle inBuffer)
{
	WorkingThreadHandle handle;
	handle.terminateRequest = createTerminateFlag();
	handle.thread = std::shared_ptr<std::thread>(new std::thread(drainAudioFunction,
																 inBuffer,
																 handle.terminateRequest));
	return handle;
}


} // namespace Nl
