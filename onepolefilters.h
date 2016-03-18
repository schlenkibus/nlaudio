#pragma once
#include "tools.h"

/** @file       onepolefilters.h
    @date       2016-04-02
    @brief      1 Pole Filter implementations, Guidlines: Reaktor
    @author     Anton Schmied [2016-02-11]
**/

enum class OnePoleFiltertype
{
    lowpass,
    highpass,
    lowshelf,
    highshelf
};

class OnePoleFilters{
    public:

    OnePoleFilters(int _sRate,
                   float _cutFreq = 22000,
                   float _shelfAmp = 0.f,
                   OnePoleFiltertype _filtertype = OnePoleFiltertype::lowpass)            //Constructor for static 1-pole bilinear LP
        : sRate(static_cast<float>(_sRate))
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
        if (filtertype == OnePoleFiltertype::lowshelf)
        {
            _cutFreq /= shelfAmp;
        }
        else if (filtertype == OnePoleFiltertype::highshelf)
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
        omega_tan = Nl::tan(_cutFreq);             //alternative to tan(cutFreq) -> tools.h;

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

    float applyFilter(float currSample, unsigned int chInd)
    {
        float output = 0.f;

        if(chInd == 0)
        {
            output += b0 * currSample;
            output += b1 * inCh1Delay1;
            output += a1 * outCh1Delay1;

            inCh1Delay1 = currSample;
            outCh1Delay1 = output;
        }

        else if (chInd == 1)
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
            case OnePoleFiltertype::lowpass:
            a1 = (1.f - omega_tan) / (1.f + omega_tan);
            b0 = omega_tan / (1.f + omega_tan);
            b1 = omega_tan / (1.f + omega_tan);
            break;

            case OnePoleFiltertype::highpass:
            a1 = (1.f - omega_tan) / (1.f + omega_tan);
            b0 = 1.f / (1.f + omega_tan);
            b1 = (1.f / (1.f + omega_tan)) * -1.f;
            break;

            case OnePoleFiltertype::lowshelf:
            a1 = (1.f - omega_tan) / (1.f + omega_tan);
            b0 = ((omega_tan / (1.f + omega_tan)) * (shelfAmp_square + -1.f)) + 1.f;
            b1 = ((omega_tan / (1.f + omega_tan)) * (shelfAmp_square + -1.f)) - a1;
            break;

            case OnePoleFiltertype::highshelf:
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
#if 0
    /*as applied in Reaktor*/
    float reakTan(float x){

        x = 0.133333 * pow(x, 5.f) + 0.333333 * pow(x, 3.f) + x;

        return x;
    }
#endif
};


