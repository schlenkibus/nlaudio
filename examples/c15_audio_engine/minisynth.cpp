#include "minisynth.h"

#include <common/stopwatch.h>
#include "cpu_stopwatch.h"

#include <audio/audioalsainput.h>
#include <audio/audioalsaoutput.h>
#include <midi/rawmididevice.h>

namespace Nl {
namespace MINISYNTH {

//--------------- Objects
VoiceManager voiceManager;

/** @brief    Callback function for Sine Generator and Audio Input - testing with ReMote 61
        @param    Input Buffer
        @param    Output Buffer

        @param    buffer size
        @param    Sample Specs
    */
void miniSynthCallback(uint8_t *out, const SampleSpecs &sampleSpecs __attribute__ ((unused)), SharedUserPtr ptr)
{
    auto midiBuffer = getBufferForName("MidiBuffer");

    //---------------- Retrieve Midi Information if midi values have changed
    if (midiBuffer)
    {
        unsigned char midiByteBuffer[3];                                // MidiByteBuffer Structure: [0] - , [1] - Played Note Address, [2] - Velocity

        while (midiBuffer->availableToRead() >= 3)
        {
            midiBuffer->get(midiByteBuffer, 3);

            //                printf("%02X %02X %02X\n", midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);      // MIDI Value Control Output

            // pass Midi Values over to the Voice Manager
#if INPUT_MIDI == 1
            voiceManager.evalMidiEvents(midiByteBuffer[0], midiByteBuffer[1], static_cast<float>(midiByteBuffer[2]));
#endif
#ifdef INPUT_TCD
            voiceManager.evalTCDEvents(midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);
#endif
        }
    }

    for (unsigned int frameIndex = 0; frameIndex < sampleSpecs.buffersizeInFramesPerPeriode; ++frameIndex)
    {
        voiceManager.voiceLoop();                           // voice manager main loop

        float outputSample;

        for (unsigned int channelIndex = 0; channelIndex < sampleSpecs.channels; ++channelIndex)
        {
            if (channelIndex)
            {
                outputSample = voiceManager.mainOut_R;
            }
            else if (!channelIndex)
            {
                outputSample = voiceManager.mainOut_L;
            }


            if (outputSample > 1.f || outputSample < -1.f)                  // Clipping
            {
                printf("WARNING!!! C15 CLIPPING!!!\n");
            }

            setSample(out, outputSample, frameIndex, channelIndex, sampleSpecs);
        }
    }
}



JobHandle miniSynthMidiControl(const AlsaAudioCardIdentifier &audioOutCard,
                               const AlsaMidiCardIdentifier &midiInCard,
                               unsigned int buffersize,
                               unsigned int samplerate)
{
    JobHandle ret;

    // No input here!
    ret.inBuffer = nullptr;
    ret.audioInput = nullptr;

    ret.outBuffer = createBuffer("OutputBuffer");
    ret.audioOutput = createAlsaOutputDevice(audioOutCard, ret.outBuffer, buffersize);
    ret.audioOutput->setSamplerate(samplerate);

    ret.inMidiBuffer = createBuffer("MidiBuffer");
    ret.midiInput = createRawMidiDevice(midiInCard, ret.inMidiBuffer);

    ret.audioOutput->start();
    ret.midiInput->start();

    ret.workingThreadHandle = registerOutputCallbackOnBuffer(ret.outBuffer, miniSynthCallback, nullptr);

    return ret;
}

}   //namespace MINISYNTH
}   //namespace NL
