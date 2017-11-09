#pragma once

/** @file       minisynth.cpp / minisynth.h
    @date       2016-04-02
    @brief      Class for synth related functions
    @author     Anton Schmied [2016-02-04]
**/

#include <audio/audiofactory.h>
#include "voicemanager.h"

namespace Nl {
namespace MINISYNTH {

    JobHandle miniSynthMidiControl(const AlsaCardIdentifier &audioInCard,
                                         const AlsaCardIdentifier &audioOutCard,
                                         const AlsaCardIdentifier &midiIn,
                                         unsigned int buffersize,
                                         unsigned int samplerate);
}   //namespace MINISYNTH
}   //namespace NL
