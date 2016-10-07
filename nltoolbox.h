#pragma once

#include <cmath>
#include "tools.h"


namespace NlToolbox {

/*****************************************************************************/
/** @brief    Crossfade Tools
******************************************************************************/

namespace Crossfades {

/*****************************************************************************/
/** @brief    mix calculation between the raw and processed sample
 *  @param    raw Sample
 *  @param    processed sample
 *  @param    dry Amount
 *  @param    wet Amount
 *  @return   a mix of both samples
******************************************************************************/

inline float crossFade(float _sample1, float _sample2, float _gain1, float _gain2)
{
    return ((_sample1 * _gain1) + (_sample2 * _gain2));
}



/*****************************************************************************/
/** @brief    bipolar mix calculation between two samples
 *  @param    first sample
 *  @param    second sample
 *  @param    mix amount
 *  @return   a mix of both samples
******************************************************************************/

inline float bipolarCrossFade(float _sample1, float _sample2, float _mix)
{
    _sample1 = (1.f - fabs(_mix)) * _sample1;            // kann fabs() anders gerechnet werden?
    _sample2 = _mix * _sample2;

    return _sample1 + _sample2;

}

} // namespcace Crossfades




/*****************************************************************************/
/** @brief    Math Tools
******************************************************************************/

namespace Math {

/*****************************************************************************/
/** @brief    sine calculation of an incoming value
 *  @param    value
 *  @return   sine value
******************************************************************************/

inline float sin(float x)
{
    float x_square = x * x;

    x = (((((x_square * -2.39f * pow(10.f, -8.f) + 2.7526f * pow(10.f, -6.f))
            * x_square + (-0.000198409f))
           * x_square + 0.00833333f)
          * x_square + (-0.166667f))
         * x_square + 1.f) * x;

    return x;
}



/*****************************************************************************/
/** @brief    cosine calculation of an incoming value
 *  @param    value
 *  @return   cosine value
******************************************************************************/

inline float cos(float x)
{
    float x_square = x * x;

    x = (((((x_square * -2.605f * pow(10.f, -7.f) + 2.47609 * pow(10.f, -5.f))
            * x_square + (-0.00138884))
           * x_square + 0.0416666)
          * x_square + (-0.499923))
         * x_square) + 1.f;

    return x;
}



/*****************************************************************************/
/** @brief    tangent calculation of an incoming value
 *  @param    value
 *  @return   tangent value
******************************************************************************/

inline float tan(float x)
{
    x = 0.133333 * pow(x, 5.f) + 0.333333 * pow(x, 3.f) + x;

    return x;
}




/*****************************************************************************/
/** @brief    sine calculation of an incoming value - 3rd degree polynomial
 *  @param    value
 *  @return   sine value
******************************************************************************/

inline float sinP3(float x)
{
    x += -0.25f;

    float x_round = round(x);
    x -= x_round;

    x += x;
    x = fabs(x);
    x = 0.5f - x;

    float x_square = x * x;
    x = x * ((2.26548 * x_square - 5.13274) * x_square + 3.14159);

    return x;
}



/*****************************************************************************/
/** @brief    interpolation of 4 neighbouring samples from the delay buffers
 *  @param    fractional part of the number of the delay samples
 *  @param	  sample from delay buffer with index n-1
 *  @param	  sample from delay buffer with index n
 *  @param	  sample from delay buffer with index n+1
 *  @param	  sample from delay buffer with index n+2
 *  @return	  interpolated value of the four samples
******************************************************************************/

inline float interpolRT(float fract, float sample_tm1, float sample_t0, float sample_tp1, float sample_tp2)
{
    float fract_square = fract * fract;
    float fract_cube = fract_square * fract;

    float a = 0.5f * (sample_tp1 - sample_tm1);
    float b = 0.5f * (sample_tp2 - sample_t0);
    float c = sample_t0 - sample_tp1;

    return sample_t0 + fract * a + fract_cube * (a + b + 2.f * c) - fract_square * (2.f * a + b + 3.f * c);
}

} // namespace Math



/*****************************************************************************/
/** @brief    Filter Tools
******************************************************************************/

namespace Filters {

/*****************************************************************************/
/** @brief    implementation of a 1. order Highpass at 30Hz
 *  @param    raw Sample
 *  @return   processed sample
******************************************************************************/

struct Highpass30Hz
{
    float mSampleRate;
    float mInStateVar;
    float mOmega;

    Highpass30Hz()                      // Default Constructor
        : mSampleRate(48000.f)
        , mInStateVar(0.f)
    {
        setOmega();
    }



    Highpass30Hz(int _sampleRate)      // Parameterized Constructor
        : mSampleRate(static_cast<float>(_sampleRate))
        , mInStateVar(0.f)
    {
        setOmega();
    }


    void setOmega()
    {
        mOmega = 30.f * (2.f * M_PI / mSampleRate);

        if (mOmega > 0.8f)
        {
            mOmega = 0.8f;
        }
    }

    float applyFilter(float _sample)
    {
        float output;

        output = _sample - mInStateVar;
        mInStateVar = output * mOmega + mInStateVar;

        return output;
    }
};



/*****************************************************************************/
/** @brief    implementation of a 1. order Lowpass at 2 Hz
 *  @param    raw Sample
 *  @return   processed sample
******************************************************************************/

struct Lowpass2Hz
{
    float mSampleRate;
    float mStateVar;
    float mOmega;

    Lowpass2Hz()                        // Default Constructor
        : mSampleRate(48000.f)
        , mStateVar(0.f)
    {
        setOmega();
    }


    Lowpass2Hz(int _sampleRate)         // Parameterized Constructor
        : mSampleRate(static_cast<float>(_sampleRate))
        , mStateVar(0.f)
    {
        setOmega();
    }


    void setOmega()
    {
        mOmega = 2.f * (2.f * M_PI / mSampleRate);

        if (mOmega > 1.9f)               //Clip
        {
            mOmega = 1.9f;
        }
    }

    float applyFilter(float _sample)
    {
        float output;

        output = _sample - mStateVar;
        output = output * mOmega + mStateVar;

        mStateVar = output;

        return output;
    }
};



/*****************************************************************************/
/** @brief    implementation of a Chirp Filter
 *  @param    raw Sample
 *  @return   processed sample
******************************************************************************/

struct ChirpFilter
{
    float mSampleRate;
    float mStateVar;
    float mOmega;
    float mA0, mA1;


    ChirpFilter()                       // Default constructor
        : mSampleRate(48000.f)
        , mStateVar(0.f)
    {
        setFrequency(7677.f);
    }



    ChirpFilter(int _sampleRate, float _cutFrequency)       // Parameterized Constructor
        : mSampleRate(static_cast<float>(_sampleRate))
        , mStateVar(0.f)
    {
        setFrequency(_cutFrequency);
    }



    void setFrequency(float _chirpFrequency)
    {
        mOmega = _chirpFrequency * (M_PI / mSampleRate);
        mOmega = NlToolbox::Math::tan(mOmega);

        mA0 = 1.f / (mOmega + 1.f);
        mA1 = mOmega - 1.f;
    }



    float applyFilter(float _sample)
    {
        float output = 0.f;
        float tmpVar = 0.f;

        output = _sample - (mA1 * mStateVar);       // IIR
        output *= mA0;

        tmpVar = output;

        output = (output + mStateVar) * mOmega;     // FIR
        mStateVar = tmpVar;

        return output;
    }
};

} // namespace Filters


/*****************************************************************************/
/** @brief    Frequently used conversion functions
******************************************************************************/

namespace Conversion {

/*****************************************************************************/
/** @brief    conversion from a value in dB in to an amplification factor
 *  @param    value in dB
 *  @return   processed sample
******************************************************************************/

inline float db2af(float dbIn)
{
    return pow(1.12202f, dbIn);
}



/*****************************************************************************/
/** @brief    conversion from pitch to frequency
 *  @param    pitch value
 *  @return   frequency value
******************************************************************************/

inline float pitch2freq(float pitch)
{
    return pow(2.f, (pitch - 69.f)/ 12.f) * 440.f;

}

} // namespace Conversion



/*****************************************************************************/
/** @brief    Other Tools (might move to a more specific namespaces)
******************************************************************************/

namespace Others {

/*****************************************************************************/
/** @brief
 *  @param
 *  @param
 *  @param
 *  @return
******************************************************************************/

inline float threeRanges(float sample, float ctrlSample, float foldAmnt)
{
    if(ctrlSample < -0.25f)
    {
        sample = (sample + 1.f) * foldAmnt + (-1.f);
    }
    else if(ctrlSample > 0.25f)
    {
        sample = (sample + (-1.f)) * foldAmnt + 1.f;
    }

    return sample;
}



/*****************************************************************************/
/** @brief
 *  @param
 *  @param
 *  @param
 *  @return
******************************************************************************/

inline float parAsym(float sample, float sample_square, float asymAmnt)
{

    return ((1.f - asymAmnt) * sample) + (2 * asymAmnt * sample_square);
}

} // namespace Others

} // Namespace NlToolbox
