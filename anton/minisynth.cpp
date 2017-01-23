#include "minisynth.h"

#define SYNTH           // turn synth on and off by define
//#define PASCALSW
//#define CPU_STOPWATCH

#include "common/stopwatch.h"
#include "cpu_stopwatch.h"

#include "audio/audioalsainput.h"
#include "audio/audioalsaoutput.h"
#include "midi/rawmididevice.h"

//------------- activate applications here
extern Nl::StopWatch sw;

namespace Nl {
namespace MINISYNTH {

//--------------- Objects
VoiceManager voiceManager;
CPU_Stopwatch cpu_sw;

    /** @brief    Callback function for Sine Generator and Audio Input - testing with ReMote 61
        @param    Input Buffer
        @param    Output Buffer

        @param    buffer size
        @param    Sample Specs
    */
    void miniSynthCallback(u_int8_t *in, u_int8_t *out, const SampleSpecs &sampleSpecs __attribute__ ((unused)), SharedUserPtr ptr __attribute__ ((unused)))
    {
#ifdef PASCALSW
        static int counter = 0;
        //StopBlockTime sft(&sw, "val" + std::to_string(counter++));
        sw.stop();
        sw.start("val" + std::to_string(counter++));
#endif

#ifdef SYNTH
        // Nicht die ideale Lösung für die Initialisierung. Warte auf Update des Frameworks (04.08.2016)
        static bool init = false;

        if (!init)
        {
#ifdef CPU_STOPWATCH
            cpu_sw = CPU_Stopwatch();
#endif
            voiceManager = VoiceManager();                          // Vocie Manager
            // Initialization done
            init = true;
        }
#endif

#ifdef CPU_STOPWATCH
        static int counter = 0;
        cpu_sw.start();
#endif

        auto midiBuffer = getBufferForName("MidiBuffer");

        //---------------- Retrieve Midi Information if midi values have changed
        if (midiBuffer)
        {
            unsigned char midiByteBuffer[3];                                // MidiByteBuffer Structure: [0] - , [1] - Played Note Address, [2] - Velocity

            while (midiBuffer->availableToRead() >= 3)
            {
                midiBuffer->get(midiByteBuffer, 3);

//                printf("%02X %02X %02X\n", midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);      // MIDI Value Control Output
#ifdef SYNTH
                // pass Midi Values over to the Voice Manager
                voiceManager.evalMidiEvents(midiByteBuffer[0], midiByteBuffer[1], static_cast<float>(midiByteBuffer[2]));
#endif
            }
        }

#ifndef SYNTH
        float outputSample = 0.f;
#endif

        for (unsigned int frameIndex = 0; frameIndex < sampleSpecs.buffersizeInFramesPerPeriode; ++frameIndex)
        {

#ifdef SYNTH
            voiceManager.voiceLoop();                           // voice manager main loop
#endif

            for (unsigned int channelIndex = 0; channelIndex<sampleSpecs.channels; ++channelIndex)
            {

#ifdef SYNTH
                float outputSample = (channelIndex == 0)
                        ? voiceManager.mainOut_L
                        : voiceManager.mainOut_R;
#endif

                setSample(out, outputSample, frameIndex, channelIndex, sampleSpecs);
            }
        }

#ifdef CPU_STOPWATCH
        cpu_sw.stop();
        counter++;

        if (counter == NUMBER_OF_TS)
        {
            cpu_sw.calcCPU();

            std::cout << "CPU Peak: " << cpu_sw.mCPU_peak << std::endl;
            counter = 0;
        }
#endif
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

        ret.audioInput = createAlsaInputDevice(audioInCard, ret.inBuffer, buffersize);
        ret.audioInput->setSamplerate(samplerate);

        ret.audioOutput = createAlsaOutputDevice(audioOutCard, ret.outBuffer, buffersize);
        ret.audioOutput->setSamplerate(samplerate);

        ret.inMidiBuffer = createBuffer("MidiBuffer");
        ret.midiInput = createRawMidiDevice(midiInCard, ret.inMidiBuffer);

        ret.audioOutput->start();
        ret.audioInput->start();
        ret.midiInput->start();

	SharedUserPtr ptr(new UserPtr("unused", nullptr));
        ret.workingThreadHandle = registerInOutCallbackOnBuffer(ret.inBuffer, ret.outBuffer, miniSynthCallback, ptr);

        return ret;
    }

}   //namespace MINISYNTH
}   //namespace NL
