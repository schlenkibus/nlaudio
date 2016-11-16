#pragma once

/** @file       minisynth.cpp / minisynth.h
    @date       2016-04-02
    @brief      Class for synth related functions
    @author     Anton Schmied [2016-02-04]
**/

#define SYNTH           // turn synth on and off by define

#include "audiofactory.h"
#include "voicemanager.h"

namespace Nl {
namespace MINISYNTH {


    struct miniSynthHandle {
        WorkingThreadHandle workingThreadHandle;
        SharedAudioAlsaInputHandle audioInput;
        SharedAudioAlsaOutputHandle audioOutput;
        SharedRawMidiDeviceHandle midiInput;
        SharedRawMidiDeviceHandle midiOutput;
        SharedBufferHandle inBuffer;
        SharedBufferHandle outBuffer;
        SharedBufferHandle inMidiBuffer;
    };

    miniSynthHandle miniSynthMidiControl(const AlsaCardIdentifier &audioInCard,
                                         const AlsaCardIdentifier &audioOutCard,
                                         const AlsaCardIdentifier &midiIn,
                                         unsigned int buffersize,
                                         unsigned int samplerate);
}   //namespace MINISYNTH
}   //namespace NL
