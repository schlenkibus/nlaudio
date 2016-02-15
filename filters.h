#pragma once
//#include <array>
//#include <cmath>
#include "math.h"

/** @file       filters.h
    @date       2016-04-02
    @brief      Filter implementations, Guidlines: Reaktor
    @author     Anton Schmied [2016-02-11]
**/

enum class onepoleFilters : int{
    lowpass = 0,
    highpass = 1,
    lowshelf = 2,
    highshelf = 3
};

class Filter{
    public:

    Filter(float sampleRate)            //Constructor for static 1-pole bilinear LP
        : in(0.f)
        , out(0.f)
        , inCh1Delay1(0.f)
        , outCh1Delay1(0.f)
        , inCh2Delay1(0.f)
        , outCh2Delay1(0.f)
        , sb0(0.f)
        , sb1(0.f)
        , sa1(0.f)
        , sRate(sampleRate)
    {
    }

    ~Filter(){}         //Destructor

    /** \ingroup Filters
    *
    * \brief Calculate coefficients for a 1-pole static Filter and apply to input (Guidlines: Reaktor)
    * \param frequency in Hz
    * \param current sample
    */
    float onePoleBilinearFilter(float currSample, float freq, float amp, onepoleFilters passtype, unsigned int channelIndex)
    {
        //check which Filter is active
        switch(passtype)
        {
            case onepoleFilters::lowpass:
            setup1PoleLowpass(freq);
            break;

            case onepoleFilters::highpass:
            setup1PoleHighpass(freq);
            break;

            case onepoleFilters::highshelf:
            setup1PoleHighshelf(freq, amp);
            break;

            case onepoleFilters::lowshelf:
            setup1PoleLowshelf(freq, amp);
            break;
        }

        in = currSample;

        if(channelIndex == 0)
        {
            sb0 = b0 * in;
            sb1 = b1 * inCh1Delay1;
            sa1 = a1 * outCh1Delay1;

            out = sb0 + sb1 + sa1;

            inCh1Delay1 = in;
            outCh1Delay1 = out;
        }
        else if (channelIndex == 1)
        {
            sb0 = b0 * in;
            sb1 = b1 * inCh2Delay1;
            sa1 = a1 * outCh2Delay1;

            out = sb0 + sb1 + sa1;

            inCh2Delay1 = in;
            outCh2Delay1 = out;
        }

        return out;
    }

private:

    float in;
    float out;
    float inCh1Delay1;
    float outCh1Delay1;
    float inCh2Delay1;
    float outCh2Delay1;

    float sb0;
    float sb1;
    float sa1;

    float sRate;

    float b0, b1, a1;               //Coefficients

    //float outputBuffer[];
    /** \ingroup Filters
    *
    * \brief Check for clipping and warp the incoming frequency value to normalized frequnecy
    * \param frequency in Hz
    * \param samplerate of the soundcard in Samples
    */
    void setup1PoleLowpass(float freq)
    {
        /*Check for clipping*/
        if (freq < (sRate / 24000.f))
        {
            //printf("Frequnecy too small!! Set to %f\n", sRate / 24000.f);
            freq = sRate / 24000.f;
        }
        else if (freq > (sRate / 2.18f))
        {
            //printf("Frequnecy too big!! Set to %f\n", sRate / 2.18f);
            freq = sRate / 2.18f;
        }

        /*Warp the incoming frequency*/
        freq *= (M_PI / sRate);
        // freq = tan(freq);
        freq = reakTan(freq);

        /*Calculate Coefficients*/
        a1 = (1.f - freq) / (1.f + freq);
        b0 = freq / (1.f + freq);
        b1 = freq / (1.f + freq);
    }

    void setup1PoleHighpass(float freq)
    {
        /*Check for clipping*/
        if (freq < (sRate / 24000.f))
        {
            printf("Frequnecy too small!! Set to %f\n", sRate / 24000.f);
            freq = sRate / 24000.f;
        }
        else if (freq > (sRate / 2.18f))
        {
            printf("Frequnecy too big!! Set to %f\n", sRate / 2.18f);
            freq = sRate / 2.18f;
        }

        /*Warp the incoming frequency*/
        freq *= (M_PI / sRate);
        //freq = tan(freq);
        freq = reakTan(freq);

        /*Calculate Coefficients*/
        a1 = (1.f - freq) / (1.f + freq);
        b0 = 1.f / (1.f + freq);
        b1 = -(1.f / (1.f + freq));
    }

    void setup1PoleLowshelf(float freq, float amp)
    {
        /*Warp Lowshelf*/
        amp = pow(10, (amp / 20));
        freq /= amp;
        amp *= amp;

        /*Check for clipping*/
        if (freq < (sRate / 24000.f))
        {
            //printf("Frequnecy too small!! Set to %f\n", sRate / 24000.f);
            freq = sRate / 24000.f;
        }
        else if (freq > (sRate / 2.18f))
        {
            //printf("Frequnecy too big!! Set to %f\n", sRate / 2.18f);
            freq = sRate / 2.18f;
        }

        /*Warp the incoming frequency*/
        freq *= (M_PI / sRate);
        //freq = tan(freq);
        freq = reakTan(freq);

        a1 = (1.f - freq) / (1.f + freq);

#if 0
        /*single step calculation*/
        float v = freq / (1.f + freq);
        float z = amp + -1.f;

        float d = v * z; //freq / (1.f + freq) * (amp + -1.f)

        b0 = d + 1;
        b1 = d - a1;
#endif

        b0 = ((freq / (1.f + freq)) * (amp + -1.f)) + 1.f;
        b1 = ((freq / (1.f + freq)) * (amp + -1.f)) - a1;
    }

    void setup1PoleHighshelf(float freq, float amp)
    {
        /*Warp Lowshelf*/
        amp = pow(10, (amp / 20));
        freq *= amp;
        amp *= amp;

        /*Check for clipping*/
        if (freq < (sRate / 24000.f))
        {
            //printf("Frequnecy too small!! Set to %f\n", sRate / 24000.f);
            freq = sRate / 24000.f;
        }
        else if (freq > (sRate / 2.18f))
        {
            //printf("Frequnecy too big!! Set to %f\n", sRate / 2.18f);
            freq = sRate / 2.18f;
        }

        /*Warp the incoming frequency*/
        freq *= (M_PI / sRate);
        //freq = tan(freq);
        freq = reakTan(freq);

        a1 = (1.f - freq) / (1.f + freq);
#if 0
        float v = freq / (1.f + freq);
        float z = (amp + -1.f);

        float b = z / (1.f + freq);

        b0 = b + 1.f;
        b1 = (b + a1) * -1.f;
#endif
        b0 = ((amp + -1.f) / (1.f + freq)) + 1.f;
        b1 = (((amp + -1.f) / (1.f + freq)) + a1) * -1.f;
    }

    /*as applied in Reaktor*/
    float reakTan(float freq){

        freq = 0.133333 * pow(freq, 5.f) + 0.333333 * pow(freq, 3.f) + freq;

        return freq;
    }
};


