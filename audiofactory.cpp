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
TerminateFlag_t getTerminateFlag()
{
	return TerminateFlag_t(false);
}

void terminateWorkingThread(WorkingThreadHandle_t handle)
{
	handle.terminateRequest->store(true);
	handle.thread->join();
}

/// Buffer
Buffer_t getBuffer(const std::string& name)
{
	// This size is in Bytes, while the buffersize below is in frames!
	//TODO: If i set buffersize to 0 or so, it does not work, even if resized afterwards. Don't know why, this needs
	//		further investigation
	return getBuffer(2*DEFAULT_BUFFERSIZE, name);
}

Buffer_t getBuffer(unsigned int size, const std::string& name)
{
	// This size is in Bytes, while the buffersize below is in frames!
	Buffer_t newBuffer = Buffer_t(new BlockingCircularBuffer<char>(size, name));

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
AudioAlsaInput_t getInputDevice(const std::string& name, Buffer_t buffer, unsigned int buffersize)
{
	std::shared_ptr<AudioAlsaInput> input(new AudioAlsaInput(name, buffer));
	input->open();
	input->setBufferCount(2);
	input->setBuffersize(buffersize);

	return input;
}

AudioAlsaInput_t getInputDevice(const std::string& name, Buffer_t buffer)
{
	return getInputDevice(name, buffer, DEFAULT_BUFFERSIZE);
}

AudioAlsaInput_t getDefaultInputDevice(Buffer_t buffer)
{
	return getInputDevice("default", buffer);
}

/// Output Factories
AudioAlsaOutput_t getOutputDevice(const std::string& name, Buffer_t buffer, unsigned int buffersize)
{
	std::shared_ptr<AudioAlsaOutput> output(new AudioAlsaOutput(name, buffer));
	output->open();
	output->setBufferCount(2);
	output->setBuffersize(buffersize);

	return output;
}

AudioAlsaOutput_t getOutputDevice(const std::string& name, Buffer_t buffer)
{
	return getOutputDevice(name, buffer, DEFAULT_BUFFERSIZE);
}

AudioAlsaOutput_t getDefaultOutputDevice(Buffer_t buffer)
{
	return getOutputDevice("default", buffer);
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
