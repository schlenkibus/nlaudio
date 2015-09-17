#include "audiofactory.h"
#include "blockingcircularbuffer.h"
#include "audioalsainput.h"
#include "audioalsaoutput.h"
#include "tools.h"

#include <iostream>

namespace Nl {

/// Buffer
AudioBuffer_t getBuffer(unsigned int size)
{
	return std::shared_ptr<BlockingCircularBuffer<char>>(new BlockingCircularBuffer<char>(size));
}


/// Input Factories
std::shared_ptr<AudioAlsaInput> getInputDevice(const std::string& name, AudioBuffer_t buffer)
{
	const int buffersize = 128; // In frames

	std::shared_ptr<AudioAlsaInput> input(new AudioAlsaInput(name, buffer));
	input->open();
	input->setBufferCount(2);
	input->setBuffersize(buffersize);

	return input;
}

std::shared_ptr<AudioAlsaInput> getDefaultInputDevice(AudioBuffer_t buffer)
{
	return getInputDevice("default", buffer);
}

/// Output Factories
std::shared_ptr<AudioAlsaOutput> getOutputDevice(const std::string& name, AudioBuffer_t buffer)
{
	const int buffersize = 128; // In frames

	std::shared_ptr<AudioAlsaOutput> output(new AudioAlsaOutput(name, buffer));
	output->open();
	output->setBufferCount(2);
	output->setBuffersize(buffersize);

	return output;
}

std::shared_ptr<AudioAlsaOutput> getDefaultOutputDevice(AudioBuffer_t buffer)
{
	return getOutputDevice("default", buffer);
}

#if 0
/// Input/Output Factories
std::shared_ptr<NlAudio> getDefaultInputOutputDevice(AudioBuffer_t buffer)
{
	NOT_IMPLEMENTED;
}

std::shared_ptr<NlAudio> getInputOutputDevice(const std::string& name)
{
	NOT_IMPLEMENTED;
}
#endif

} // namespace Nl
