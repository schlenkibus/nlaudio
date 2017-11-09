#pragma once

/** \defgroup Factory
 *
 * \brief A collection of factory functions.
 *
 * This file contains factory functions, to simplify the use of the framework.
 *
*/

#include <map>
#include <memory>
#include <thread>

#include "common/blockingcircularbuffer.h"
#include "audio/audioalsainput.h"
#include "audio/audioalsaoutput.h"
#include "midi/rawmididevice.h"
#include "audio/audioalsaexception.h"

#include "audio/audiojack.h"

namespace Nl {

/**
 * \brief The User Pointer struct
 *
 * A handle that is passed to the audio callbacks for userdata
 *
 */
struct UserPtr {
    UserPtr() :
        info("unused"),
        ptr(nullptr) {}
    UserPtr(const std::string info, void* ptr) :
        info(info),
        ptr(ptr) {}
    std::string info; /**< A description of the type passed */
    void *ptr;	/**< The actual user pointer */
};

typedef std::shared_ptr<UserPtr> SharedUserPtr;

class AudioAlsaInput;
class AudioAlsaOutput;
class RawMidiDevice;

/*! A shared handle to a \ref std::atomic<bool> */
typedef std::shared_ptr<std::atomic<bool>> SharedTerminateFlag;

typedef void (*AudioCallbackIn)(uint8_t*, const SampleSpecs &specs, SharedUserPtr ptr);
typedef void (*AudioCallbackOut)(uint8_t*, const SampleSpecs &specs, SharedUserPtr ptr);
typedef void (*AudioCallbackInOut)(uint8_t*, uint8_t*, const SampleSpecs &specs, SharedUserPtr ptr);

/*! A shared handle to a \ref std::thread */
typedef std::shared_ptr<std::thread> SharedThreadHandle;

/**
 * \brief The WorkingThreadHandle struct
 *
 * A handle to a working thread, as used by the callback creators:
 * - Nl::registerInOutCallbackOnBuffer()
 * - Nl::registerOutputCallbackOnBuffer()
 * - Nl::registerInputCallbackOnBuffer()
 */
struct WorkingThreadHandle {
	SharedThreadHandle thread;	/**< A thread handle */
	SharedTerminateFlag terminateRequest; /**< A terminate request handle */
};

struct JobHandle {
    WorkingThreadHandle workingThreadHandle;
    SharedAudioHandle audioInput;
    SharedAudioHandle audioOutput;
    SharedRawMidiDeviceHandle midiInput;
    SharedRawMidiDeviceHandle midiOutput;
    SharedBufferHandle inBuffer;
    SharedBufferHandle outBuffer;
    SharedBufferHandle inMidiBuffer;
};


// Factory Functions
SharedRawMidiDeviceHandle createRawMidiDevice(const AlsaCardIdentifier &card, SharedBufferHandle buffer);

SharedTerminateFlag createTerminateFlag();

SharedBufferHandle createBuffer(const std::string& name);
SharedBufferHandle getBufferForName(const std::string& name);

void terminateWorkingThread(WorkingThreadHandle handle);

SharedAudioHandle createJackInputDevice(const AlsaCardIdentifier& card, SharedBufferHandle buffer, unsigned int buffersize);
SharedAudioHandle createJackOutputDevice(const AlsaCardIdentifier& card, SharedBufferHandle buffer, unsigned int buffersize);

SharedAudioHandle createDefaultInputDevice(SharedBufferHandle buffer);
SharedAudioHandle createAlsaInputDevice(const AlsaCardIdentifier &card, SharedBufferHandle buffer);
SharedAudioHandle createAlsaInputDevice(const AlsaCardIdentifier &card, SharedBufferHandle buffer, unsigned int buffersize);

SharedAudioHandle createDefaultOutputDevice(SharedBufferHandle buffer);
SharedAudioHandle createAlsaOutputDevice(const AlsaCardIdentifier &card, SharedBufferHandle buffer);
SharedAudioHandle createAlsaOutputDevice(const AlsaCardIdentifier &card, SharedBufferHandle buffer, unsigned int buffersize);

WorkingThreadHandle registerInputCallbackOnBuffer(SharedBufferHandle inBuffer,
												  AudioCallbackIn callback,
												  SharedUserPtr ptr);
WorkingThreadHandle registerOutputCallbackOnBuffer(SharedBufferHandle outBuffer,
												   AudioCallbackOut callback,
												   SharedUserPtr ptr);
WorkingThreadHandle registerInOutCallbackOnBuffer(SharedBufferHandle inBuffer,
												  SharedBufferHandle outBuffer,
												  AudioCallbackInOut callback,
												  SharedUserPtr ptr);
WorkingThreadHandle registerAutoDrainOnBuffer(SharedBufferHandle inBuffer);

// Thread function, that handles blocking io calls on the buffers
auto readAudioFunction = [](SharedBufferHandle audioBuffer,
AudioCallbackIn callback,
SharedTerminateFlag terminateRequest, SharedUserPtr ptr)
{

	SampleSpecs sampleSpecs = audioBuffer->sampleSpecs();

	const int buffersize = sampleSpecs.buffersizeInBytesPerPeriode;
	u_int8_t *buffer = new u_int8_t[buffersize];

	while(!terminateRequest->load()) {
		audioBuffer->get(buffer, buffersize);
		callback(buffer, sampleSpecs, ptr);
	}

	delete[] buffer;
};

// Thread function, that handles blocking io calls on the buffers
auto writeAudioFunction = [](SharedBufferHandle audioBuffer,
AudioCallbackIn callback,
SharedTerminateFlag terminateRequest, SharedUserPtr ptr) {

	SampleSpecs sampleSpecs = audioBuffer->sampleSpecs();

	const int buffersize = sampleSpecs.buffersizeInBytesPerPeriode;
	u_int8_t *buffer = new u_int8_t[buffersize];

	memset(buffer, 0, sampleSpecs.buffersizeInBytesPerPeriode);

	while(!terminateRequest->load()) {
		callback(buffer, sampleSpecs, ptr);
		audioBuffer->set(buffer, buffersize);
	}

	delete[] buffer;
};

// Thread function, that handles blocking io calls on the buffers
auto readWriteAudioFunction = [](SharedBufferHandle audioInBuffer,
SharedBufferHandle audioOutBuffer,
AudioCallbackInOut callback,
SharedTerminateFlag terminateRequest, SharedUserPtr ptr) {

	SampleSpecs sampleSpecsIn = audioInBuffer->sampleSpecs();
	SampleSpecs sampleSpecsOut = audioOutBuffer->sampleSpecs();

	const int inBuffersize = sampleSpecsIn.buffersizeInBytesPerPeriode;
	const int outBuffersize = sampleSpecsOut.buffersizeInBytesPerPeriode;

	u_int8_t *inBuffer = new u_int8_t[inBuffersize];
	u_int8_t *outBuffer = new u_int8_t[outBuffersize];

	try {

		if (inBuffersize != outBuffersize)
			std::cout << "#### Error, in and out buffer are not the same size!! " << __FILE__ << ":" << __func__ << ":" << __LINE__ << std::endl;


		memset(outBuffer, 0, outBuffersize);
		memset(inBuffer, 0, inBuffersize);

		audioOutBuffer->set(outBuffer, outBuffersize);

		while(!terminateRequest->load()) {
			audioInBuffer->get(inBuffer, inBuffersize);
			callback(inBuffer, outBuffer, sampleSpecsIn, ptr);
			audioOutBuffer->set(outBuffer, inBuffersize);
		}

	} catch (AudioAlsaException& e) {
		std::cout << "### Exception from " << __func__ <<  " ###" << std::endl << "  " << e.what() << std::endl;
	} catch (std::exception& e) {
		std::cout << "### Exception from " << __func__ << " ###" << std::endl << "  " << e.what() << std::endl;
	} catch(...) {
		std::cout << "### Exception from " << __func__ << " ###" << std::endl << "  default" << std::endl;
	}

	delete[] inBuffer;
	delete[] outBuffer;
};

// Thread function, that just drains the buffer for testing puposes
auto drainAudioFunction = [](SharedBufferHandle audioInBuffer,
SharedTerminateFlag terminateRequest) {

	SampleSpecs sampleSpecsIn = audioInBuffer->sampleSpecs();

	const int inBuffersize = sampleSpecsIn.buffersizeInBytesPerPeriode;

	u_int8_t *inBuffer = new u_int8_t[inBuffersize];

	while(!terminateRequest->load()) {
		audioInBuffer->get(inBuffer, inBuffersize);
	}
};

} // namespace Nl
