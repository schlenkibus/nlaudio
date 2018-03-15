#include "dsp_host_handle.h"

#include <audio/audioalsainput.h>
#include <audio/audioalsaoutput.h>
#include <midi/rawmididevice.h>

namespace Nl {
namespace DSP_HOST_HANDLE {

    dsp_host m_host;    // renamed member dsp_host to m_host (Matthias)

    /** @brief    Callback function for Sine Generator and Audio Input - testing with ReMote 61
            @param    Input Buffer
            @param    Output Buffer

            @param    buffer size
            @param    Sample Specs
    */
    void dspHostCallback(uint8_t *out, const SampleSpecs &sampleSpecs __attribute__ ((unused)), SharedUserPtr ptr)
    {
        auto midiBuffer = getBufferForName("MidiBuffer");

        //---------------- Retrieve Midi Information if midi values have changed
        if (midiBuffer)
        {
            unsigned char midiByteBuffer[3];                                // MidiByteBuffer Structure: [0] - , [1] - Played Note Address, [2] - Velocity

            while (midiBuffer->availableToRead() >= 3)
            {
                midiBuffer->get(midiByteBuffer, 3);

                // printf("%02X %02X %02X\n", midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);      // MIDI Value Control Output

#if 1
                m_host.evalMidi();
#else
                m_host.evalMidi();
#endif
            }
        }

        for (unsigned int frameIndex = 0; frameIndex < sampleSpecs.buffersizeInFramesPerPeriode; ++frameIndex)
        {
            m_host.tickMain();

            float outputSample;

            for (unsigned int channelIndex = 0; channelIndex < sampleSpecs.channels; ++channelIndex)
            {
                if (channelIndex)
                {
                     outputSample = m_host.m_mainOut_R;
                }
                else if (!channelIndex)
                {
                     outputSample = m_host.m_mainOut_L;
                }


                if (outputSample > 1.f || outputSample < -1.f)                  // Clipping
                {
                    printf("WARNING!!! C15 CLIPPING!!!\n");
                }

                setSample(out, outputSample, frameIndex, channelIndex, sampleSpecs);
            }
        }
    }






    // Matthias: added polyphony as argument
    JobHandle dspHostTCDControl(const AlsaCardIdentifier &audioOutCard,
                                const AlsaCardIdentifier &midiInCard,
                                unsigned int buffersize,
                                unsigned int samplerate,
                                unsigned int polyphony)
    {
        m_host.init(samplerate, polyphony);
        JobHandle ret;

        // No input here
        ret.inBuffer = nullptr;
        ret.audioInput = nullptr;

        ret.outBuffer = createBuffer("OutputBuffer");
        ret.audioOutput = createAlsaOutputDevice(audioOutCard, ret.outBuffer, buffersize);
        ret.audioOutput->setSamplerate(samplerate);

        ret.inMidiBuffer = createBuffer("MidiBuffer");
        ret.midiInput = createRawMidiDevice(midiInCard, ret.inMidiBuffer);

        ret.audioOutput->start();
        ret.midiInput->start();

        ret.workingThreadHandle = registerOutputCallbackOnBuffer(ret.outBuffer, dspHostCallback, nullptr);

        return ret;
    }
} // namespace DSP_HOST
} // namespace Nl
