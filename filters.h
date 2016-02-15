#pragma once
//#include <array>
//#include <cmath>
#include "math.h"

/** @file       filters.h
    @date       2016-04-02
    @brief      Filter implementations, Guidlines: Reaktor
    @author     Anton Schmied [2016-02-11]
**/

enum class OnePoleFilterPasstype : int{
    lowpass = 0,
    highpass = 1,
    lowshelf = 2,
    highshelf = 3
};

class OnePoleFilters{
    public:

    OnePoleFilters(float _sRate, float _cutFreq, float _shelfAmp, OnePoleFilterPasstype _passtype)            //Constructor for static 1-pole bilinear LP
        : sRate(_sRate)
        , in(0.f)
        , out(0.f)
        , inCh1Delay1(0.f)
        , outCh1Delay1(0.f)
        , inCh2Delay1(0.f)
        , outCh2Delay1(0.f)
        , sb0(0.f)
        , sb1(0.f)
        , sa1(0.f)
    {
        setCutFreq(_cutFreq, _passtype);
        setShelfAmp(_shelfAmp, _passtype);
    }

    ~OnePoleFilters(){}         //Destructor

    void setCutFreq(float _cutFreq, OnePoleFilterPasstype passtype)
    {
        cutFreq = _cutFreq;

        /*Check for clipping*/
        if (cutFreq < (sRate / 24000.f))
        {
            cutFreq = sRate / 24000.f;
        }

        else if (cutFreq > (sRate / 2.18f))
        {
            cutFreq = sRate / 2.18f;
        }

        /*Warp the incoming frequency*/
        cutFreq *= (M_PI / sRate);
        // freq = tan(cutFreq);                 //C++ Library Function
        cutFreq = reakTan(cutFreq);             //Reaktor Solution

        switch(passtype)                        //check which Filter is active and update coefficients
        {
            case OnePoleFilterPasstype::lowpass:
            setup1PoleLowpass();
            break;

            case OnePoleFilterPasstype::highpass:
            setup1PoleHighpass();
            break;

            case OnePoleFilterPasstype::highshelf:
            setup1PoleHighshelf();
            break;

            case OnePoleFilterPasstype::lowshelf:
            setup1PoleLowshelf();
            break;
        }
    }

    void setShelfAmp(float _shelfAmp, OnePoleFilterPasstype passtype)
    {
        shelfAmp = _shelfAmp;
        shelfAmp = pow(10, (shelfAmp / 20));

        switch(passtype)                        //check which Filter is active and update coefficients
        {
            case OnePoleFilterPasstype::highshelf:
            cutFreq *= shelfAmp;
            shelfAmp *= shelfAmp;
            setup1PoleHighshelf();
            break;

            case OnePoleFilterPasstype::lowshelf:
            cutFreq /= shelfAmp;
            shelfAmp *= shelfAmp;
            setup1PoleLowshelf();
            break;

            case OnePoleFilterPasstype::highpass:
            break;

            case OnePoleFilterPasstype::lowpass:
            break;
        }
    }

    /** \ingroup Filters
    *
    * \brief Apply coefficients to the Input
    * \param current Sample
    * \param channel Index (assuming we are using 2 channels)
    */
    float onePoleBilinearFilter(float currSample, unsigned int channelIndex)
    {     
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

    float cutFreq;
    float shelfAmp;
    float sRate;

    float in;
    float out;
    float inCh1Delay1;
    float outCh1Delay1;
    float inCh2Delay1;
    float outCh2Delay1;

    float sb0;
    float sb1;
    float sa1;


    float b0, b1, a1;               //Coefficients

    //float outputBuffer[];

    /** \ingroup Filters
    *
    * \brief Calculate the coefficients depending on which filter was chosen
    */
    void setup1PoleLowpass()
    {
        a1 = (1.f - cutFreq) / (1.f + cutFreq);
        b0 = cutFreq / (1.f + cutFreq);
        b1 = cutFreq / (1.f + cutFreq);
    }

    void setup1PoleHighpass()
    {
        a1 = (1.f - cutFreq) / (1.f + cutFreq);
        b0 = 1.f / (1.f + cutFreq);
        b1 = -(1.f / (1.f + cutFreq));
    }

    void setup1PoleLowshelf()
    {
        a1 = (1.f - cutFreq) / (1.f + cutFreq);
        b0 = ((cutFreq / (1.f + cutFreq)) * (shelfAmp + -1.f)) + 1.f;
        b1 = ((cutFreq / (1.f + cutFreq)) * (shelfAmp + -1.f)) - a1;
    }

    void setup1PoleHighshelf()
    {
        a1 = (1.f - cutFreq) / (1.f + cutFreq);
        b0 = ((shelfAmp + -1.f) / (1.f + cutFreq)) + 1.f;
        b1 = (((shelfAmp + -1.f) / (1.f + cutFreq)) + a1) * -1.f;
    }

    /*as applied in Reaktor*/
    float reakTan(float freq){

        freq = 0.133333 * pow(freq, 5.f) + 0.333333 * pow(freq, 3.f) + freq;

        return freq;
    }
};


