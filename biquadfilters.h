#pragma once
#include "math.h"
#include "tools.h"
enum class BiquadFilterPasstype : int
{
    lowpass = 0,
    highpass = 1,
    lowshelf = 2,
    highshelf = 3
};

class BiquadFilters
{
public:
    BiquadFilters(float _sRate, float _cutFreq = 22000.f, float _shelfAmp = 0.f,
                  float _resonance = 0.5f, BiquadFilterPasstype _passtype = BiquadFilterPasstype::lowpass)           // Constructor for static a biquad Filter
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
        setCutFreq(_cutFreq, _resonance, _passtype);
        setShelfAmp(_shelfAmp, _passtype);
    }

    ~BiquadFilters(){}

    void setCutFreq(float _cutFreq, float _resonance, BiquadFilterPasstype _passtype)
    {
        cutFreq = _cutFreq;
        resonance = _resonance;

        /*Check for Frequency clipping*/
        if (cutFreq < (sRate / 24000.f))
        {
            cutFreq = sRate / 24000.f;
        }

        else if (cutFreq > (sRate / 2.18f))
        {
            cutFreq = sRate / 2.18f;
        }

        /*check for Resonance clipping*/
        if (resonance > 0.999f)
        {
            resonance = 0.999f;
        }

        else if (resonance < -0.999f)
        {
            resonance = -0.999f;
        }

        cutFreq *= (2.f * M_PI / sRate);        // Omega

        //cutFreq_cos = cos(cutFreq);
        cutFreq_cos = reakCos(cutFreq);

        //alpha = sin(cutFreq) * (1.f - resonance);
        alpha = reakSin(cutFreq) * (1.f - resonance);

        setupBquadFilter(_passtype);
#if 0
        switch(_passtype)                        //check which Filter is active and update coefficients
        {
            case BiquadFilterPasstype::lowpass:
            setupBiquadLowpass();
            break;

            case BiquadFilterPasstype::highpass:
            setupBiquadHighpass();
            break;

            case BiquadFilterPasstype::highshelf:
            setupBiquadHighshelf();
            break;

            case BiquadFilterPasstype::lowshelf:
            setupBiquadLowshelf();
            break;
        }
#endif
    }

    void setShelfAmp(float _shelfAmp, BiquadFilterPasstype _passtype)
    {
        shelfAmp = _shelfAmp;
        shelfAmp = pow(10.f, (shelfAmp / 40.f));

        setupBquadFilter(_passtype);
#if 0
        switch(_passtype)                        //check which Filter is active and update coefficients
        {
            case BiquadFilterPasstype::highshelf:
            setupBiquadHighshelf();
            break;

            case BiquadFilterPasstype::lowshelf:
            setupBiquadLowshelf();
            break;

            case BiquadFilterPasstype::highpass:
            break;

            case BiquadFilterPasstype::lowpass:
            break;
        }
#endif
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

    float cutFreq;
    float cutFreq_cos;
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


    void setupBquadFilter(BiquadFilterPasstype _passtype)
    {
        switch(_passtype)
        {
            case BiquadFilterPasstype::lowpass:
            setupBiquadLowpass();
            break;

            case BiquadFilterPasstype::highpass:
            setupBiquadHighpass();
            break;

            case BiquadFilterPasstype::highshelf:
            setupBiquadHighshelf();
            break;

            case BiquadFilterPasstype::lowshelf:
            setupBiquadLowshelf();
            break;
        }
    }

    void setupBiquadLowpass()
    {
        a0 = 1 + alpha;
        a1 = (cutFreq_cos * -2.f);
        a2 = 1 - alpha;
        b0 = (1 - cutFreq_cos) / 2.f;
        b1 = 1 - cutFreq_cos;
        b2 = b0;
    }

    void setupBiquadHighpass()
    {
        a0 = 1 + alpha;
        a1 = cutFreq_cos * -2.f;
        a2 = 1 - alpha;
        b0 = (1 + cutFreq_cos) / 2.f;
        b1 = (1 + cutFreq_cos) * -1.f;
        b2 = b0;
    }

    void setupBiquadLowshelf()
    {
        a0 = (shelfAmp + 1.f) + (cutFreq_cos * (shelfAmp - 1.f)) + (2.f * sqrt(shelfAmp) * alpha);
        a1 = ((shelfAmp - 1.f) + (cutFreq_cos * (shelfAmp + 1.f))) * -2.f;
        a2 = (shelfAmp + 1.f) + (cutFreq_cos * (shelfAmp - 1.f)) - (2.f * sqrt(shelfAmp) * alpha);
        b0 = ((shelfAmp + 1.f) - (cutFreq_cos * (shelfAmp - 1.f)) + (2.f * sqrt(shelfAmp) * alpha)) * shelfAmp;
        b1 = ((shelfAmp - 1.f) - (cutFreq_cos * (shelfAmp + 1.f))) * 2.f * shelfAmp;
        b2 = ((shelfAmp + 1.f) - (cutFreq_cos * (shelfAmp - 1.f)) - (2.f * sqrt(shelfAmp) * alpha)) * shelfAmp;
    }

    void setupBiquadHighshelf()
    {
        a0 = (shelfAmp + 1.f) - (cutFreq_cos * (shelfAmp - 1.f)) + (2.f * sqrt(shelfAmp) * alpha);
        a1 = ((shelfAmp - 1.f) - (cutFreq_cos * (shelfAmp + 1.f))) * 2.f;
        a2 = (shelfAmp + 1.f) - (cutFreq_cos * (shelfAmp - 1.f)) - (2.f * sqrt(shelfAmp) * alpha);
        b0 = ((shelfAmp + 1.f) + (cutFreq_cos * (shelfAmp - 1.f)) + (2.f * sqrt(shelfAmp) * alpha)) * shelfAmp;
        b1 = ((shelfAmp - 1.f) + (cutFreq_cos * (shelfAmp + 1.f))) * -2.f * shelfAmp;
        b2 = ((shelfAmp + 1.f) + (cutFreq_cos * (shelfAmp - 1.f)) - (2.f * sqrt(shelfAmp) * alpha)) * shelfAmp;
    }

    /*as applied in Reaktor*/
    float reakSin(float x){

        float x_square = x * x;

        x = ((((((x_square * -2.39f * pow(10.f, -8.f) + 2.7526f * pow(10.f, -6.f)) * x_square + (0.198409f * pow(10.f, -3.f)))
                * x_square + (8.33333f * pow(10.f, -3.f))) * x_square + (-0.166667f)) * x_square) + 1.f) * x;

        return x;
    }

    /*as applied in Reaktor*/
    float reakCos(float x){

        float x_square = x * x;

        x = (((((x_square * -2.605f * pow(10.f, -7.f) + 2.47609 * pow(10.f, -5.f)) * x_square + (-0.00138884)) * x_square + 0.0416666)
              * x_square + (-0.499923)) * x_square) + 1.f;

        return x;
    }
};
