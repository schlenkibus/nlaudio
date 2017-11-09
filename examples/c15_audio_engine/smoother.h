/******************************************************************************/
/** @file		smoother.h
    @date		2016-07-01
    @version	0.2
    @author		Anton Schmied[2016-03-18]
    @brief		An implementation of Smoothing Modules
                as used in the C15 and implemented in Reaktor
*******************************************************************************/

#pragma once
#include "nltoolbox.h"

class Smoother
{
public:

    Smoother();                             // Default Constructor

    Smoother(uint32_t _sRate, float _smTime);    // Parameterized Constructor

    ~Smoother(){}                           // Class Destructor

    void initSmoother(float _currValue);
    float smooth();

private:
    float mSampleRate;
    float mSmoothingTime;

    float mTarget;
    float mBase;				// starting value of the smoothing ramp
    float mDiff;				// difference between the starting and ending points of the curve
    float mRamp;				// ramp value, which is incremented with every step or is 1 if the curve is not applied
    float mHeldValue;           // incremented value with every step
    float mInc;                 // increment value, which is constant once set

    inline void setInc(uint32_t _sRate, float _smTime);
    inline void setInc();
};
