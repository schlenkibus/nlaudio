#pragma once

/** @file       dsp_host_handle.cpp / dsp_host_handle.h
    @date       2018-03-07
    @brief      replacing the voice_manager as new main container
    @author     Matthias Seeber & Anton Schmied [2018-03-07]
**/

#include <audio/audiofactory.h>
#include "dsp_host.h"

namespace Nl {
namespace DSP_HOST_HANDLE {

    JobHandle dspHostTCDControl(const AlsaCardIdentifier &audioOutCard,
                                const AlsaCardIdentifier &midiIn,
                                unsigned int buffersize,
                                unsigned int samplerate);

}   //namespace DSP_HOST
}   //namespace NL
