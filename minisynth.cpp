#include "minisynth.h"

#include "stopwatch.h"
#include "audioalsainput.h"
#include "audioalsaoutput.h"
#include "rawmididevice.h"


//------------- activate applications here
extern Nl::StopWatch sw;

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
    void miniSynthCallback(u_int8_t *in, u_int8_t *out, size_t size, const SampleSpecs &sampleSpecs __attribute__ ((unused)))
    {
        static int counter = 0;
        //StopBlockTime sft(&sw, "val" + std::to_string(counter++));
        sw.stop();
        sw.start("val" + std::to_string(counter++));

        // Nicht die ideale Lösung für die Initialisierung. Warte auf Update des Frameworks (04.08.2016)
        static bool init = false;

        if (!init)
        { 
            voiceManager = VoiceManager();                          // Vocie Manager

            // Initialization done
            init = true;
        }

        auto midiBuffer = getBufferForName("MidiBuffer");

        //---------------- Retrieve Midi Information if midi values have changed
        if (midiBuffer)
        {
            unsigned char midiByteBuffer[3];                                // MidiByteBuffer Structure: [0] - , [1] - Played Note Address, [2] - Velocity

            while (midiBuffer->availableToRead() >= 3)
            {
                midiBuffer->get(midiByteBuffer, 3);

                // printf("%02X %02X %02X\n", midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);      // MIDI Value Control Output

                // pass Midi Values over to the Voice Manager
                voiceManager.evalMidiEvents(midiByteBuffer[0], midiByteBuffer[1], static_cast<float>(midiByteBuffer[2]));
            }
        }


        for (unsigned int frameIndex = 0; frameIndex < sampleSpecs.buffersizeInFramesPerPeriode; ++frameIndex)
        {
            voiceManager.voiceLoop();                           // voice manager main loop

            for (unsigned int channelIndex = 0; channelIndex<sampleSpecs.channels; ++channelIndex)
            {
#if 1
                float outputSample = (channelIndex == 0)
                        ? voiceManager.mainOut_L
                        : voiceManager.mainOut_R;
#endif

                setSample(out, outputSample, frameIndex, channelIndex, sampleSpecs);
            }
        }
    }



    miniSynthHandle miniSynthMidiControl(const AlsaCardIdentifier &audioInCard,
                                         const AlsaCardIdentifier &audioOutCard,
                                         const AlsaCardIdentifier &midiInCard,
                                         unsigned int buffersize,
                                         unsigned int samplerate)
    {
        miniSynthHandle ret;

        ret.inBuffer = createBuffer("InputBuffer");
        ret.outBuffer = createBuffer("OutputBuffer");

        ret.audioInput = createInputDevice(audioInCard, ret.inBuffer, buffersize);
        ret.audioInput->setSamplerate(samplerate);

        ret.audioOutput = createOutputDevice(audioOutCard, ret.outBuffer, buffersize);
        ret.audioOutput->setSamplerate(samplerate);

        ret.inMidiBuffer = createBuffer("MidiBuffer");
        ret.midiInput = createRawMidiDevice(midiInCard, ret.inMidiBuffer);

        ret.audioOutput->start();
        ret.audioInput->start();
        ret.midiInput->start();

        ret.workingThreadHandle = registerInOutCallbackOnBuffer(ret.inBuffer, ret.outBuffer, miniSynthCallback);

        return ret;
    }

}   //namespace MINISYNTH
}   //namespace NL
