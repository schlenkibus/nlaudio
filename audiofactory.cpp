#include "audiofactory.h"
#include "blockingcircularbuffer.h"
#include "audioalsainput.h"
#include "audioalsaoutput.h"
#include "tools.h"

#include <iostream>

namespace Nl {

const int DEFAULT_BUFFERSIZE = 128;
std::map<std::string, Buffer_t> BuffersDictionary;

/// Termination
TerminateFlag_t createTerminateFlag()
{
	return TerminateFlag_t(new std::atomic<bool>(false));
}

void terminateWorkingThread(WorkingThreadHandle_t handle)
{
	handle.terminateRequest->store(true);
	handle.thread->join();
}

Buffer_t createBuffer(const std::string& name)
{
	Buffer_t newBuffer = Buffer_t(new BlockingCircularBuffer<u_int8_t>(name));

	BuffersDictionary.insert(std::make_pair(name, newBuffer));
	return newBuffer;
}

Buffer_t getBufferForName(const std::string& name)
{
	auto it = BuffersDictionary.find(name);

	if (it == BuffersDictionary.end())
		//TODO: proper errorhandling
		return nullptr;

	return it->second;
}

/// Input Factories
AudioAlsaInput_t createInputDevice(const std::string& name, Buffer_t buffer, unsigned int buffersize)
{
	std::shared_ptr<AudioAlsaInput> input(new AudioAlsaInput(name, buffer));
	input->open();
	input->setBufferCount(2);
	input->setBuffersize(buffersize);

	return input;
}

AudioAlsaInput_t createInputDevice(const std::string& name, Buffer_t buffer)
{
	return createInputDevice(name, buffer, DEFAULT_BUFFERSIZE);
}

AudioAlsaInput_t createDefaultInputDevice(Buffer_t buffer)
{
	return createInputDevice("default", buffer);
}

/// Output Factories
AudioAlsaOutput_t createOutputDevice(const std::string& name, Buffer_t buffer, unsigned int buffersize)
{
	std::shared_ptr<AudioAlsaOutput> output(new AudioAlsaOutput(name, buffer));
	output->open();
	output->setBufferCount(2);
	output->setBuffersize(buffersize);

	return output;
}

AudioAlsaOutput_t createOutputDevice(const std::string& name, Buffer_t buffer)
{
	return createOutputDevice(name, buffer, DEFAULT_BUFFERSIZE);
}

AudioAlsaOutput_t createDefaultOutputDevice(Buffer_t buffer)
{
	return createOutputDevice("default", buffer);
}

/// Callbacks
WorkingThread_t registerInputCallbackOnBuffer(Buffer_t inBuffer,
														   audioCallbackIn callback,
														   std::shared_ptr<std::atomic<bool>> terminateRequest)
{
	return std::shared_ptr<std::thread>(new std::thread(readAudioFunction,
														inBuffer,
														callback,
														terminateRequest));
}

WorkingThread_t registerOutputCallbackOnBuffer(Buffer_t outBuffer,
															audioCallbackOut callback,
															std::shared_ptr<std::atomic<bool>> terminateRequest)
{
	return std::shared_ptr<std::thread>(new std::thread(writeAudioFunction,
														outBuffer,
														callback,
														terminateRequest));
}

WorkingThread_t registerInOutCallbackOnBuffer(Buffer_t inBuffer,
														   Buffer_t outBuffer,
														   audioCallbackInOut callback,
														   std::shared_ptr<std::atomic<bool>> terminateRequest)
{
	return std::shared_ptr<std::thread>(new std::thread(readWriteAudioFunction,
														inBuffer,
														outBuffer,
														callback,
														terminateRequest));
}

} // namespace Nl
