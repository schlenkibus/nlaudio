#pragma once

#include <atomic>
#include <map>

#include "audioalsainput.h"
#include "audioalsaoutput.h"
#include "blockingcircularbuffer.h"
#include "memory"

namespace Nl {

typedef std::shared_ptr<BlockingCircularBuffer<char>> Buffer_t;
typedef std::shared_ptr<std::atomic<bool>> TerminateFlag_t;
typedef std::shared_ptr<std::thread> WorkingThread_t;

typedef std::shared_ptr<AudioAlsaInput> AudioAlsaInput_t;
typedef std::shared_ptr<AudioAlsaOutput> AudioAlsaOutput_t;

typedef void (*audioCallbackIn)(void*, size_t size);
typedef void (*audioCallbackOut)(void*, size_t size);
typedef void (*audioCallbackInOut)(void*, void*, size_t size);

struct WorkingThreadHandle_t {
	WorkingThread_t thread;
	TerminateFlag_t terminateRequest;
};

// Factory Functions

TerminateFlag_t getTerminateFlag();

Buffer_t getBuffer(const std::string& name);
Buffer_t getBuffer(unsigned int size, const std::string& name);
Buffer_t getBufferForName(const std::string& name);

void terminateWorkingThread(WorkingThreadHandle_t handle);

AudioAlsaInput_t getDefaultInputDevice(Buffer_t buffer);
AudioAlsaInput_t getInputDevice(const std::string& name, Buffer_t buffer);
AudioAlsaInput_t getInputDevice(const std::string& name, Buffer_t buffer, unsigned int buffersize);

AudioAlsaOutput_t getDefaultOutputDevice(Buffer_t buffer);
AudioAlsaOutput_t getOutputDevice(const std::string& name, Buffer_t buffer);
AudioAlsaOutput_t getOutputDevice(const std::string& name, Buffer_t buffer, unsigned int buffersize);

WorkingThread_t registerInputCallbackOnBuffer(Buffer_t inBuffer,
														   audioCallbackIn callback,
														   std::shared_ptr<std::atomic<bool>> terminateRequest);
WorkingThread_t registerOutputCallbackOnBuffer(Buffer_t outBuffer,
															audioCallbackOut callback,
															std::shared_ptr<std::atomic<bool>> terminateRequest);
WorkingThread_t registerInOutCallbackOnBuffer(Buffer_t inBuffer,
														   Buffer_t outBuffer,
														   audioCallbackInOut callback,
														   std::shared_ptr<std::atomic<bool>> terminateRequest);

auto readAudioFunction = [](Buffer_t audioBuffer,
							audioCallbackIn callback,
							std::shared_ptr<std::atomic<bool>> terminateRequest) {

	const int buffersize = audioBuffer->size() / 2;
	char *buffer = new char[buffersize];

	while(!(*terminateRequest)) {
		audioBuffer->get(buffer, buffersize);
		callback(buffer, buffersize);
	}

	delete[] buffer;
};

auto writeAudioFunction = [](Buffer_t audioBuffer,
							 audioCallbackIn callback,
							 std::shared_ptr<std::atomic<bool>> terminateRequest) {

	const int buffersize = audioBuffer->size() / 2;
	char *buffer = new char[buffersize];

	while(!(*terminateRequest)) {
		callback(buffer, buffersize);
		audioBuffer->set(buffer, buffersize);
	}

	delete[] buffer;
};

auto readWriteAudioFunction = [](Buffer_t audioInBuffer,
								 Buffer_t audioOutBuffer,
								 audioCallbackInOut callback,
								 std::shared_ptr<std::atomic<bool>> terminateRequest) {

	const int inBuffersize = audioInBuffer->size() / 2;
	const int outBuffersize = audioOutBuffer->size() / 2;

	if (inBuffersize != outBuffersize)
		std::cout << "#### Error, in and out buffer are not the same size!! " << __FILE__ << ":" << __func__ << ":" << __LINE__ << std::endl;

	char *inBuffer = new char[inBuffersize];
	char *outBuffer = new char[outBuffersize];

	while(!terminateRequest->load()) {
		audioInBuffer->get(inBuffer, inBuffersize);
		callback(inBuffer, outBuffer, inBuffersize);
		audioOutBuffer->set(outBuffer, inBuffersize);
	}

	delete[] inBuffer;
	delete[] outBuffer;
};

} // namespace Nl
