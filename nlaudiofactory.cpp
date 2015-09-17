#include "nlaudiofactory.h"
#include "blockingcircularbuffer.h"
#include "nlaudioalsainput.h"
#include "nlaudioalsaoutput.h"
#include "tools.h"

#include <iostream>

/// Buffer
Nl::AudioBuffer_t Nl::getBuffer(unsigned int size)
{
	return std::shared_ptr<BlockingCircularBuffer<char>>(new BlockingCircularBuffer<char>(size));
}


/// Input Factories
std::shared_ptr<NlAudioAlsaInput> Nl::getInputDevice(const std::string& name, Nl::AudioBuffer_t buffer)
{
	const int buffersize = 128; // In frames

	std::shared_ptr<NlAudioAlsaInput> input(new NlAudioAlsaInput(name, buffer));
	input->open();
	input->setBufferCount(2);
	input->setBuffersize(buffersize);

	return input;
}

std::shared_ptr<NlAudioAlsaInput> Nl::getDefaultInputDevice(Nl::AudioBuffer_t buffer)
{
	return getInputDevice("default", buffer);
}

/// Output Factories
std::shared_ptr<NlAudioAlsaOutput> Nl::getOutputDevice(const std::string& name, Nl::AudioBuffer_t buffer)
{
	const int buffersize = 128; // In frames

	std::shared_ptr<NlAudioAlsaOutput> output(new NlAudioAlsaOutput(name, buffer));
	output->open();
	output->setBufferCount(2);
	output->setBuffersize(buffersize);

	return output;
}

std::shared_ptr<NlAudioAlsaOutput> Nl::getDefaultOutputDevice(Nl::AudioBuffer_t buffer)
{
	return getOutputDevice("default", buffer);
}

#if 0
/// Input/Output Factories
std::shared_ptr<NlAudio> Nl::getDefaultInputOutputDevice(Nl::AudioBuffer_t buffer)
{
	NOT_IMPLEMENTED;
}

std::shared_ptr<NlAudio> Nl::getInputOutputDevice(const std::string& name)
{
	NOT_IMPLEMENTED;
}
#endif
