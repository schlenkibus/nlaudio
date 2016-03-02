#include "effects.h"
#include "onepolefilters.h"
#include "biquadfilters.h"
#include "cabinet.h"

#include "stopwatch.h"
#include "audioalsainput.h"
#include "audioalsaoutput.h"
#include "rawmididevice.h"
#include "tools.h"
#include <atomic>
extern Nl::StopWatch sw;

namespace Nl {
namespace EFFECTS{


    /** @brief    mutes the Inputsignal depending on the state of the muteSwitch
        @param    muteState [0 - off, 1 - on]
        @param    current Sample
        @param    smoothing Factor predefined
        @param    mute factor [0..1]
        @return   resturns the effected sample
    */
    float muteThis(int muteSwitch, float curSample, float sFactor, float *mFactor)
    {
        if(muteSwitch)                          //mFactor is außerhalb der Funktion deklariert ... eher uncool ... vielleicht!?
        {
            if(*mFactor <= 0.f)
                *mFactor = 0.f;
            else
                *mFactor -= sFactor;
        }
        else if(muteSwitch == 0)
        {
            if(*mFactor >= 1.f)
                *mFactor = 1.f;
            else
                *mFactor += sFactor;
        }

        return curSample * *mFactor;
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

        int samplerate = sampleSpecs.samplerate;                //Samplerate of Audio device

        static float curFrequency = 0.f;                        //Frequency for the Sine Generator
        //static float lastFrequency = 0.f;                       //last Freqneuncy from frequency generator
        //static float freqSmoothingFactor = 0.f;                  //Smoothing Factor for the frequency Fader
        //static int freqFadeDirection = 0;                       //Fader Direction [1 move up, -1 move down, 0 not moved]

        bool reset = false;                                     //Phase Reset

        // static float velocity = 0;
        // static int notesOn = 0;
        float curMidiValue = 0.f;                               //Midi Value [0 .. 127]

        static float curVolumeFactor = 0.f;                     //current Volume Factor [0 .. ]
        static float lastVolumeFactor = 0.f;                    //last Volume Factor [0 ..]
        static float volumeSmoothingFactor = 0.f;               //smoothing Factor for the Volume Fader
        static int volFadeDirection = 0;                        //Fader Direction [1 move up, -1 move down, 0 not moved]

        float smoothInSec = 0.05f;                                        //Length of Smoothing in Seconds
        int smoothInSamples = static_cast<int>(smoothInSec * samplerate); //Length of Mute Smoothing in Samples

        float muteSmoothingFactor = 1.f / smoothInSamples;      //Mute Smoothing Factor
        static float muteFactor = 0.f;                          //mute Factor [0 .. 1]
        static int muteSwitch = 1;                              //Mute State [1 mute on, 0 mute off]

        float faderMaxDB = 12.f;                                //Max of the Fader Range in dB
        int midiSteps = 24;                                     //Resolution in midi steps ... !
        static float stepResolution = faderMaxDB / midiSteps;   //dB resolution in dB per midi step

        static float curCrossfadeVal = 0.f;                     //crossfade values [0 .. 1] - calculated form midi
        static float lastCrossfadeVal = 0.f;                    //crossfade values [0 .. 1]
        static int crossfaderDirection = 0;                     //Crossfader Direction [1 turn right, -1 turn left, 0 not moved]
        static float crossfadeSmoothingFactor = 0.f;            //Smoothing Factor for the crossfader
        static int crossfadeSwitch = 0;                         //Crossfade On/Off

        static int inputSwitch = 0;                             //Input State [0 Sine, 1 Audio Input]
        static float inputSwitchVal = 0.f;
        float outputSample = 0.f;                               //output Sample which comes from AudioIn or sine generator

#if 0
        static BiquadFilterPasstype passtype;
        static BiquadFilters biquadFilter(samplerate);          //default Values: cutoff(22000.f), shelfAmp(0.f), resonance(0.5f), passtype(lowpass)
        static float cutoffFreq;
        static float shelfAmp;
        static float resonance;
        static int switchCounter = 0;
#endif

        static Cabinet cabinet(samplerate);
        static float hiCut;
        static float loCut;
        static float mix;
        static float cabLvl;
        static float drive;
        static float tilt;
        static float fold;
        static float asym;

        auto midiBuffer = getBufferForName("MidiBuffer");

        /*Retrieve Midi Information if midi values have changed*/
        if (midiBuffer)
        {
            unsigned char midiByteBuffer[3];                    //MidiByteBuffer Structure: [0] - , [1] - Played Note Address, [2] - Velocity

            while (midiBuffer->availableToRead() >= 3)
            {
                midiBuffer->get(midiByteBuffer, 3);

                printf("%02X %02X %02X\n", midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);      //MIDI Value Output

                /*Retrieve Volume Fader Value and calculate current Volume*/
                if (midiByteBuffer[1] == 0x06)
                {
                    curMidiValue = static_cast<float>(midiByteBuffer[2]);

                    if (curMidiValue < (127 - (127 / midiSteps) * midiSteps))
                    {  
                        curVolumeFactor = pow(10.f, (faderMaxDB - (127.f - curMidiValue) * stepResolution) / 20.f) * curMidiValue * 0.1429f;
                    }
                    else
                    {
                        curVolumeFactor = pow(10.f, (faderMaxDB - (127.f - curMidiValue) * stepResolution) / 20.f);
                        //printf("curVolumeFactor: %f\n", curVolumeFactor);
                    }
                }

                /*Change Volumfade Direction State, if it has changed*/
                if (lastVolumeFactor < curVolumeFactor)
                {
                    volFadeDirection = 1;
                }
                else if (lastVolumeFactor > curVolumeFactor)
                {
                    volFadeDirection = -1;
                }
                else
                {
                    volFadeDirection = 0;
                }

                /*calculate crossfade values from midi input*/
                if (midiByteBuffer[1] == 0x12)
                {
                    curCrossfadeVal = static_cast<float>(midiByteBuffer[2])/127;
                }

                /*Change Crossfade Direction State, if it has changed*/
                if (lastCrossfadeVal < curCrossfadeVal)
                {
                    crossfaderDirection = 1;
                }
                else if (lastCrossfadeVal > curCrossfadeVal)
                {
                    crossfaderDirection = -1;
                }
                else
                {
                    crossfaderDirection = 0;
                }

                /*Retrieve Frequency Fader Value and calculate Frequency*/
                if (midiByteBuffer[1] == 0x02)
                {
                    curFrequency = 20.f * pow(2.f, static_cast<double>(midiByteBuffer[2]) / 12.75f);
                    reset = true;
                }

#if 0
                /*1-Pole Filters*/
                /*Retrieve cutoff Frequency from Knob [13] and calculate Frequency this schould be from 20Hz to 20kHz*/
                if (midiByteBuffer[1] == 0x13)
                {
                    cutoffFreq = 20.f * pow(2.f, static_cast<double>(midiByteBuffer[2]) / 12.75f);

                    onePoleFilter.setCutFreq(cutoffFreq, passtype);
                }

                /*Retrieve shelf amplification from fader - Range -12dB .. 12dB*/
                if (midiByteBuffer[1] == 0x08)
                {
                    shelfAmp = ((static_cast<float>(midiByteBuffer[2]) - 64) * 6) / 64;

                    if((passtype == OnePoleFilterPasstype::lowshelf) || (passtype == OnePoleFilterPasstype::highshelf))
                    {
                        onePoleFilter.setShelfAmp(shelfAmp, passtype);
                    }
                }

                /*Filter Passtype Switch*/
                if ((midiByteBuffer[1] == 0x1C) && (midiByteBuffer[2] > 0x00))
                {
                    if (switchCounter == 0)
                    {
                        passtype = OnePoleFilterPasstype::lowpass;
                        onePoleFilter.setCutFreq(cutoffFreq, passtype);
                        printf("lowpass on\n");
                    }

                    if (switchCounter == 1)
                    {
                        passtype = OnePoleFilterPasstype::highpass;
                        onePoleFilter.setCutFreq(cutoffFreq, passtype);
                        printf("highpass on\n");
                    }

                    if (switchCounter == 2)
                    {
                        passtype = OnePoleFilterPasstype::lowshelf;
                        onePoleFilter.setCutFreq(cutoffFreq, passtype);
                        onePoleFilter.setShelfAmp(shelfAmp, passtype);
                        printf("lowshelf on\n");
                    }

                    if (switchCounter == 3)
                    {
                        passtype = OnePoleFilterPasstype::highshelf;
                        onePoleFilter.setCutFreq(cutoffFreq, passtype);
                        onePoleFilter.setShelfAmp(shelfAmp, passtype);
                        printf("highshelf on\n");
                    }

                    ++switchCounter;

                    if (switchCounter > 3)
                        switchCounter = 0;
                }
#endif

#if 0
                /*Biquad Filters*/
                /*Retrieve cutoff Frequency from Knob [13] and calculate Frequency this schould be from 20Hz to 20kHz*/
                if (midiByteBuffer[1] == 0x13)
                {
                    cutoffFreq = 20.f * pow(2.f, static_cast<double>(midiByteBuffer[2]) / 12.75f);

                    biquadFilter.setCutFreq(cutoffFreq, resonance, passtype);
                }

                /*Retrieve cutoff Frequency from Knob [14] and calculate Frequency this schould be from 20Hz to 20kHz*/
                if (midiByteBuffer[1] == 0x14)
                {
                    resonance = (static_cast<float>(midiByteBuffer[2]) - 64.f) / 32.f;
                    printf("resonance: %f\n", resonance);
                    biquadFilter.setCutFreq(cutoffFreq, resonance, passtype);
                }

                /*Retrieve shelf amplification from fader - Range -12dB .. 12dB*/
                if (midiByteBuffer[1] == 0x08)
                {
                    shelfAmp = ((static_cast<float>(midiByteBuffer[2]) - 64.f) * 50.f) / 64.f;

                    if((passtype == BiquadFilterPasstype::lowshelf) || (passtype == BiquadFilterPasstype::highshelf))
                    {
                        biquadFilter.setShelfAmp(shelfAmp, passtype);
                    }
                }

                /*Filter Passtype Switch*/
                if ((midiByteBuffer[1] == 0x1C) && (midiByteBuffer[2] > 0x00))
                {
                    if (switchCounter == 0)
                    {
                        passtype = BiquadFilterPasstype::lowpass;
                        biquadFilter.setCutFreq(cutoffFreq, resonance, passtype);
                        printf("lowpass on\n");
                    }

                    if (switchCounter == 1)
                    {
                        passtype = BiquadFilterPasstype::highpass;
                        biquadFilter.setCutFreq(cutoffFreq, resonance, passtype);
                        printf("highpass on\n");
                    }

                    if (switchCounter == 2)
                    {
                        passtype = BiquadFilterPasstype::lowshelf;
                        biquadFilter.setCutFreq(cutoffFreq, resonance, passtype);
                        biquadFilter.setShelfAmp(shelfAmp, passtype);
                        printf("lowshelf on\n");
                    }

                    if (switchCounter == 3)
                    {
                        passtype = BiquadFilterPasstype::highshelf;
                        biquadFilter.setCutFreq(cutoffFreq, resonance, passtype);
                        biquadFilter.setShelfAmp(shelfAmp, passtype);
                        printf("highshelf on\n");
                    }

                    ++switchCounter;

                    if (switchCounter > 3)
                        switchCounter = 0;
                }
#endif

#if 1
                /*Cabinet*/
                /*Retrieve hiCut Frequency from Knob [13] and calculate Frequency this schould be from 260Hz to 26737Hz*/
                if (midiByteBuffer[1] == 0x13)
                {
                    hiCut = 260.f * pow(2.f, static_cast<float>(midiByteBuffer[2]) / 19.f);
                    cabinet.setHiCut(hiCut);
                }

                /*Retrieve loCut Frequency from Knob [20] and calculate Frequency this schould be from 25Hz to 2637Hz*/
                if (midiByteBuffer[1] == 0x14)
                {
                    loCut = 25.f * pow(2.f, static_cast<float>(midiByteBuffer[2]) / 18.9f);
                    cabinet.setLoCut(loCut);
                }

                /*Retrieve mix amount from Fader [03]*/
                if (midiByteBuffer[1] == 0x03)
                {
                    mix = static_cast<float>(midiByteBuffer[2]) / 127.f;
                    cabinet.setMix(mix);
                }

                /*Retrieve Cabinet Level from Fader [04]*/
                if (midiByteBuffer[1] == 0x04)
                {
                    cabLvl = (static_cast<float>(midiByteBuffer[2]) - 127.f) * (50.f / 127.f);
                    cabinet.setCabLvl(cabLvl);
                }

                /*Retrieve Drive from Fader [05]*/
                if (midiByteBuffer[1] == 0x05)
                {
                    drive = static_cast<float>(midiByteBuffer[2]) * (50.f / 127.f);
                    cabinet.setDrive(drive);
                }

                /*Retrieve Tilt from Fader [08]*/
                if (midiByteBuffer[1] == 0x08)
                {
                    tilt = (static_cast<float>(midiByteBuffer[2]) - 64.f) * (50.f / 64.f);
                    cabinet.setTilt(tilt);
                }

                /*Retrieve Fold from Fader [09]*/
                if (midiByteBuffer[1] == 0x09)
                {
                    fold = static_cast<float>(midiByteBuffer[2]) / 127.f;
                    cabinet.setFold(fold);
                }

                /*Retrieve Asym from Fader [0C]*/
                if (midiByteBuffer[1] == 0x0C)
                {
                    asym = static_cast<float>(midiByteBuffer[2]) / 127.f;
                    cabinet.setAsym(asym);
                }
#endif

#if 0
                /*Change frequency fade Direction State, if it has changed*/
                if (lastFrequency < curFrequency)
                {
                    freqFadeDirection = 1;
                }
                else if (lastFrequency > curFrequency)
                {
                    freqFadeDirection = -1;
                }
                else
                {
                    freqFadeDirection = 0;
                }
#endif

                /*Change Input Switch State, if the Value has changed*/
                if ((midiByteBuffer[2] > 0x00) && (midiByteBuffer[1] == 0x17))
                {
                    switch (inputSwitch)
                    {
                        case 0:
                        inputSwitch=1;
                        printf("Source: Audio Input chosen\n");
                        break;

                        case 1:
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
                        muteSwitch = 1;
                        printf("Mute: ON\n");
                        break;

                        case 1:
                        muteSwitch = 0;
                        printf("Mute: OFF\n");
                        break;
                    }
                }

                /*Change Mute Switch State, if the Value has changed*/
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

                /*smoothing factors- all linear for now*/
                volumeSmoothingFactor = (curVolumeFactor - lastVolumeFactor) / smoothInSamples;
                crossfadeSmoothingFactor = (curCrossfadeVal - lastCrossfadeVal) / smoothInSamples;
                //freqSmoothingFactor = (curFrequency - lastFrequency) / smoothInSamples;
            }
        }

#if 0
        /*check freq fader direction and limits - apply smoother*/
        if (((freqFadeDirection == 1) && (lastFrequency > curFrequency)) || ((freqFadeDirection == -1) && (lastFrequency < curFrequency)))
        {
            lastFrequency = curFrequency;
            freqFadeDirection = 0;
        }
        else if (freqFadeDirection != 0)
        {
            lastFrequency += freqSmoothingFactor;
            //reset = true;
        }
#endif

        float sineSamples[sampleSpecs.buffersizeInFramesPerPeriode];                        //Using Sine Generator from the function in tools.h
        sinewave<float>(sineSamples, curFrequency, reset, sampleSpecs);
        //sinewave<float>(sineSamples, lastFrequency, reset, sampleSpecs);

        for (unsigned int frameIndex=0; frameIndex<sampleSpecs.buffersizeInFramesPerPeriode; ++frameIndex)
        {
            /*check volume fader direction and limits - apply smoother*/
            if (((volFadeDirection == 1) && (lastVolumeFactor > curVolumeFactor)) || ((volFadeDirection == -1) && (lastVolumeFactor < curVolumeFactor)))
            {
                lastVolumeFactor = curVolumeFactor;
                volFadeDirection = 0;
            }
            else if (volFadeDirection != 0)
            {
                lastVolumeFactor += volumeSmoothingFactor;
            }

            /*check crossfade direction and limits - apply smoother*/
            if (((crossfaderDirection == 1) && (lastCrossfadeVal > curCrossfadeVal)) || ((crossfaderDirection == -1) && (lastCrossfadeVal < curCrossfadeVal)))
            {
                lastCrossfadeVal = curCrossfadeVal;
                crossfaderDirection = 0;
            }
            else if (crossfaderDirection != 0)
            {
                lastCrossfadeVal += crossfadeSmoothingFactor;
            }

            for (unsigned int channelIndex = 0; channelIndex<sampleSpecs.channels; ++channelIndex)
            {
                /*Smoothen the Input Switch if the butten has been pressed*/
                if(inputSwitch)
                {
                    float audioInSample = getSample(in, frameIndex, channelIndex, sampleSpecs);
                    audioInSample *= lastVolumeFactor;

                    float sineSample = sineSamples[frameIndex] * lastVolumeFactor;

                    inputSwitchVal += muteSmoothingFactor;

                    if(inputSwitchVal > 1)
                        inputSwitchVal = 1.f;

                    outputSample = (audioInSample * inputSwitchVal) + (sineSample * (1 - inputSwitchVal));
                }
                else
                {
                    float audioInSample = getSample(in, frameIndex, channelIndex, sampleSpecs);
                    audioInSample *= lastVolumeFactor;

                    float sineSample = sineSamples[frameIndex] * lastVolumeFactor;

                    inputSwitchVal -= muteSmoothingFactor;

                    if(inputSwitchVal < 0)
                        inputSwitchVal = 0.f;

                    outputSample = (audioInSample * inputSwitchVal) + (sineSample * (1 - inputSwitchVal));
                }

                /*If crossfade is active, smoothen the crossfade Values*/
                if(crossfadeSwitch)
                {
                    float audioInSample = getSample(in, frameIndex, channelIndex, sampleSpecs);
                    audioInSample *= lastVolumeFactor;

                    float sineSample = sineSamples[frameIndex] * lastVolumeFactor;

                    outputSample = (sineSample * lastCrossfadeVal) + (audioInSample * (1 - lastCrossfadeVal));
                }

                /*Mute Check*/
                outputSample = muteThis(muteSwitch, outputSample, muteSmoothingFactor, &muteFactor);                    //Mute influnce

#if 0
                outputSample = onePoleFilter.applyFilter(outputSample, channelIndex);              //1-Pole Filter influence
                outputSample = biquadFilter.applyFilter(outputSample, channelIndex);               //Biquad Filter influence
#endif
                outputSample = cabinet.applyCab(outputSample, channelIndex);                        //Cabinet influence

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
