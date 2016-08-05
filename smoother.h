/******************************************************************************/
/** @file		smoother.h
    @date		2016-07-01
    @version	0.2
    @author		Anton Schmied[2016-03-18]
    @brief		An implementation of Smoothing Modules
                as used in the C15 and implemented in Reaktor
*******************************************************************************/

#pragma once

class Smoother
{
public:

    Smoother();                             // Default Constructor

    Smoother(int _sRate, float _smTime);    // Parameterized Constructor

    ~Smoother(){}                           // Class Destructor

    void initSmoother(float _currValue);
    float smooth();

private:
    float mBase;				// starting value of the smoothing ramp
    float mDiff;				// difference between the starting and ending points of the curve
    float mRamp;				// ramp value, which is incremented with every step or is 1 if the curve is not applied
    float mHeldValue;           // incremented value with every step
    float mInc;                 // increment value, which is constant once set

    inline void setInc(int _sRate, float _smTime);
};
