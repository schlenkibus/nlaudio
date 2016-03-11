#pragma once

#include "math.h"
#include "biquadfilters.h"
#include "tiltfilters.h"
#include "smoother.h"

class Cabinet
{
public:

    Cabinet(int _sRate,
            float _cabLvl = -14.f,
            float _drive = 20.f,
            float _hiCut = 4700.f,
            float _loCut = 61.f,
            float _tilt = -12.f)
    :sRate(static_cast<float>(_sRate))
    ,mix(0.f)
    ,fold(0.25f)
    ,asym(0.1f)
    ,lowpass1(_sRate, _hiCut)
    ,lowpass2(_sRate, _hiCut * 1.333f)
    ,highpass1(_sRate, _loCut, 0.f, 0.5, BiquadFiltertype::highpass)
    ,lowshelf1(_sRate, 1200.f, _tilt, 2.f, 0.5f, TiltFiltertype::lowshelf)
    ,lowshelf2(_sRate, 1200.f, _tilt * (-1.f), 2.f, 0.5f, TiltFiltertype::lowshelf)
    ,drySmoother(_sRate, 0.032f)
    ,wetSmoother(_sRate, 0.032f)
    ,driveSmoother(_sRate, 0.032f)
    ,inCh1Delay(0.f)
    ,inCh2Delay(0.f)
    {
        setDrive(_drive);
        setCabLvl(_cabLvl);
        setMix(mix);
        setSatL(_tilt);
    }

    ~Cabinet(){}

    float applyCab(float currSample, unsigned int channelIndex)         //Signal processing
    {
        //apply smoothers
        dry = drySmoother.smooth();
        wet = wetSmoother.smooth();
        drive = driveSmoother.smooth();

        float output;

        // apply drive
        output = currSample * drive;

        // apply biquad highpass filter
        output = highpass1.applyFilter(output, channelIndex);

        // apply first biquad tilt lowshelf filters
        output = lowshelf1.applyFilter(output, channelIndex);

        // apply hypSinSat
        output = hypSinSatL(output, channelIndex);

        // apply second biquad tilt lowshelf filters
        output = lowshelf2.applyFilter(output, channelIndex);

        // apply biquad lowpass filters
        output = lowpass1.applyFilter(output, channelIndex);
        output = lowpass2.applyFilter(output, channelIndex);

        // apply effect amount
        output = xFade(currSample, output);
        output = xFade(currSample, output);

        return output;
    }

    float hypSinSatL(float currSample, unsigned int channelIndex)
    {
        currSample *= (0.1588f / satL);

        float ctrl = currSample;
        currSample = sinP3(currSample);                             //sin p3

        if(ctrl < -0.25f)                                            //3 Ranges
        {
            currSample = (currSample + 1.f) * fold + (-1.f);
        }
        else if(ctrl > 0.25f)
        {
            currSample = (currSample + (-1.f)) * fold + 1.f;
        }

        currSample = ((1.f - asym) * currSample) + (hp1(currSample * currSample, channelIndex) * asym * 2); //par asym

        currSample *= satL;

        return currSample;
    }

    inline float xFade(float sampleIn1, float sampleIn2)            //xFade Function, input1 - raw input, input2 - processed input
    {
        return ((sampleIn1 * dry) + (sampleIn2 * wet));
    }

    inline void setFold(float _fold)        //set FOLD amount
    {
        fold = _fold;
    }

    inline void setAsym(float _asym)         //set ASYM amount
    {
        asym = _asym;
    }

    inline void setSatL(float _tilt)        //set SETL amount
    {
        satL = db2af(0.5f * _tilt);
    }

    inline void setMix(float _mix)          //set MIX amount
    {
        mix = _mix;
        wet = mix * cabLvl;
        dry = 1.f - mix;

        drySmoother.initSmoother(dry);
        wetSmoother.initSmoother(wet);
    }

    void setCabLvl(float _cabLvl)           //set CABLVL
    {
        cabLvl = db2af(_cabLvl);
        setMix(mix);
    }

    void setDrive(float _drive)             //set DRIVE
    {
        drive = db2af(_drive);
        driveSmoother.initSmoother(drive);
    }

    void setHiCut(float _hiCut)             //set HICUT
    {
        lowpass1.setCutFreq(_hiCut);
        lowpass2.setCutFreq(_hiCut * 1.333f);
    }

    void setLoCut(float _loCut)             //set LOCUT
    {
        highpass1.setCutFreq(_loCut);
    }

    void setTilt(float _tilt)               //set TILT
    {
        lowshelf1.setTilt(_tilt);
        lowshelf2.setTilt(_tilt * (-1.f));
        setSatL(_tilt);
    }

private:

    float sRate;
    float output;

    /*Cabinet Controls*/
    float drive;        // [0 .. 50] dB, default 20dB
    float cabLvl;       // [-50 .. 0] dB, default -14dB
    float mix;          // [0 .. 1], default 0.0
    float fold;         // [0 .. 1], default 0.25
    float asym;         // [0 .. 1], default 0.1

    /*Resulting Parameters*/
    float satL;
    float wet;
    float dry;

    // filter objects
    BiquadFilters lowpass1;
    BiquadFilters lowpass2;
    BiquadFilters highpass1;
    TiltFilters lowshelf1;
    TiltFilters lowshelf2;

    // Smoother
    Smoother drySmoother;
    Smoother wetSmoother;
    Smoother driveSmoother;

    //hp 1
    float inCh1Delay;
    float inCh2Delay;


    inline float db2af(float dbIn)              //helper fucntion db2af
    {
        return pow(1.12202f, dbIn);
    }

    inline float sinP3(float x)                 //
    {
        x += -0.25f;
        float x_round = round(x);
        x -= x_round;                     //Wrap - !!!check this for rounding correctly!!!

        //x = 0.5f - fabs(x + x);          //p3 sh
        x += x;
        x = fabs(x);
        x = 0.5f - x;

        float x_square = x * x;
        x = x * ((2.26548 * x_square - 5.13274) * x_square + 3.14159);       //p3

        return x;
    }

    float hp1(float sampleIN, unsigned int channelIndex)    //filter implementation ... might move to an own file
    {
        float w0 = 30.f * (2 * M_PI / sRate);               //Normalize

        if(w0 > 0.8f)                                        //Clip
            w0 = 0.8f;

        float sampleOUT;

        if (channelIndex == 0)                              //process
        {
            sampleOUT = sampleIN - inCh1Delay;

            inCh1Delay = sampleOUT * w0 + inCh1Delay;
        }

        else if (channelIndex == 1)
        {
            sampleOUT = sampleIN - inCh2Delay;

            inCh2Delay = sampleOUT * w0 + inCh2Delay;
        }

        return sampleOUT;
    }
};
