/******************************************************************************/
/** @file		nltoolbox.h
    @date		2018-06-27
    @version	0.1
    @authors    Anton Schmied[2017-03-16], Matthias Seeber[2018-06-27]
    @brief		A collection of frequently used function for crossfading,
                conversion, etc.
*******************************************************************************/

#pragma once

#include <cmath>
#include <common/tools.h>
#include "nlglobaldefines.h"


namespace NlToolbox {

/*****************************************************************************/
/** @brief    Clipping Tools
 *  @param    value (normalized, abs(value) <= 1.0)
 *  @return   clipped value (currently either unipolar or bipolar)
******************************************************************************/

namespace Clipping {

/* handle clipping of single floats (fmin, fmax are of type double) */

/* return the biggest of two floats */
inline float floatMax(float _x, float _y)
{
    if(_x > _y)
    {
        return _x;
    }
    else
    {
        return _y;
    }
}

/* return the smallest of two floats */
inline float floatMin(float _x, float _y)
{
    if(_x < _y)
    {
        return _x;
    }
    else
    {
        return _y;
    }
}

/* clip a float in the normalized unipolar interval [0 ... 1] */
inline float uniNorm(float _value)
{
    return (floatMax(0.f, floatMin(1.f, _value)));
}

/* clip a float in the normalized bipolar interval [-1 ... 1] */
inline float biNorm(float _value)
{
    return (floatMax(-1.f, floatMin(1.f, _value)));
}

} // namespace Clipping

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



/*****************************************************************************/
/** @brief    unipolar mix calculation between two samples
 *  @param    first sample
 *  @param    second sample
 *  @param    mix amount
 *  @return   a mix of both samples
******************************************************************************/

inline float unipolarCrossFade(float _sample1, float _sample2, float _mix)
{
    _sample1 = (1.f - _mix) * _sample1;
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

    x = (((((x_square * -2.39e-8 + 2.7526e-6)
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

    x = (((((x_square * -2.605e-7 + 2.47609e-5)
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
    x = 0.133333f * pow(x, 5.f) + 0.333333f * pow(x, 3.f) + x;

    return x;
}




/*****************************************************************************/
/** @brief    arctangent calculation of an incoming value
 *  @param    value
 *  @return   arctangent value
******************************************************************************/

inline float arctan(float x)
{
    if(x > 1.f)
    {
        x = 1.f / x;
        float x_square = x * x;

        x = (((((((( x_square * 0.00286623f - 0.0161857f) * x_square + 0.0429096f)
                  * x_square - 0.0752896f)  * x_square + 0.106563f)
                * x_square - 0.142089f) * x_square + 0.199936f)
              * x_square - 0.333331f) * x_square + 1.f) * x;

        x  = 1.5708f - x;
    }
    else if(x < -1.f)
    {
        x = 1.f / x;
        float x_square = x * x;

        x = (((((((( x_square * 0.00286623f - 0.0161857f) * x_square + 0.0429096f)
                  * x_square - 0.0752896f)  * x_square + 0.106563f)
                * x_square - 0.142089f) * x_square + 0.199936f)
              * x_square - 0.333331f) * x_square + 1.f) * x;

        x  = -1.5708f - x;

    }
    else
    {
        float x_square = x * x;

        x = (((((((( x_square * 0.00286623f - 0.0161857f) * x_square + 0.0429096f)
                  * x_square - 0.0752896f)  * x_square + 0.106563f)
                * x_square - 0.142089f) * x_square + 0.199936f)
              * x_square - 0.333331f) * x_square + 1.f) * x;
    }

    return x;
}



/*****************************************************************************/
/** @brief    sine calculation of an incoming value - 3rd degree polynomial
 *  @param    value
 *  @return   sine value
******************************************************************************/

inline float sinP3_wrap(float _x)
{
    _x += -0.25f;

//    _x = _x - round(_x);

    if (_x >= 0.f)
    {
        _x -= static_cast<int>(_x + 0.5f);
    }
    else
    {
        _x -= static_cast<int>(_x - 0.5f);
    }

    _x += _x;
    _x = fabs(_x);
    _x = 0.5f - _x;

    float x_square = _x * _x;
    return _x * ((2.26548 * x_square - 5.13274) * x_square + 3.14159);
}



/*****************************************************************************/
/** @brief    sine calculation of an incoming value - 3rd degree polynomial
 *            with no warping
 *  @param    value
 *  @return   sine value
******************************************************************************/

inline float sinP3_noWrap(float _x)
{
    _x += _x;
    _x = fabs(_x);
    _x = 0.5f - _x;

    float x_square = _x * _x;
    return _x * ((2.26548 * x_square - 5.13274) * x_square + 3.14159);
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



/*****************************************************************************/
/** @brief    bell function with y = 0 at x = 0 & 1 and y = 1 at x = 0.5,
 *            expontential  when x < 0 | x > 1
 *  @param    x
 *  @return	  y
******************************************************************************/
inline float bell(float _x)
{
    _x = fabs(_x - 0.5f) * 4.f - 1.f;
    return (2.f - fabs(_x)) * _x * -0.5f + 0.5f;
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



    Highpass30Hz(uint32_t _sampleRate)      // Parameterized Constructor
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
        mInStateVar = (output * mOmega + mInStateVar) + DNC_CONST;

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


    Lowpass2Hz(uint32_t _sampleRate)         // Parameterized Constructor
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

        mStateVar = output + DNC_CONST;

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



    ChirpFilter(uint32_t _sampleRate, float _cutFrequency)       // Parameterized Constructor
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
        mStateVar = tmpVar + DNC_CONST;

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
 *  @return   value as amplitude factor
******************************************************************************/

inline float db2af(float dbIn)
{
    return pow(1.12202f, dbIn);
}



/*****************************************************************************/
/** @brief    conversion from a value as amplitude factor into dB
 *  @param    value as amplitude factor
 *  @return   value in dB
******************************************************************************/

inline float af2db(float afIn)
{
    if (afIn == 0.f)                        // vorsichthalber gegen ne = schützen
        afIn = 1e15;

    return 20.f * log10(afIn);      // gibt es eine feine annäherung?
}



/*****************************************************************************/
/** @brief    conversion from pitch to frequency
 *  @param    pitch value
 *  @return   frequency value
******************************************************************************/

inline float pitch2freq(float pitch)
{
    /// Beide der filgenden Ansätze bedienen sich der pow()- Funtkion
    /// Zur Optimierung sollen diese mit einem Polynom oder
    /// einem Tabellen-Ansatz ersetzt werden!

    return pow(2.f, (pitch - 69.f)/ 12.f) * 440.f;

//    return pow(1.05946309f, pitch) * 8.17579892f;         // alternative
}



/*****************************************************************************/
/** @brief    conversion from float to int using casting within an expression
 *  @param    float value
 *  @return   int value
******************************************************************************/

inline int float2int(float _value)
{
    /// cmath approach
//    return round(_value);

    /// Statement approach
//    return (_value >= 0.f)
//           ? static_cast<int>(_value + 0.5f)
//           : static_cast<int>(_value - 0.5f);

    /// Expression approach
    if (_value >= 0.f)
    {
        return static_cast<int>(_value + 0.5f);
    }
    else
    {
        return static_cast<int>(_value - 0.5f);
    }
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



/*****************************************************************************/
/** @brief  Curve tools
******************************************************************************/

namespace Curves {

/*****************************************************************************/
/** @brief    Control shaper for the range from -1 to +1. It increases the
 *            resolution near to the range limits
 *  @param    value
 *  @return   resulting value
******************************************************************************/

inline float applySineCurve(float _in)
{
    _in = (_in * 2.f) + (-1.f);

    return _in * _in * _in * (-0.25f) + (_in * 0.75f) + 0.5;
}



/*****************************************************************************/
/** @brief    Shaper for control signals. One breakpoint.
 *            [0], [50] and [100] adjust the levels for [in] = 0,
 *            [in] = 0.5(breakpoint) and [in] = 1.
 *            Matthias: renamed object, fixed bug for values (0.5 ... 1.0)
******************************************************************************/

struct Shaper_1_BP
{
    float m_Factor1, m_Factor2;
    float m_Startpoint, m_Breakpoint, m_Endpoint;
    float m_Split = 1.f / 2.f;

    void setCurve(float _startpoint, float _breakpoint, float _endpoint)
    {
        m_Factor1 = (_breakpoint - _startpoint) / (m_Split - 0.f);
        m_Factor2 = (_endpoint - _breakpoint) / (1.f - m_Split);

        m_Startpoint = _startpoint;
        m_Breakpoint = _breakpoint;
        m_Endpoint   = _endpoint;
    }

    float applyCurve(float _in)
    {
        float out;

        if (_in <= m_Split)
        {
            out = (_in * m_Factor1) + m_Startpoint;
        }
        else
        {
            out = ((_in - m_Split) * m_Factor2) + m_Breakpoint;
        }

        return out;
    }
};

/*****************************************************************************/
/** @brief    Shaper for control signals. Two breakpoints.
 *            [0], [33.3], [66.6] and [100]
 *            (intended for SVF Resonance Post Processing)
******************************************************************************/

struct Shaper_2_BP
{
    float m_Factor1, m_Factor2, m_Factor3;
    float m_Startpoint, m_Breakpoint1, m_Breakpoint2, m_Endpoint;
    float m_Split1 = 1.f / 3.f;
    float m_Split2 = 2.f / 3.f;

    void setCurve(float _startpoint, float _breakpoint1, float _breakpoint2, float _endpoint)
    {
        m_Factor1 = (_breakpoint1 - _startpoint) / (m_Split1 - 0.f);
        m_Factor2 = (_breakpoint2 - _breakpoint1) / (m_Split2 - m_Split1);
        m_Factor3 = (_endpoint - _breakpoint2) / (1.f - m_Split2);

        m_Startpoint = _startpoint;
        m_Breakpoint1 = _breakpoint1;
        m_Breakpoint2 = _breakpoint2;
        m_Endpoint = _endpoint;
    }

    float applyCurve(float _in)
    {
        float out;

        if(_in <= m_Split1)
        {
            out = (_in * m_Factor1) + m_Startpoint;
        }
        else if(_in <= m_Split2)
        {
            out = ((_in - m_Split1) * m_Factor2) + m_Breakpoint1;
        }
        else
        {
            out = ((_in - m_Split2) * m_Factor3) + m_Breakpoint2;
        }

        return out;
    }
};

/*****************************************************************************/
/** @brief      Squared Curvature Evaluation
 *              (usable for polynomial transitions of TCD renderers)
 *  @param      value (representing transition progress) [0 ... 1]
 *              (can also be bipolar [-1 ... 1])
 *  @param      curvature (affecting the transition slope) [-1 ... 1]
 *  @return     resulting value
 *              (depending on the curvature argument, the transformation
 *              shapes incoming values to be within the following ranges:
 *              (1 - (1 - value)^2),    (curvature = -1, decreasing slope)
 *              (value),                (curvature = 0, constant slope)
 *              (value^2),              (curvature = 1, increasing slope)
******************************************************************************/

inline float SquaredCurvature(const float _value, const float _curvature)
{
    return(_value * (1.f + (_curvature * (fabs(_value) - 1.f))));
}

} // Namespace Curves

} // Namespace NlToolbox
