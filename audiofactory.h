#pragma once

#include "audioalsainput.h"
#include "audioalsaoutput.h"
#include "blockingcircularbuffer.h"
#include "memory"

namespace Nl {

typedef std::shared_ptr<BlockingCircularBuffer<char>> AudioBuffer_t;
typedef void (*audioCallbackIn)(void*, size_t size);
typedef void (*audioCallbackOut)(void*, size_t size);
typedef void (*audioCallbackInOut)(void*, void*, size_t size);


AudioBuffer_t getBuffer(unsigned int size);

std::shared_ptr<AudioAlsaInput> getDefaultInputDevice(AudioBuffer_t buffer);
std::shared_ptr<AudioAlsaInput> getInputDevice(const std::string& name, AudioBuffer_t buffer);

std::shared_ptr<AudioAlsaOutput> getDefaultOutputDevice(AudioBuffer_t buffer);
std::shared_ptr<AudioAlsaOutput> getOutputDevice(const std::string& name, AudioBuffer_t buffer);

//std::shared_ptr<NlAudio> getDefaultInputOutputDevice(AudioBuffer_t buffer);
//std::shared_ptr<NlAudio> getInputOutputDevice(const std::string& name, AudioBuffer_t buffer);

} // namespace Nl

