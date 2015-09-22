#pragma once

#include <atomic>
#include <map>

#include "audioalsainput.h"
#include "audioalsaoutput.h"
#include "blockingcircularbuffer.h"
#include "memory"

namespace Nl {

typedef std::shared_ptr<BlockingCircularBuffer<u_int8_t>> Buffer_t;
typedef std::shared_ptr<std::atomic<bool>> TerminateFlag_t;
typedef std::shared_ptr<std::thread> WorkingThread_t;

typedef std::shared_ptr<AudioAlsaInput> AudioAlsaInput_t;
typedef std::shared_ptr<AudioAlsaOutput> AudioAlsaOutput_t;

typedef void (*audioCallbackIn)(u_int8_t*, size_t size, const SampleSpecs_t &sampleSpecs);
typedef void (*audioCallbackOut)(u_int8_t*, size_t size, const SampleSpecs_t &sampleSpecs);
typedef void (*audioCallbackInOut)(u_int8_t*, uint8_t*, size_t size, const SampleSpecs_t &sampleSpecs);

struct WorkingThreadHandle_t {
	WorkingThread_t thread;
	TerminateFlag_t terminateRequest;
};

// Factory Functions

TerminateFlag_t createTerminateFlag();

Buffer_t createBuffer(const std::string& name);
Buffer_t createBuffer(unsigned int size, const std::string& name);
Buffer_t getBufferForName(const std::string& name);

void terminateWorkingThread(WorkingThreadHandle_t handle);

AudioAlsaInput_t createDefaultInputDevice(Buffer_t buffer);
AudioAlsaInput_t createInputDevice(const std::string& name, Buffer_t buffer);
AudioAlsaInput_t createInputDevice(const std::string& name, Buffer_t buffer, unsigned int buffersize);

AudioAlsaOutput_t createDefaultOutputDevice(Buffer_t buffer);
AudioAlsaOutput_t createOutputDevice(const std::string& name, Buffer_t buffer);
AudioAlsaOutput_t createOutputDevice(const std::string& name, Buffer_t buffer, unsigned int buffersize);

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

	SampleSpecs_t sampleSpecs = audioBuffer->sampleSpecs();

	const int buffersize = sampleSpecs.buffersizeInBytesPerPeriode;
	u_int8_t *buffer = new u_int8_t[buffersize];

	while(!terminateRequest->load()) {
		audioBuffer->get(buffer, buffersize);
		callback(buffer, buffersize, sampleSpecs);
	}

	delete[] buffer;
};

auto writeAudioFunction = [](Buffer_t audioBuffer,
							 audioCallbackIn callback,
							 std::shared_ptr<std::atomic<bool>> terminateRequest) {

	SampleSpecs_t sampleSpecs = audioBuffer->sampleSpecs();

	const int buffersize = sampleSpecs.buffersizeInBytesPerPeriode;
	u_int8_t *buffer = new u_int8_t[buffersize];

	memset(buffer, 0, sampleSpecs.buffersizeInBytesPerPeriode);

	while(!terminateRequest->load()) {
		//callback(buffer, buffersize, sampleSpecs);
		audioBuffer->set(buffer, buffersize);
		std::cout << "Just Wrote to Buffer" << std::endl;
	}

	delete[] buffer;
};

auto readWriteAudioFunction = [](Buffer_t audioInBuffer,
								 Buffer_t audioOutBuffer,
								 audioCallbackInOut callback,
								 std::shared_ptr<std::atomic<bool>> terminateRequest) {

	SampleSpecs_t sampleSpecsIn = audioInBuffer->sampleSpecs();
	SampleSpecs_t sampleSpecsOut = audioOutBuffer->sampleSpecs();

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
