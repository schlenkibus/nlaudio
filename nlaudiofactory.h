#pragma once

#include "nlaudioalsainput.h"
#include "nlaudioalsaoutput.h"
#include "blockingcircularbuffer.h"
#include "memory"

namespace Nl {


typedef std::shared_ptr<BlockingCircularBuffer<char>> AudioBuffer_t;
typedef void (*audioCallbackIn)(void*, size_t size);
typedef void (*audioCallbackOut)(void*, size_t size);
typedef void (*audioCallbackInOut)(void*, void*, size_t size);


AudioBuffer_t getBuffer(unsigned int size);

std::shared_ptr<NlAudioAlsaInput> getDefaultInputDevice(AudioBuffer_t buffer);
std::shared_ptr<NlAudioAlsaInput> getInputDevice(const std::string& name, AudioBuffer_t buffer);

std::shared_ptr<NlAudioAlsaOutput> getDefaultOutputDevice(AudioBuffer_t buffer);
std::shared_ptr<NlAudioAlsaOutput> getOutputDevice(const std::string& name, AudioBuffer_t buffer);

//std::shared_ptr<NlAudio> getDefaultInputOutputDevice(AudioBuffer_t buffer);
//std::shared_ptr<NlAudio> getInputOutputDevice(const std::string& name, AudioBuffer_t buffer);


}

