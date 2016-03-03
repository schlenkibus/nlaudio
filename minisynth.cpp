#include "minisynth.h"

#include "stopwatch.h"
#include "audioalsainput.h"
#include "audioalsaoutput.h"
#include "rawmididevice.h"
#include "tools.h"
#include "cabinet.h"

extern Nl::StopWatch sw;

namespace Nl {
namespace MINISYNTH {


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

        int samplerate = sampleSpecs.samplerate;                //Samplerate of Audio device
        static float outputSample = 0.f;

        static float frequency = 0.f;                                       //Sine Frequency
        static float velocity = 0.f;                                        //Velocity value
        static int notesOn = 0;                                             //1 Note is played, 0 Note is not played        
        bool reset = false;                                                 //Phase reset

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
            unsigned char midiByteBuffer[3];                                /*MidiByteBuffer Structure: [0] - , [1] - Played Note Address, [2] - Velocity*/

            while (midiBuffer->availableToRead() >= 3)
            {
                midiBuffer->get(midiByteBuffer, 3);


                printf("%02X %02X %02X\n", midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);      //MIDI Value Control Output

                if (midiByteBuffer[0] == 0x90)
                {
                    if (midiByteBuffer[2] > 0x00)
                    {
                        notesOn++;

                            frequency = pow(2.f, static_cast<float>((midiByteBuffer[1]-69)/12.f))*440.f;    //From MIDI to Frequency

                            reset = true;

                            //velocity = static_cast<float>(midiByteBuffer[2])/127.f;                       //From MIDI to Velocity [0.0 .. 1.0] - linear
                            velocity = pow(10,(static_cast<float>(midiByteBuffer[2]+1.f)-64.f)/64.f)/10.f;  //From MIDI to Velocity [0.0 .. 1.0] - logarithmic
                    }
                    else
                    {
                        notesOn--;
                    }
                }
                else if (midiByteBuffer[0] == 0x80)
                {
                    notesOn--;
                }

#if 1
                if (midiByteBuffer[0] == 0xB0)
                {
                    /*Cabinet*/
                    /*Retrieve hiCut Frequency from Knob [B0 05] and calculate Frequency this schould be from 260Hz to 26737Hz*/
                    if (midiByteBuffer[1] == 0x05)
                    {
                        hiCut = 260.f * pow(2.f, static_cast<float>(midiByteBuffer[2]) / 19.f);
                        cabinet.setHiCut(hiCut);
                    }

                    /*Retrieve loCut Frequency from Knob [B0 50] and calculate Frequency this schould be from 25Hz to 2637Hz*/
                    if (midiByteBuffer[1] == 0x50)
                    {
                        loCut = 25.f * pow(2.f, static_cast<float>(midiByteBuffer[2]) / 18.9f);
                        cabinet.setLoCut(loCut);
                    }

                    /*Retrieve mix amount from Fader [B0 6C]*/
                    if (midiByteBuffer[1] == 0x6C)
                    {
                        mix = static_cast<float>(midiByteBuffer[2]) / 127.f;
                        cabinet.setMix(mix);
                    }

                    /*Retrieve Cabinet Level from Fader [B0 6E]*/
                    if (midiByteBuffer[1] == 0x6E)
                    {
                        cabLvl = (static_cast<float>(midiByteBuffer[2]) - 127.f) * (50.f / 127.f);
                        cabinet.setCabLvl(cabLvl);
                    }

                    /*Retrieve Drive from Fader [B0 6D]*/
                    if (midiByteBuffer[1] == 0x6D)
                    {
                        drive = static_cast<float>(midiByteBuffer[2]) * (50.f / 127.f);
                        cabinet.setDrive(drive);
                    }

                    /*Retrieve Tilt from Fader [B0 52]*/
                    if (midiByteBuffer[1] == 0x52)
                    {
                        tilt = (static_cast<float>(midiByteBuffer[2]) - 64.f) * (50.f / 64.f);
                        cabinet.setTilt(tilt);
                    }

                    /*Retrieve Fold from Fader [B0 53]*/
                    if (midiByteBuffer[1] == 0x53)
                    {
                        fold = static_cast<float>(midiByteBuffer[2]) / 127.f;
                        cabinet.setFold(fold);
                    }

                    /*Retrieve Asym from Fader [B0 55]*/
                    if (midiByteBuffer[1] == 0x55)
                    {
                        asym = static_cast<float>(midiByteBuffer[2]) / 127.f;
                        cabinet.setAsym(asym);
                    }
                }
#endif
#if 0
                /*Input-Switch Selection - not used for now*/
                if (midiByteBuffer[2] > 0x00 && midiByteBuffer[1] == 0x48)
                {
                    switch(inputSwitch)
                    {
                        case 0:
                        inputSwitch=1;
                        break;

                        case 1:
                        inputSwitch=0;
                        break;
                    }
                }
#endif
            }
        }

        /*Check if the note is being played*/
        if (notesOn > 0)
        {
            float sineSamples[sampleSpecs.buffersizeInFramesPerPeriode];
            sinewave<float>(sineSamples, frequency, reset, sampleSpecs);

            for (unsigned int frameIndex=0; frameIndex<sampleSpecs.buffersizeInFramesPerPeriode; ++frameIndex)
            {
                for (unsigned int channelIndex=0; channelIndex<sampleSpecs.channels; ++channelIndex)
                {
                    outputSample = sineSamples[frameIndex] * velocity;
                    outputSample = cabinet.applyCab(outputSample, channelIndex);                        //Cabinet influence
                    setSample(out, outputSample, frameIndex, channelIndex, sampleSpecs);
                }
            }
        }
        else
        {
            memset(out,0, size);
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
