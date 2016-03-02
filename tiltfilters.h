#pragma once
#include "math.h"

enum class TiltFilterPasstype : int
{
    lowpass = 0,
    highpass = 1,
    lowshelf = 2,
    highshelf = 3
};

class TiltFilters
{
public:
    TiltFilters(float _sRate, float _cutFreq = 22000.f, float _tilt = -12.f, float _slopeWidth = 2.f,
                  float _resonance = 0.5f, TiltFilterPasstype _passtype = TiltFilterPasstype::lowshelf)           // Constructor for static a Tilt Filter
        : sRate(_sRate)
        , tilt(_tilt)
        , slopeWidth(_slopeWidth)   //no min clipping, since it's hardcoded for now
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
        setTilt(_tilt, _passtype);
    }

    ~TiltFilters(){}

    void setCutFreq(float _cutFreq, float _resonance, TiltFilterPasstype _passtype)
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

        setAlpha();
        //alpha = sin(cutFreq) * (1.f - resonance);
        //alpha = reakSin(cutFreq) * (1.f - resonance);

        setupBquadFilter(_passtype);
    }

    void setTilt(float _tilt, TiltFilterPasstype _passtype)
    {
        tilt = _tilt;
        tilt = pow(10.f, (tilt / 40.f));

        setAlpha();

        setupBquadFilter(_passtype);
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

    float sRate;
    float cutFreq;
    float cutFreq_cos;
    float alpha;
    float tilt;
    float slopeWidth;
    float resonance;

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


    void setupBquadFilter(TiltFilterPasstype _passtype)
    {
        switch(_passtype)
        {
            case TiltFilterPasstype::lowpass:
            setupTiltLowpass();
            break;

            case TiltFilterPasstype::highpass:
            setupTiltHighpass();
            break;

            case TiltFilterPasstype::highshelf:
            setupTiltHighshelf();
            break;

            case TiltFilterPasstype::lowshelf:
            setupTiltLowshelf();
            break;
        }
    }

    void setupTiltLowpass()
    {
        a0 = 1 + alpha;
        a1 = (cutFreq_cos * -2.f);
        a2 = 1 - alpha;
        b0 = (1 - cutFreq_cos) / 2.f;
        b1 = 1 - cutFreq_cos;
        b2 = b0;
    }

    void setupTiltHighpass()
    {
        a0 = 1 + alpha;
        a1 = cutFreq_cos * -2.f;
        a2 = 1 - alpha;
        b0 = (1 + cutFreq_cos) / 2.f;
        b1 = (1 + cutFreq_cos) * -1.f;
        b2 = b0;
    }

    void setupTiltLowshelf()
    {
        a0 = (tilt + 1.f) + (cutFreq_cos * (tilt - 1.f)) + (2.f * sqrt(tilt) * alpha);
        a1 = ((tilt - 1.f) + (cutFreq_cos * (tilt + 1.f))) * -2.f;
        a2 = (tilt + 1.f) + (cutFreq_cos * (tilt - 1.f)) - (2.f * sqrt(tilt) * alpha);
        b0 = ((tilt + 1.f) - (cutFreq_cos * (tilt - 1.f)) + (2.f * sqrt(tilt) * alpha)) * tilt;
        b1 = ((tilt - 1.f) - (cutFreq_cos * (tilt + 1.f))) * 2.f * tilt;
        b2 = ((tilt + 1.f) - (cutFreq_cos * (tilt - 1.f)) - (2.f * sqrt(tilt) * alpha)) * tilt;
    }

    void setupTiltHighshelf()
    {
        a0 = (tilt + 1.f) - (cutFreq_cos * (tilt - 1.f)) + (2.f * sqrt(tilt) * alpha);
        a1 = ((tilt - 1.f) - (cutFreq_cos * (tilt + 1.f))) * 2.f;
        a2 = (tilt + 1.f) - (cutFreq_cos * (tilt - 1.f)) - (2.f * sqrt(tilt) * alpha);
        b0 = ((tilt + 1.f) + (cutFreq_cos * (tilt - 1.f)) + (2.f * sqrt(tilt) * alpha)) * tilt;
        b1 = ((tilt - 1.f) + (cutFreq_cos * (tilt + 1.f))) * -2.f * tilt;
        b2 = ((tilt + 1.f) + (cutFreq_cos * (tilt - 1.f)) - (2.f * sqrt(tilt) * alpha)) * tilt;
    }

    void setAlpha()
    {
        alpha *= (tilt + (1.f / tilt)) * (slopeWidth - 1.f);
        alpha = sqrt(alpha + 2.f) * sin(cutFreq) * (1.f - resonance);
        // alternative: alpha = sqrt(alpha + 2.f) * reakSin(cutFreq) * (1.f - resonance);
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
