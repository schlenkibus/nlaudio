#include "effects.h"

#include "stopwatch.h"
#include "audioalsainput.h"
#include "audioalsaoutput.h"
#include "rawmididevice.h"
#include "tools.h"
#include <atomic>

//select application here, check the midiIDs in the corresponding header files
//ONLY CABINET and ECHO are predefined to run simultaniously

//#define ONEPOLEFILTER
//#define BIQUADFILTER
//#define TILTFILTER
#define CABINET
#define ECHO

extern Nl::StopWatch sw;

namespace Nl {
namespace EFFECTS{

// Objects
Smoother volumeSmoother;
Smoother muteSmoother;
Smoother crossfadeSmoother;
Smoother inputSwitchSmoother;


void initializeEffects(int samplerate)
{
    volumeSmoother = Smoother(samplerate, 0.032f);
    muteSmoother = Smoother(samplerate, 0.032f);
    crossfadeSmoother = Smoother(samplerate, 0.032f);
    inputSwitchSmoother = Smoother(samplerate, 0.032f);

}


    /** @brief    Callback function for Sine Generator and Audio Input - testing with Korg nano Kontrol
        @param    Input Buffer
        @param    Output Buffer
        @param    buffer size
        @param    Sample Specs
    */
    void effectsCallback(u_int8_t *in, u_int8_t *out, size_t size, const SampleSpecs &sampleSpecs __attribute__ ((unused)))
    {
        static int counter = 0;
        //StopBlockTime sft(&sw, "val" + std::to_string(counter++));
        sw.stop();
        sw.start("val" + std::to_string(counter++));

        int samplerate = sampleSpecs.samplerate;                // Samplerate of Audio device

        static int inClipCntr = 0;
        static int outClipCntr = 0;

        static float curFrequency = 0.f;                        // Frequency for the Sine Generator

        float curMidiValue = 0.f;                               // Midi Value [0 .. 127]

        float faderMaxDB = 12.f;                                // Max of the Fader Range in dB
        int midiSteps = 24;                                     // Resolution in midi steps ... !
        static float stepResolution = faderMaxDB / midiSteps;   // dB resolution in dB per midi step

        static Smoother volumeSmoother(samplerate, 0.032f);
        float volumeFactor = 0.f;                               // Volume Factor [0 .. 3.98]

        static Smoother muteSmoother(samplerate, 0.032f);
        float muteFactor = 0.f;                                 // Mute Factor [0 .. 1]
        static int muteSwitch = 1;                              // Mute State [1 mute on, 0 mute off]

        static Smoother crossfadeSmoother(samplerate, 0.032f);
        float crossfadeFactor = 0.f;                            // Crossfade factor [0 .. 1] - calculated form midi
        static int crossfadeSwitch = 0;                         // Crossfade State [1 crossfade on, 0 crossfade off]

        static Smoother inputSwitchSmoother(samplerate, 0.032f);
        float inputSwitchFactor = 0.f;                          // Input switch factor [0 .. 1]
        static int inputSwitch = 0;                             // Input State [0 Sine, 1 Audio Input]

        float outputSample = 0.f;                               // Output Sample which comes from AudioIn or sine generator

#ifdef ONEPOLEFILTER
        static OnePoleFilters filter_l;                         // default Values: cutFreq: 22000.f, shelfAmp: 0.f, filterType: Lowpass
        static OnePoleFilters filter_r;
#endif
#ifdef BIQUADFILTER
        static BiquadFilters biquadFilter_l;                    // default Values: cutFreq: 22000.f, shelfAmp: 0.f, resonance: 0.5f, filterType: Lowpass
        static BiquadFilters biquadFilter_r;
#endif
#ifdef TILTFILTER
        static TiltFilters tiltFilter_l;                        // default Values: cutFreq: 22000.f, shelfAmp: 0.f, resonance: 0.5f, filterType: Lowpass
        static TiltFilters tiltFilter_r;
#endif
#ifdef CABINET
        static Cabinet cabinet_L;
        static Cabinet cabinet_R;
#endif

#ifdef ECHO
        static Echo echo;
#endif
        auto midiBuffer = getBufferForName("MidiBuffer");

        /*Retrieve Midi Information if midi values have changed*/
        if (midiBuffer)
        {
            unsigned char midiByteBuffer[3];                    //MidiByteBuffer Structure: [0] - , [1] - Played Note Address, [2] - Velocity

            while (midiBuffer->availableToRead() >= 3)
            {
                midiBuffer->get(midiByteBuffer, 3);

                //printf("%02X %02X %02X\n", midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);      //MIDI Value Output

                /*Retrieve Volume Fader Value and calculate current Volume*/
                if (midiByteBuffer[1] == 0x02 || midiByteBuffer[1] == 0x2A)
                {
                    curMidiValue = static_cast<float>(midiByteBuffer[2]);

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

                /*calculate crossfade values from midi input*/
                if (midiByteBuffer[1] == 0x03)
                {
                    crossfadeFactor = static_cast<float>(midiByteBuffer[2])/127;
                    crossfadeSmoother.initSmoother(crossfadeFactor);
                }

                /*Change Crossfade Switch State, if the Value has changed*/
                if ((midiByteBuffer[2] > 0x00) && (midiByteBuffer[1] == 0x22))
                {
                    switch (crossfadeSwitch)
                    {
                        case 0:
                        crossfadeSwitch = 1;
                        printf("Crossfade: ON\n");
                        break;

                        case 1:
                        crossfadeSwitch = 0;
                        printf("Crossfade: OFF\n");
                        break;
                    }
                }

                /*Retrieve Frequency Fader Value and calculate Frequency*/
                if (midiByteBuffer[1] == 0x16)
                {
                    curFrequency = 20.f * pow(2.f, static_cast<double>(midiByteBuffer[2]) / 12.75f);
                }

                /*Change Input Switch State, if the Value has changed*/
                if ((midiByteBuffer[2] > 0x00) && (midiByteBuffer[1] == 0x17))
                {
                    switch (inputSwitch)
                    {
                        case 0:
                        inputSwitchSmoother.initSmoother(1.f);
                        inputSwitch=1;
                        printf("Source: Audio Input chosen\n");
                        break;

                        case 1:
                        inputSwitchSmoother.initSmoother(0.f);
                        inputSwitch=0;
                        printf("Source: Sine Generator Input\n");
                        break;
                    }
                }

                /*Change Mute Switch State, if the Value has changed*/
                if ((midiByteBuffer[2] > 0x00) && (midiByteBuffer[1] == 0x21 || midiByteBuffer[1] == 0x4C))
                {
                    switch (muteSwitch)
                    {
                        case 0:
                        muteSmoother.initSmoother(0.f);
                        muteSwitch = 1;
                        printf("Mute: ON\n");
                        break;

                        case 1:
                        muteSmoother.initSmoother(1.f);
                        muteSwitch = 0;
                        printf("Mute: OFF\n");
                        break;
                    }
                }
#ifdef ONEPOLEFILTER
                filter_l.setFilterParams(static_cast<float>(midiByteBuffer[2]), midiByteBuffer[1]);
                filter_r.setFilterParams(static_cast<float>(midiByteBuffer[2]), midiByteBuffer[1]);
#endif
#ifdef BIQUADFILTER
                biquadFilter_l.setFilterParams(static_cast<float>(midiByteBuffer[2]), midiByteBuffer[1]);
                biquadFilter_r.setFilterParams(static_cast<float>(midiByteBuffer[2]), midiByteBuffer[1]);
#endif
#ifdef TILTFILTER
                tiltFilter_l.setFilterParams(static_cast<float>(midiByteBuffer[2]), midiByteBuffer[1]);
                tiltFilter_r.setFilterParams(static_cast<float>(midiByteBuffer[2]), midiByteBuffer[1]);
#endif
#ifdef CABINET
                cabinet_L.setCabinetParams(static_cast<float>(midiByteBuffer[2]), midiByteBuffer[1]);
                cabinet_R.setCabinetParams(static_cast<float>(midiByteBuffer[2]), midiByteBuffer[1]);
#endif
#ifdef ECHO
                echo.setEchoParams(static_cast<float>(midiByteBuffer[2]), midiByteBuffer[1]);
#endif
            }
        }


        float sineSamples[sampleSpecs.buffersizeInFramesPerPeriode];                        //Using Sine Generator from the function in tools.h
        sinewave<float>(sineSamples, curFrequency, false, sampleSpecs);

        for (unsigned int frameIndex=0; frameIndex<sampleSpecs.buffersizeInFramesPerPeriode; ++frameIndex)
        {
            volumeFactor = volumeSmoother.smooth();             //Volume Fader smoothing

            crossfadeFactor = crossfadeSmoother.smooth();       //Crossfade smoothing

            muteFactor = muteSmoother.smooth();                 //Mute smoothing

            inputSwitchFactor = inputSwitchSmoother.smooth();   //Input Switch smoothing

            for (unsigned int channelIndex = 0; channelIndex < sampleSpecs.channels; ++channelIndex)
            {
                float audioInSample = getSample(in, frameIndex, channelIndex, sampleSpecs);

                float sineSample = sineSamples[frameIndex];

                if (audioInSample > 1.f || audioInSample < -1.f)            //Audio Input Clipping
                {
                    printf("WARNING!!! AUDIO INPUT CLIPPING %d!\n", ++inClipCntr);
                }
                else if (sineSample > 1.f || sineSample < -1.f)             //Sine Clipping
                {
                    printf("WARNING!!! SINE CLIPPING %d!\n", ++inClipCntr);
                }
                else
                {
                    inClipCntr = 0;
                }


                outputSample = (audioInSample * inputSwitchFactor) + (sineSample * (1.f - inputSwitchFactor));

                if(crossfadeSwitch)                             //If crossfade is active, smoothen the crossfade Values
                {
                    outputSample = (sineSample * crossfadeFactor) + (audioInSample * (1.f - crossfadeFactor));
                }


                switch (channelIndex)                // apply filter on the active channel
                {
#ifdef ONEPOLEFILTER
                    case 0:
                    outputSample = filter_l.applyFilter(outputSample);              //1-Pole Filter influence
                    break;

                    case 1:
                    outputSample = filter_r.applyFilter(outputSample);              //1-Pole Filter influence
                    break;
#endif
#ifdef BIQUADFILTER
                    case 0:
                    outputSample = biquadFilter_l.applyFilter(outputSample);              //1-Pole Filter influence
                    break;

                    case 1:
                    outputSample = biquadFilter_r.applyFilter(outputSample);              //1-Pole Filter influence
                    break;
#endif
#ifdef TILTFILTER
                    case 0:
                    outputSample = tiltFilter_l.applyFilter(outputSample);              //1-Pole Filter influence
                    break;

                    case 1:
                    outputSample = tiltFilter_r.applyFilter(outputSample);              //1-Pole Filter influence
                    break;
#endif
                }
#ifdef CABINET
                switch (channelIndex)
                {
                    case 0:
                    outputSample = cabinet_L.applyCab(outputSample);
                    break;

                    case 1:
                    outputSample = cabinet_R.applyCab(outputSample);
                    break;
                }
#endif
#ifdef ECHO
                outputSample = echo.applyEcho(outputSample, channelIndex);
#endif
                outputSample *= volumeFactor;               //Volume Influence
                outputSample *= muteFactor;                 //Main Mute Influence

                if (outputSample > 1.f || outputSample < -1.f)
                {
                    printf("WARNING!!! OUTPUT CLIPPING %d!\n", ++outClipCntr);
                }
                else
                {
                    outClipCntr = 0;
                }

                setSample(out, outputSample, frameIndex, channelIndex, sampleSpecs);
            }
        }
    }

    effectsMidiControlHandle effectsMidiControl(const AlsaCardIdentifier &audioInCard,
                                         const AlsaCardIdentifier &audioOutCard,
                                         const AlsaCardIdentifier &midiInCard,
                                         unsigned int buffersize,
                                         unsigned int samplerate)
    {
        effectsMidiControlHandle ret;

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

        ret.workingThreadHandle = registerInOutCallbackOnBuffer(ret.inBuffer, ret.outBuffer, effectsCallback);

        return ret;
    }
}   //namespace EFFECTS
}   //namespace NL
