#pragma once
#include "math.h"

enum BiquadFiltertype
{
    lowpass,
    highpass,
    lowshelf,
    highshelf
};

class BiquadFilters
{
public:
    BiquadFilters(float _sRate, float _cutFreq = 22000.f, float _shelfAmp = 0.f,
                  float _resonance = 0.5f, BiquadFiltertype _filtertype = BiquadFiltertype::lowpass)           // Constructor for static a biquad Filter
        : sRate(_sRate)
        , in(0.f)
        , out(0.f)
        , inCh1Delay1(0.f)          //Delays
        , inCh1Delay2(0.f)
        , outCh1Delay1(0.f)
        , outCh1Delay2(0.f)
        , inCh2Delay1(0.f)
        , inCh2Delay2(0.f)
        , outCh2Delay1(0.f)
        , outCh2Delay2(0.f)
        , sb0(0.f)                  //intirim calculations
        , sb1(0.f)
        , sb2(0.f)
        , sa1(0.f)
        , sa2(0.f)
        , b0(0.f)                   //Coefficients
        , b1(0.f)
        , b2(0.f)
        , a0(0.f)
        , a1(0.f)
        , a2(0.f)

    {
        setCutFreq(_cutFreq);
        setShelfAmp(_shelfAmp);
        setResonance(_resonance);
        setFiltertype(_filtertype);
    }

    ~BiquadFilters(){}

    void setCutFreq(float _cutFreq)                  /*set cut frequency*/
    {
        if (_cutFreq < (sRate / 24000.f))            //Check for Frequency clipping
        {
            _cutFreq = sRate / 24000.f;
        }

        else if (_cutFreq > (sRate / 2.18f))
        {
            _cutFreq = sRate / 2.18f;
        }

        omega = _cutFreq * (2.f * M_PI / sRate);     //Frequncy to Omega (Warp)

        //omega_cos = cos(cutFreq);
        omega_cos = reakCos(omega);

        setAlpha();
        setupBiquadFilter();
    }

    void setShelfAmp(float _shelfAmp)               /*set shelf amplification*/
    {
        shelfAmp = pow(10.f, (_shelfAmp / 40.f));   //dB to amplification factor
        setupBiquadFilter();
    }

    void setResonance(float _resonance)             /*set resonance*/
    {
        resonance = _resonance;

        if (resonance > 0.999f)                     //check for resonance clipping
        {
            resonance = 0.999f;
        }

        else if (resonance < -0.999f)
        {
            resonance = -0.999f;
        }

        setAlpha();
        setupBiquadFilter();
    }

    void setFiltertype(BiquadFiltertype _filtertype)                // set filtertype
    {
        filtertype = _filtertype;
        resetDelays();
        setupBiquadFilter();
    }


    float applyFilter(float currSample, unsigned int channelIndex)
    {
        in = currSample;

        if(channelIndex == 0)
        {
            sb0 = b0 / a0;
            sb0 *= in;
            sb1 = b1 / a0;
            sb1 *= inCh1Delay1;
            sb2 = b2 / a0;
            sb2 *= inCh1Delay2;

            sa1 = -1.f * a1 / a0;
            sa1 *= outCh1Delay1;
            sa2 = -1.f * a2 / a0;
            sa2 *= outCh1Delay2;

            out = sb0 + sb1 + sb2 + sa1 + sa2;

            inCh1Delay2 = inCh1Delay1;
            inCh1Delay1 = in;

            outCh1Delay2 = outCh1Delay1;
            outCh1Delay1 = out;
        }

        else if (channelIndex == 1)
        {
            sb0 = b0 / a0 * in;
            sb1 = b1 / a0 * inCh2Delay1;
            sb2 = b2 / a0 * inCh2Delay2;

            sa1 = -1.f * a1 / a0 * outCh2Delay1;
            sa2 = -1.f * a2 / a0 * outCh2Delay2;

            out = sb0 + sb1 + sb2 + sa1 + sa2;

            inCh2Delay2 = inCh2Delay1;
            inCh2Delay1 = in;

            outCh2Delay2 = outCh2Delay1;
            outCh2Delay1 = out;
        }

        return out;
    }

private:

    float omega;
    float omega_cos;
    float alpha;
    float shelfAmp;
    float resonance;
    float sRate;

    float in;
    float out;

    float inCh1Delay1;          //Channel 1
    float inCh1Delay2;
    float outCh1Delay1;
    float outCh1Delay2;

    float inCh2Delay1;          //Channel 2
    float inCh2Delay2;
    float outCh2Delay1;
    float outCh2Delay2;

    float sb0, sb1, sb2, sa1, sa2;              //interim calculation - array!?

    float b0, b1, b2, a0, a1, a2;               //Coefficient - array!?

    BiquadFiltertype filtertype;                //Filtertype

    void setupBiquadFilter()
    {
        switch(filtertype)
        {
            case BiquadFiltertype::lowpass:
            calcLowpassCoeff();
            break;

            case BiquadFiltertype::highpass:
            calcHighpassCoeff();
            break;

            case BiquadFiltertype::highshelf:
            calcHighshelfCoeff();
            break;

            case BiquadFiltertype::lowshelf:
            calcLowshelfCoeff();
            break;
        }
    }

    void calcLowpassCoeff()
    {
        a0 = 1 + alpha;
        a1 = omega_cos * -2.f;
        a2 = 1 - alpha;
        b0 = (1 - omega_cos) / 2.f;
        b1 = 1 - omega_cos;
        b2 = b0;
    }

    void calcHighpassCoeff()
    {
        a0 = 1 + alpha;
        a1 = omega_cos * -2.f;
        a2 = 1 - alpha;
        b0 = (1 + omega_cos) / 2.f;
        b1 = (1 + omega_cos) * -1.f;
        b2 = b0;
    }

    void calcLowshelfCoeff()
    {
        a0 = (shelfAmp + 1.f) + (omega_cos * (shelfAmp - 1.f)) + (2.f * sqrt(shelfAmp) * alpha);
        a1 = ((shelfAmp - 1.f) + (omega_cos * (shelfAmp + 1.f))) * -2.f;
        a2 = (shelfAmp + 1.f) + (omega_cos * (shelfAmp - 1.f)) - (2.f * sqrt(shelfAmp) * alpha);
        b0 = ((shelfAmp + 1.f) - (omega_cos * (shelfAmp - 1.f)) + (2.f * sqrt(shelfAmp) * alpha)) * shelfAmp;
        b1 = ((shelfAmp - 1.f) - (omega_cos * (shelfAmp + 1.f))) * 2.f * shelfAmp;
        b2 = ((shelfAmp + 1.f) - (omega_cos * (shelfAmp - 1.f)) - (2.f * sqrt(shelfAmp) * alpha)) * shelfAmp;
    }

    void calcHighshelfCoeff()
    {
        a0 = (shelfAmp + 1.f) - (omega_cos * (shelfAmp - 1.f)) + (2.f * sqrt(shelfAmp) * alpha);
        a1 = ((shelfAmp - 1.f) - (omega_cos * (shelfAmp + 1.f))) * 2.f;
        a2 = (shelfAmp + 1.f) - (omega_cos * (shelfAmp - 1.f)) - (2.f * sqrt(shelfAmp) * alpha);
        b0 = ((shelfAmp + 1.f) + (omega_cos * (shelfAmp - 1.f)) + (2.f * sqrt(shelfAmp) * alpha)) * shelfAmp;
        b1 = ((shelfAmp - 1.f) + (omega_cos * (shelfAmp + 1.f))) * -2.f * shelfAmp;
        b2 = ((shelfAmp + 1.f) + (omega_cos * (shelfAmp - 1.f)) - (2.f * sqrt(shelfAmp) * alpha)) * shelfAmp;
    }

    void setAlpha()
    {
        //alpha = sin(omega) * (1.f - resonance);
        alpha = reakSin(omega) * (1.f - resonance);
    }

    void resetDelays()              //sould think about an array and smoothing...
    {
        inCh1Delay1 = 0.f;          //Channel 1
        inCh1Delay2 = 0.f;
        outCh1Delay1 = 0.f;
        outCh1Delay2 = 0.f;

        inCh2Delay1 = 0.f;          //Channel 2
        inCh2Delay2 = 0.f;
        outCh2Delay1 = 0.f;
        outCh2Delay2 = 0.f;
    }


    /*as applied in Reaktor --- befindet sich hier vorübergehend*/
    float reakSin(float x){

        float x_square = x * x;

        x = ((((((x_square * -2.39f * pow(10.f, -8.f) + 2.7526f * pow(10.f, -6.f)) * x_square + (0.198409f * pow(10.f, -3.f)))
                * x_square + (8.33333f * pow(10.f, -3.f))) * x_square + (-0.166667f)) * x_square) + 1.f) * x;

        return x;
    }

    /*as applied in Reaktor --- befindet sich hier vorübergehend*/
    float reakCos(float x){

        float x_square = x * x;

        x = (((((x_square * -2.605f * pow(10.f, -7.f) + 2.47609 * pow(10.f, -5.f)) * x_square + (-0.00138884)) * x_square + 0.0416666)
              * x_square + (-0.499923)) * x_square) + 1.f;

        return x;
    }
};
