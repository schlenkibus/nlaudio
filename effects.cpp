#include "effects.h"
#include "onepolefilters.h"
#include "biquadfilters.h"
#include "altbiquadfilters.h"
#include "cabinet.h"
#include "smoother.h"

//#define ONEPOLE 1                 //choose which filter you want to test here
//#define BIQUAD 1
//#define ALTBIQUAD 1
//#define TILTFILTER 1
#define CABINET 1


#include "stopwatch.h"
#include "audioalsainput.h"
#include "audioalsaoutput.h"
#include "rawmididevice.h"
#include "tools.h"
#include <atomic>
extern Nl::StopWatch sw;

namespace Nl {
namespace EFFECTS{

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

        int samplerate = sampleSpecs.samplerate;                //Samplerate of Audio device

        static int inClipCntr = 0;
        static int outClipCntr = 0;

        static float curFrequency = 0.f;                        //Frequency for the Sine Generator

        float curMidiValue = 0.f;                               //Midi Value [0 .. 127]

        float faderMaxDB = 12.f;                                //Max of the Fader Range in dB
        int midiSteps = 24;                                     //Resolution in midi steps ... !
        static float stepResolution = faderMaxDB / midiSteps;   //dB resolution in dB per midi step

        static Smoother volumeSmoother(samplerate, 0.032f);
        float volumeFactor = 0.f;                               //Volume Factor [0 .. 3.98]

        static Smoother muteSmoother(samplerate, 0.032f);
        float muteFactor = 0.f;                                 //mute Factor [0 .. 1]
        static int muteSwitch = 1;                              //Mute State [1 mute on, 0 mute off]

        static Smoother crossfadeSmoother(samplerate, 0.032f);
        float crossfadeFactor = 0.f;                            //crossfade factor [0 .. 1] - calculated form midi
        static int crossfadeSwitch = 0;                         //Crossfade State [1 crossfade on, 0 crossfade off]

        static Smoother inputSwitchSmoother(samplerate, 0.032f);
        float inputSwitchFactor = 0.f;                          //input switch factor [0 .. 1]
        static int inputSwitch = 0;                             //Input State [0 Sine, 1 Audio Input]

        float outputSample = 0.f;                               //output Sample which comes from AudioIn or sine generator

#ifdef ONEPOLE
        static OnePoleFilters onePoleFilter(samplerate);
        static float cutoffFreq;
        static float shelfAmp;
        static int switchCounter = 0;
#endif
#ifdef BIQUAD
        static BiquadFilters biquadFilter(samplerate);          //default Values: cutoff(22000.f), shelfAmp(0.f), resonance(0.5f), passtype(lowpass)
        static float cutoffFreq;
        static float shelfAmp;
        static float resonance;
        static int switchCounter = 0;
#endif
#ifdef ALTBIQUAD
        static AltBiquadFilters altBiquadFilter(samplerate);          //default Values: cutoff(22000.f), shelfAmp(0.f), resonance(0.5f), passtype(lowpass)
        static float cutoffFreq;
        static float shelfAmp;
        static float resonance;
        static int switchCounter = 0;
#endif
#ifdef TILTFILTER
        TiltFilters tiltFilter(samplerate);                  //default Values: cutoff(22000.f), shelfAmp(0.f), resonance(0.5f), passtype(lowpass)
        static float cutoffFreq;
        static float tilt;
        static float resonance;
        static int switchCounter = 0;
#endif
#ifdef CABINET
        static Cabinet cabinet(samplerate);
        static float hiCut;
        static float loCut;
        static float mix;
        static float cabLvl;
        static float drive;
        static float tilt;
        static float fold;
        static float asym;
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
                if (midiByteBuffer[1] == 0x06)
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
                if (midiByteBuffer[1] == 0x12)
                {
                    crossfadeFactor = static_cast<float>(midiByteBuffer[2])/127;
                    crossfadeSmoother.initSmoother(crossfadeFactor);
                }

                /*Retrieve Frequency Fader Value and calculate Frequency*/
                if (midiByteBuffer[1] == 0x02)
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
                if ((midiByteBuffer[2] > 0x00) && (midiByteBuffer[1] == 0x25))
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

                /*Change Crossfade Switch State, if the Value has changed*/
                if ((midiByteBuffer[2] > 0x00) && (midiByteBuffer[1] == 0x1B))
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

#ifdef ONEPOLE
                /*1-Pole Filters*/
                /*Retrieve cutoff Frequency from Knob [13] and calculate Frequency this schould be from 20Hz to 20kHz*/
                if (midiByteBuffer[1] == 0x13)
                {
                    cutoffFreq = 20.f * pow(2.f, static_cast<double>(midiByteBuffer[2]) / 12.75f);

                    onePoleFilter.setCutFreq(cutoffFreq);
                }

                /*Retrieve shelf amplification from fader - Range -12dB .. 12dB*/
                if (midiByteBuffer[1] == 0x08)
                {
                    shelfAmp = ((static_cast<float>(midiByteBuffer[2]) - 64) * 12.f) / 64.f;

                    if((switchCounter == 2) || (switchCounter == 3)){
                        onePoleFilter.setShelfAmp(shelfAmp);
                    }
                }

                /*Filter Passtype Switch*/
                if ((midiByteBuffer[1] == 0x1C) && (midiByteBuffer[2] > 0x00))
                {
                    ++switchCounter;
                    currSample = lowpass1.applyF0.032f)ilter(currSample, channelIndex);
                    currSample = lowpass2.applyFilter(currSample, channelIndex);
                    if (switchCounter > 3)
                        switchCounter = 0;

                    if (switchCounter == 0)
                    {
                        onePoleFilter.setFiltertype(OnePoleFiltertype::lowpass);
                        printf("lowpass on\n");
                    }

                    if (switchCounter == 1)
                    {
                        onePoleFilter.setFiltertype(OnePoleFiltertype::highpass);
                        printf("highpass on\n");
                    }

                    if (switchCounter == 2)
                    {
                        onePoleFilter.setFiltertype(OnePoleFiltertype::lowshelf);
                        printf("lowshelf on\n");
                    }

                    if (switchCounter == 3)
                    {
                        onePoleFilter.setFiltertype(OnePoleFiltertype::highshelf);
                        printf("highshelf on\n");
                    }
                }
#endif
#ifdef BIQUAD
                /*Biquad Filters*/
                /*Retrieve cutoff Frequency from Knob [13] and calculate Frequency this schould be from 20Hz to 20kHz*/
                if (midiByteBuffer[1] == 0x13)
                {
                    cutoffFreq = 20.f * pow(2.f, static_cast<double>(midiByteBuffer[2]) / 12.75f);

                    biquadFilter.setCutFreq(cutoffFreq);
                }

                /*Retrieve resonance Frequency from Knob [14] and calculate Frequency this schould be from 20Hz to 20kHz*/
                if (midiByteBuffer[1] == 0x14)
                {
                    resonance = (static_cast<float>(midiByteBuffer[2]) - 64.f) / 32.f;

                    biquadFilter.setResonance(resonance);
                }

                /*Retrieve shelf amplification from fader - Range -12dB .. 12dB*/
                if (midiByteBuffer[1] == 0x08)
                {
                    shelfAmp = ((static_cast<float>(midiByteBuffer[2]) - 64.f) * 12.f) / 64.f;

                    if((switchCounter == 2) || (switchCounter == 3))
                    {
                        biquadFilter.setShelfAmp(shelfAmp);
                    }
                }

                /*Filter Passtype Switch*/
                if ((midiByteBuffer[1] == 0x1C) && (midiByteBuffer[2] > 0x00))
                {
                    ++switchCounter;
                    if (switchCounter > 3)
                        switchCounter = 0;

                    if (switchCounter == 0)
                    {
                        biquadFilter.setFiltertype(BiquadFiltertype::lowpass);
                        printf("lowpass on\n");
                    }

                    if (switchCounter == 1)
                    {
                        biquadFilter.setFiltertype(BiquadFiltertype::highpass);
                        printf("highpass on\n");
                    }

                    if (switchCounter == 2)
                    {
                        biquadFilter.setFiltertype(BiquadFiltertype::lowshelf);
                        printf("lowshelf on\n");
                    }

                    if (switchCounter == 3)
                    {
                        biquadFilter.setFiltertype(BiquadFiltertype::highshelf);
                        printf("highshelf on\n");
                    }

                }
#endif
#ifdef ALTBIQUAD
                /*Biquad Filters*/
                /*Retrieve cutoff Frequency from Knob [13] and calculate Frequency this schould be from 20Hz to 20kHz*/
                if (midiByteBuffer[1] == 0x13)
                {
                    cutoffFreq = 20.f * pow(2.f, static_cast<double>(midiByteBuffer[2]) / 12.75f);

                    altBiquadFilter.setCutFreq(cutoffFreq);
                }

                /*Retrieve resonance Frequency from Knob [14] and calculate Frequency this schould be from 20Hz to 20kHz*/
                if (midiByteBuffer[1] == 0x14)
                {
                    resonance = (static_cast<float>(midiByteBuffer[2]) - 64.f) / 32.f;

                    altBiquadFilter.setResonance(resonance);
                }

                /*Retrieve shelf amplification from fader - Range -12dB .. 12dB*/
                if (midiByteBuffer[1] == 0x08)
                {
                    shelfAmp = ((static_cast<float>(midiByteBuffer[2]) - 64.f) * 12.f) / 64.f;

                    if((switchCounter == 2) || (switchCounter == 3))
                    {
                        altBiquadFilter.setShelfAmp(shelfAmp);
                    }
                }

                /*Filter Passtype Switch*/
                if ((midiByteBuffer[1] == 0x1C) && (midiByteBuffer[2] > 0x00))
                {
                    ++switchCounter;
                    if (switchCounter > 3)
                        switchCounter = 0;

                    if (switchCounter == 0)
                    {
                        altBiquadFilter.setFiltertype(AltBiquadFiltertype::lowpass);
                        printf("lowpass on\n");
                    }

                    if (switchCounter == 1)
                    {
                        altBiquadFilter.setFiltertype(AltBiquadFiltertype::highpass);
                        printf("highpass on\n");
                    }

                    if (switchCounter == 2)
                    {
                        altBiquadFilter.setFiltertype(AltBiquadFiltertype::lowshelf);
                        printf("lowshelf on\n");
                    }

                    if (switchCounter == 3)
                    {
                        altBiquadFilter.setFiltertype(AltBiquadFiltertype::highshelf);
                        printf("highshelf on\n");
                    }
                }
#endif
#ifdef TILTFILTER
                /*TILT Filters*/
                /*Retrieve cutoff Frequency from Knob [13] and calculate Frequency this schould be from 20Hz to 20kHz*/
                if (midiByteBuffer[1] == 0x13)
                {
                    cutoffFreq = 20.f * pow(2.f, static_cast<double>(midiByteBuffer[2]) / 12.75f);

                    tiltFilter.setCutFreq(cutoffFreq);
                }

                /*Retrieve resonance Frequency from Knob [14] and calculate Frequency this schould be from 20Hz to 20kHz*/
                if (midiByteBuffer[1] == 0x14)
                {
                    resonance = (static_cast<float>(midiByteBuffer[2]) - 64.f) / 32.f;

                    tiltFilter.setResonance(resonance);
                }

                /*Retrieve shelf amplification from fader - Range -12dB .. 12dB*/
                if (midiByteBuffer[1] == 0x08)
                {
                    tilt = (static_cast<float>(midiByteBuffer[2]) - 64.f) * (50.f / 64.f);

                    if((switchCounter == 2) || (switchCounter == 3))
                    {
                        tiltFilter.setTilt(tilt);
                    }
                }

                /*Filter Passtype Switch*/
                if ((midiByteBuffer[1] == 0x1C) && (midiByteBuffer[2] > 0x00))
                {
                    ++switchCounter;
                    if (switchCounter > 3)
                        switchCounter = 0;

                    if (switchCounter == 0)
                    {
                        tiltFilter.setFiltertype(TiltFiltertype::lowpass);
                        printf("lowpass on\n");
                    }

                    if (switchCounter == 1)
                    {
                        tiltFilter.setFiltertype(TiltFiltertype::highpass);
                        printf("highpass on\n");
                    }

                    if (switchCounter == 2)
                    {
                        tiltFilter.setFiltertype(TiltFiltertype::lowshelf);
                        printf("lowshelf on\n");
                    }

                    if (switchCounter == 3)
                    {
                        tiltFilter.setFiltertype(TiltFiltertype::highshelf);
                        printf("highshelf on\n");
                    }

                }
#endif
#ifdef CABINET
                /*Cabinet*/
                /*Retrieve hiCut Frequency from Knob [13] and calculate Frequency this schould be from 260Hz to 26737Hz*/
                if (midiByteBuffer[1] == 0x13)
                {
                    // Pitch Values for better testing
                    hiCut = (static_cast<float>(midiByteBuffer[2]) * 80.f) / 127.f + 60.f;
                    printf("HiCut: %f\n", hiCut);

                    hiCut = pow(2.f, (hiCut - 69.f) / 12) * 440.f;

                    //hiCut = 260.f * pow(2.f, static_cast<float>(midiByteBuffer[2]) / 19.f);

                    cabinet.setHiCut(hiCut);
                }

                /*Retrieve loCut Frequency from Knob [20] and calculate Frequency this schould be from 25Hz to 2637Hz*/
                if (midiByteBuffer[1] == 0x14)
                {
                    // Pitch Values for better testing
                    loCut = (static_cast<float>(midiByteBuffer[2]) * 80.f) / 127.f + 20.f;
                    printf("LoCut: %f\n", loCut);

                    loCut = pow(2.f, (loCut - 69.f) / 12) * 440.f;

                    //loCut = 25.f * pow(2.f, static_cast<float>(midiByteBuffer[2]) / 18.9f);

                    cabinet.setLoCut(loCut);
                }

                /*Retrieve mix amount from Fader [03]*/
                if (midiByteBuffer[1] == 0x03)
                {
                    mix = static_cast<float>(midiByteBuffer[2]) / 127.f;
                    printf("Mix: %f\n", mix);
                    cabinet.setMix(mix);
                }

                /*Retrieve Cabinet Level from Fader [04]*/
                if (midiByteBuffer[1] == 0x04)
                {
                    cabLvl = (static_cast<float>(midiByteBuffer[2]) - 127.f) * (50.f / 127.f);
                    printf("Cab Lvl: %f\n", cabLvl);

                    cabinet.setCabLvl(cabLvl);
                }

                /*Retrieve Drive from Fader [05]*/
                if (midiByteBuffer[1] == 0x05)
                {
                    drive = static_cast<float>(midiByteBuffer[2]) * (50.f / 127.f);
                    printf("Drive: %f\n", drive);

                    cabinet.setDrive(drive);
                }

                /*Retrieve Tilt from Fader [08]*/
                if (midiByteBuffer[1] == 0x08)
                {
                    tilt = (static_cast<float>(midiByteBuffer[2]) - 63.5f) * (50.f / 63.5f);
                    printf("Tilt: %f\n", tilt);

                    cabinet.setTilt(tilt);
                }

                /*Retrieve Fold from Fader [09]*/
                if (midiByteBuffer[1] == 0x09)
                {
                    fold = static_cast<float>(midiByteBuffer[2]) / 127.f;
                    printf("Fold: %f\n", fold);

                    cabinet.setFold(fold);
                }

                /*Retrieve Asym from Fader [0C]*/
                if (midiByteBuffer[1] == 0x0C)
                {
                    asym = static_cast<float>(midiByteBuffer[2]) / 127.f;
                    printf("Asym: %f\n", asym);

                    cabinet.setAsym(asym);
                }
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

            for (unsigned int channelIndex = 0; channelIndex<sampleSpecs.channels; ++channelIndex)
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

#ifdef ONEPOLE
                outputSample = onePoleFilter.applyFilter(outputSample, channelIndex);              //1-Pole Filter influence
#endif
#ifdef BIQUAD
                outputSample = biquadFilter.applyFilter(outputSample, channelIndex);               //Biquad Filter influence
#endif
#ifdef ALTBIQUAD
                outputSample = altBiquadFilter.applyFilter(outputSample, channelIndex);            //alternative Biquad approach Filter influence
#endif
#ifdef TILTFILTER
                outputSample = tiltFilter.applyFilter(outputSample, channelIndex);                 //Biquad Filter influence
#endif
#ifdef CABINET
                outputSample = cabinet.applyCab(outputSample, channelIndex);
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
