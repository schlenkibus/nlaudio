#include "minisynth.h"

#include "stopwatch.h"
#include "audioalsainput.h"
#include "audioalsaoutput.h"
#include "rawmididevice.h"
#include "tools.h"

#include "phase22.h"
#include "smoother.h"
#include "cabinet.h"

//------------- activate applications here
#define CABINET

extern Nl::StopWatch sw;

namespace Nl {
namespace MINISYNTH {

#define NUM_VOICES 12

//--------------- Objects
Smoother volumeSmoother;
Phase22 voice[NUM_VOICES];

#ifdef CABINET
Cabinet cabinet;
#endif

//--------------- Voice Allocation global variables
static int voiceState[NUM_VOICES] = {};                               // which voices are active? 1 - on, 0 - off

static int oldestAssigned;
static int youngestAssigned;
static int nextAssigned[NUM_VOICES] = {};                             // array with the next elemts per voice
static int previousAssigned[NUM_VOICES] = {};

static int numAssigned;

static int oldestReleased;                                           // index of the earliest disabled voice (full use: earliest turned on)
static int youngestReleased;                                         // index of the last disabled voice (full use: last turned on)
static int nextReleased[NUM_VOICES] = {};

//--------------- step resolution caculation
float faderMaxDB = 12.f;                                // Max of the Fader Range in dB
int midiSteps = 24;                                     // Resolution in midi steps ... !
float stepResolution = faderMaxDB / midiSteps;          // dB resolution in dB per midi step




/******************************************************************************/
/** @brief   initialisation function for all synth components and
 *           voice allocation variables
*******************************************************************************/

    void initializeSynth(int sampleRate)
    {
        volumeSmoother = Smoother(sampleRate, 0.032f);          // Volume Smoother
        volumeSmoother.initSmoother(1.f);

        voice[NUM_VOICES] = Phase22();                          // Phase22 Voices

        for (unsigned int i = 0; i < NUM_VOICES; i++)
        {
            voice[i].setVoiceNumber(i);
        }

#ifdef CABINET
        cabinet = Cabinet();                                    // Cabinet Effect
#endif

        for (int i = 0; i < NUM_VOICES; i++)                    // Voice Allocation Initialization
        {
            voiceState[i] = -1;
        }

        numAssigned = 0;
        oldestAssigned = 0;
        oldestReleased = 0;
        youngestAssigned = 0;
        youngestReleased = NUM_VOICES - 1;

        for (int i = 0; i < NUM_VOICES - 1; i++)
        {
            nextReleased[i] = i + 1;
        }
    }



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
            initializeSynth(sampleSpecs.samplerate);

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

//                printf("%02X %02X %02X\n", midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);      // MIDI Value Control Output

                /*Retrieve Volume Fader Value and calculate current Volume*/
                if (midiByteBuffer[1] == 0x30 && (midiByteBuffer[0] == 0xB0 || midiByteBuffer[0] == 0xB1))
                {
                    float curMidiValue = static_cast<float>(midiByteBuffer[2]);
                    float volumeFactor;

                    if (curMidiValue < (127 - (127 / midiSteps) * midiSteps))
                    {
                        volumeFactor = pow(10.f, (faderMaxDB - (127.f - curMidiValue) * stepResolution) / 20.f) * curMidiValue * 0.1429f;
                    }
                    else
                    {
                        volumeFactor = pow(10.f, (faderMaxDB - (127.f - curMidiValue) * stepResolution) / 20.f);
                    }
                    printf("volumeFactor: %f\n", volumeFactor);
                    volumeSmoother.initSmoother(volumeFactor);
                }

                int midiVal = static_cast<int>(midiByteBuffer[1]);

                // Voice Allocation - Note On
                if (midiByteBuffer[0] == 0x90 || midiByteBuffer[0] == 0x91 || midiByteBuffer[0] == 0x92)         // key down
                {
                    int v;

                    if (numAssigned < NUM_VOICES)
                    {
                        if (numAssigned == 0)
                        {
                            oldestAssigned = oldestReleased;
                            youngestAssigned = oldestReleased;
                        }

                        v = oldestReleased;
                        oldestReleased = nextReleased[v];

                        numAssigned++;
                    }
                    else
                    {
                        v = oldestAssigned;
                        oldestAssigned = nextAssigned[v];
                    }

                    previousAssigned[v] = youngestAssigned;
                    nextAssigned[youngestAssigned] = v;

                    youngestAssigned = v;

                    voiceState[v] = midiVal;


                    voice[v].setPitch(static_cast<float>(midiVal));
                    voice[v].resetPhase(); // hier ist es falsch
                }

                // Voice Allocation - Note Off
                else if (midiByteBuffer[0] == 0x80 || midiByteBuffer[0] == 0x81 || midiByteBuffer[0] == 0x82)    // key up
                {
                    int v;

                    for (v = 0; v < NUM_VOICES; v++)
                    {
                        if (voiceState[v] == midiVal)
                        {
                            nextReleased[youngestReleased] = v;
                            youngestReleased = v;

                            if (numAssigned == NUM_VOICES)
                            {
                                oldestReleased = v;
                            }

                            numAssigned--;

                            if (oldestAssigned == v)
                            {
                                oldestAssigned = nextAssigned[v];

                            }
                            else if (youngestAssigned == v)
                            {
                                youngestAssigned = previousAssigned[v];
                            }
                            else
                            {
                                nextAssigned[previousAssigned[v]] = nextAssigned[v];
                                previousAssigned[nextAssigned[v]] = previousAssigned[v];
                            }

                            voiceState[v] = -1;

                            break;
                        }
                    }
                }

                for(unsigned int i = 0; i < NUM_VOICES; i++)
                {
                    voice[i].setOscParams(midiByteBuffer[0], midiByteBuffer[1], static_cast<float>(midiByteBuffer[2]));
                }

#ifdef CABINET
                if (midiByteBuffer[0] == 0xB2)
                {
                    cabinet.setCabinetParams(static_cast<float>(midiByteBuffer[2]), midiByteBuffer[1]);
                }
#endif
            }
        }


        for (unsigned int frameIndex = 0; frameIndex < sampleSpecs.buffersizeInFramesPerPeriode; ++frameIndex)
        {
            float outputSample = 0.f;
            float volumeFactor;

            volumeFactor = volumeSmoother.smooth();             //Volume Fader smoothing

            for(unsigned int i = 0; i < NUM_VOICES; i++)        //Calculate the output of all the voices if these are active
                if(voiceState[i] > -1)
                    outputSample += voice[i].makeNoise();
#ifdef CABINET
            outputSample = cabinet.applyCab(outputSample);
#endif
            outputSample *= volumeFactor;


            for (unsigned int channelIndex=0; channelIndex<sampleSpecs.channels; ++channelIndex)
            {
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
