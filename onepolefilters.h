#pragma once
//#include <array>
//#include <cmath>
#include "math.h"

/** @file       onepolefilters.h
    @date       2016-04-02
    @brief      1 Pole Filter implementations, Guidlines: Reaktor
    @author     Anton Schmied [2016-02-11]
**/

enum OnePoleFiltertype{
    onepole_lowpass,
    onepole_highpass,
    onepole_lowshelf,
    onepole_highshelf
};

class OnePoleFilters{
    public:

    OnePoleFilters(float _sRate, float _cutFreq = 22000, float _shelfAmp = 0.f, OnePoleFiltertype _filtertype = OnePoleFiltertype::onepole_lowpass)            //Constructor for static 1-pole bilinear LP
        : sRate(_sRate)
        , inCh1Delay1(0.f)
        , outCh1Delay1(0.f)
        , inCh2Delay1(0.f)
        , outCh2Delay1(0.f)
    {
        setCutFreq(_cutFreq);
        setShelfAmp(_shelfAmp);
        setFiltertype(_filtertype);
    }

    ~OnePoleFilters(){}         //Destructor

    void setCutFreq(float _cutFreq)
    {
        if (filtertype == OnePoleFiltertype::onepole_lowshelf)
        {
            _cutFreq /= shelfAmp;
        }
        else if (filtertype == OnePoleFiltertype::onepole_highshelf)
        {
            _cutFreq *= shelfAmp;
        }

        /*Check for clipping*/
        if (_cutFreq < (sRate / 24000.f))
        {
            _cutFreq = sRate / 24000.f;
        }

        else if (_cutFreq > (sRate / 2.18f))
        {
            _cutFreq = sRate / 2.18f;
        }

        _cutFreq *= (M_PI / sRate);                //Warp the incoming frequency
        omega_tan = reakTan(_cutFreq);             //alternative to tan(cutFreq);

        calcCoeff();
    }

    void setShelfAmp(float _shelfAmp)
    {
        shelfAmp = pow(1.059f, _shelfAmp);         //alternative to pow(10, (_shelfAmp / 40.f));
        shelfAmp_square = shelfAmp * shelfAmp;

        calcCoeff();
    }

    void setFiltertype(OnePoleFiltertype _filtertype)                // set filtertype
    {
        filtertype = _filtertype;
        resetDelays();
        calcCoeff();
    }

    /** \ingroup Filters
    *
    * \brief Apply coefficients to the Input
    * \param current Sample
    * \param channel Index (assuming we are using 2 channels)
    */
    float applyFilter(float currSample, unsigned int channelIndex)
    {
        float output = 0.f;

        if(channelIndex == 0)
        {
            output += b0 * currSample;
            output += b1 * inCh1Delay1;
            output += a1 * outCh1Delay1;

            inCh1Delay1 = currSample;
            outCh1Delay1 = output;
        }

        else if (channelIndex == 1)
        {
            output += b0 * currSample;
            output += b1 * inCh2Delay1;
            output += a1 * outCh2Delay1;

            inCh2Delay1 = currSample;
            outCh2Delay1 = output;
        }

        return output;
    }

private:

    float omega_tan;
    float shelfAmp;
    float shelfAmp_square;
    float sRate;

    float inCh1Delay1;
    float outCh1Delay1;
    float inCh2Delay1;
    float outCh2Delay1;

    float b0, b1, a1;               //Coefficients

    OnePoleFiltertype filtertype;

    void calcCoeff()
    {
        switch(filtertype)                        //check which Filter is active and update coefficients
        {
            case OnePoleFiltertype::onepole_lowpass:
            a1 = (1.f - omega_tan) / (1.f + omega_tan);
            b0 = omega_tan / (1.f + omega_tan);
            b1 = omega_tan / (1.f + omega_tan);
            break;

            case OnePoleFiltertype::onepole_highpass:
            a1 = (1.f - omega_tan) / (1.f + omega_tan);
            b0 = 1.f / (1.f + omega_tan);
            b1 = (1.f / (1.f + omega_tan)) * -1.f;
            break;

            case OnePoleFiltertype::onepole_lowshelf:
            a1 = (1.f - omega_tan) / (1.f + omega_tan);
            b0 = ((omega_tan / (1.f + omega_tan)) * (shelfAmp_square + -1.f)) + 1.f;
            b1 = ((omega_tan / (1.f + omega_tan)) * (shelfAmp_square + -1.f)) - a1;
            break;

            case OnePoleFiltertype::onepole_highshelf:
            a1 = (1.f - omega_tan) / (1.f + omega_tan);
            b0 = ((shelfAmp_square + -1.f) / (1.f + omega_tan)) + 1.f;
            b1 = (((shelfAmp_square + -1.f) / (1.f + omega_tan)) + a1) * -1.f;
            break;
        }
    }

    void resetDelays()              //sould think about an array and smoothing...
    {
        inCh1Delay1 = 0.f;          //Channel 1
        outCh1Delay1 = 0.f;

        inCh2Delay1 = 0.f;          //Channel 2
        outCh2Delay1 = 0.f;
    }

    /*as applied in Reaktor*/
    float reakTan(float x){

        x = 0.133333 * pow(x, 5.f) + 0.333333 * pow(x, 3.f) + x;

        return x;
    }
};


