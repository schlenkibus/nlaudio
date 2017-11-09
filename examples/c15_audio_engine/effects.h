#pragma once

/** @file       effects.cpp / effects.h
    @date       2016-04-02
    @brief      Class for testing effects
    @author     Anton Schmied [2016-02-04]
**/

#include "onepolefilters.h"
#include "biquadfilters.h"
#include "tiltfilters.h"
#include "cabinet.h"
#include "echo.h"
#include "smoother.h"

#include <audio/audiofactory.h>

namespace Nl {
namespace EFFECTS {

    struct effectsMidiControlHandle {
        WorkingThreadHandle workingThreadHandle;
        SharedAudioAlsaInputHandle audioInput;
        SharedAudioAlsaOutputHandle audioOutput;
        SharedRawMidiDeviceHandle midiInput;
        SharedRawMidiDeviceHandle midiOutput;
        SharedBufferHandle inBuffer;
        SharedBufferHandle outBuffer;
        SharedBufferHandle inMidiBuffer;
};

    effectsMidiControlHandle effectsMidiControl(const AlsaCardIdentifier &audioInCard,
                                         const AlsaCardIdentifier &audioOutCard,
                                         const AlsaCardIdentifier &midiIn,
                                         unsigned int buffersize,
                                         unsigned int samplerate);
}   //namespace EFFECTS
}   //namespace NL

