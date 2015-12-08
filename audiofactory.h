#pragma once

/** \defgroup Factory
 *
 * \brief A collection of factory functions.
 *
 * This file contains factory functions, to simplify the use of the framework.
 *
*/

#include <atomic>
#include <map>

#include "audioalsainput.h"
#include "audioalsaoutput.h"

#include "rawmididevice.h"

#include "blockingcircularbuffer.h"
#include "memory"

namespace Nl {

/*! A shared handle to a \ref BlockingCircularBuffer<uint8_t> */
typedef std::shared_ptr<BlockingCircularBuffer<uint8_t>> SharedBufferHandle;

/*! A shared handle to a \ref std::atomic<bool> */
typedef std::shared_ptr<std::atomic<bool>> SharedTerminateFlag;

/*! A shared handle to a \ref std::thread */
typedef std::shared_ptr<std::thread> SharedThreadHandle;

/*! A shared handle to a \ref AudioAlsaInput */
typedef std::shared_ptr<AudioAlsaInput> SharedAudioAlsaInputHandle;

/*! A shared handle to a \ref AudioAlsaOutput */
typedef std::shared_ptr<AudioAlsaOutput> SharedAudioAlsaOutputHandle;

/*! A shared handle to a \ref RawMidiDevice */
typedef std::shared_ptr<RawMidiDevice> SharedRawMidiDeviceHandle;

typedef void (*AudioCallbackIn)(u_int8_t*, size_t size, const SampleSpecs &sampleSpecs);
typedef void (*AudioCallbackOut)(u_int8_t*, size_t size, const SampleSpecs &sampleSpecs);
typedef void (*AudioCallbackInOut)(u_int8_t*, uint8_t*, size_t size, const SampleSpecs &sampleSpecs);

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

// Factory Functions
SharedRawMidiDeviceHandle createRawMidiDevice(const AlsaCardIdentifier &card, SharedBufferHandle buffer);

SharedTerminateFlag createTerminateFlag();

SharedBufferHandle createBuffer(const std::string& name);
SharedBufferHandle getBufferForName(const std::string& name);

void terminateWorkingThread(WorkingThreadHandle handle);

SharedAudioAlsaInputHandle createDefaultInputDevice(SharedBufferHandle buffer);
SharedAudioAlsaInputHandle createInputDevice(const AlsaCardIdentifier &card, SharedBufferHandle buffer);
SharedAudioAlsaInputHandle createInputDevice(const AlsaCardIdentifier &card, SharedBufferHandle buffer, unsigned int buffersize);

SharedAudioAlsaOutputHandle createDefaultOutputDevice(SharedBufferHandle buffer);
SharedAudioAlsaOutputHandle createOutputDevice(const AlsaCardIdentifier &card, SharedBufferHandle buffer);
SharedAudioAlsaOutputHandle createOutputDevice(const AlsaCardIdentifier &card, SharedBufferHandle buffer, unsigned int buffersize);

WorkingThreadHandle registerInputCallbackOnBuffer(SharedBufferHandle inBuffer,
											  AudioCallbackIn callback);
WorkingThreadHandle registerOutputCallbackOnBuffer(SharedBufferHandle outBuffer,
											   AudioCallbackOut callback);
WorkingThreadHandle registerInOutCallbackOnBuffer(SharedBufferHandle inBuffer,
											  SharedBufferHandle outBuffer,
											  AudioCallbackInOut callback);

// Thread function, that handles blocking io calls on the buffers
auto readAudioFunction = [](SharedBufferHandle audioBuffer,
AudioCallbackIn callback,
std::shared_ptr<std::atomic<bool>> terminateRequest) {

	SampleSpecs sampleSpecs = audioBuffer->sampleSpecs();

    const int buffersize = sampleSpecs.buffersizeInBytesPerPeriode;
    u_int8_t *buffer = new u_int8_t[buffersize];

    while(!terminateRequest->load()) {
        audioBuffer->get(buffer, buffersize);
        callback(buffer, buffersize, sampleSpecs);
    }

    delete[] buffer;
};

// Thread function, that handles blocking io calls on the buffers
auto writeAudioFunction = [](SharedBufferHandle audioBuffer,
AudioCallbackIn callback,
std::shared_ptr<std::atomic<bool>> terminateRequest) {

	SampleSpecs sampleSpecs = audioBuffer->sampleSpecs();

    const int buffersize = sampleSpecs.buffersizeInBytesPerPeriode;
    u_int8_t *buffer = new u_int8_t[buffersize];

    memset(buffer, 0, sampleSpecs.buffersizeInBytesPerPeriode);

    while(!terminateRequest->load()) {
        callback(buffer, buffersize, sampleSpecs);
        audioBuffer->set(buffer, buffersize);
    }

    delete[] buffer;
};

// Thread function, that handles blocking io calls on the buffers
auto readWriteAudioFunction = [](SharedBufferHandle audioInBuffer,
SharedBufferHandle audioOutBuffer,
AudioCallbackInOut callback,
std::shared_ptr<std::atomic<bool>> terminateRequest) {

	SampleSpecs sampleSpecsIn = audioInBuffer->sampleSpecs();
	SampleSpecs sampleSpecsOut = audioOutBuffer->sampleSpecs();

    const int inBuffersize = sampleSpecsIn.buffersizeInBytesPerPeriode;
    const int outBuffersize = sampleSpecsOut.buffersizeInBytesPerPeriode;

    if (inBuffersize != outBuffersize)
        std::cout << "#### Error, in and out buffer are not the same size!! " << __FILE__ << ":" << __func__ << ":" << __LINE__ << std::endl;

    u_int8_t *inBuffer = new u_int8_t[inBuffersize];
    u_int8_t *outBuffer = new u_int8_t[outBuffersize];

    while(!terminateRequest->load()) {
        audioInBuffer->get(inBuffer, inBuffersize);
        callback(inBuffer, outBuffer, inBuffersize, sampleSpecsIn);
        audioOutBuffer->set(outBuffer, inBuffersize);
    }

    delete[] inBuffer;
    delete[] outBuffer;
};

} // namespace Nl
