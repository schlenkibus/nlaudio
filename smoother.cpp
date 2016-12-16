/******************************************************************************/
/** @file		smoother.cpp
    @date		2016-08-04
    @version	0.1
    @author		Anton Schmied
    @brief		Smoother Class member and method definitions
*******************************************************************************/

#include "smoother.h"

/******************************************************************************/
/** Smoother Default Constructor
 * @brief    initialization of the modules local variabels with default
 *           parameters
 *           Samplerate:        48000 Hz
 *           smoothing time:    0.032 s
*******************************************************************************/

Smoother::Smoother()
    : mTarget(0.f)
    , mBase(0.f)
    , mDiff(0.f)
    , mRamp(1.f)
    , mHeldValue(0.f)
{
    setInc(48000, 0.032f);
}



/******************************************************************************/
/** Smoother Parameterized Constructor
 * @brief    initialization of the modules local variabels with custom
 *           parameters
*******************************************************************************/

Smoother::Smoother(uint32_t _sRate, float _smTime)
    : mTarget(0.f)
    , mBase(0.f)
    , mDiff(0.f)
    , mRamp(1.f)
    , mHeldValue(0.f)
{
    setInc(_sRate, _smTime);
}



/******************************************************************************/
/** @brief    Initializes values, if for example a fader has been moved
 *  @param    current Value which will be the maximum/ minimum point of the curvee
*******************************************************************************/

void Smoother::initSmoother(float _currValue)
{
    mTarget = _currValue;
    mBase = mHeldValue;
    mDiff = mTarget - mBase;
    mRamp = 0.f;
}



/******************************************************************************/
/** @brief    increments the base value by a prespecified step
 *  @return   incremented value, depending on the ramp value
*******************************************************************************/

float Smoother::smooth()
{
    if (mRamp < 1.f)
    {
        mRamp += mInc;
        mHeldValue = mBase + mDiff * mRamp;
    }
    else
    {
        mHeldValue = mTarget;
    }

    return mHeldValue;
}



/******************************************************************************/
/** @brief    sets the increment value
 *  @param    samplingrate
 *  @param    length of the smoothing curve in ms
*******************************************************************************/

inline void Smoother::setInc(uint32_t _sRate, float _smTime)
{
    mInc = 5.f / (static_cast<float>(_sRate) * _smTime);
}

inline void Smoother::setInc()
{
    mInc = mDiff / (mSmoothingTime * mSampleRate);
}











