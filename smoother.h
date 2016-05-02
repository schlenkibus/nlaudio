/**
 * @file       smoother.h
 * @date       2016-03-18
 * @brief      this class is an implementation of a simple smoothing function
 * @author     Anton Schmied [date of file creation 2016-03-18]
*/

#pragma once

class Smoother
{
public:
	/** Smoother Constructor with initial values */
    Smoother(int _sRate,
             float _smTime)
        :base(0.f)
        ,diff(0.f)
        ,ramp(1.f)
        ,heldValue(0.f)
    {
        setInc(_sRate, _smTime);
    }

    ~Smoother(){}

	/** @brief    Initializes values, if for example a fader has been moved
     *  @param    current Value which will be the maximum/ minimum point of the curve
    */
    void initSmoother(float currValue) 
    {
        base = heldValue;
        diff = currValue - heldValue;
        ramp = 0.f;
    }

	/** @brief    increments the base value by a prespecified step
     *  @return   incremented value, depending on the ramp value
    */
    float smooth()                              
    {
        if (ramp < 1.f)
        {
            ramp += inc;
        }
        else
            ramp = 1.f;

        heldValue = base + diff * ramp;

        return heldValue;
    }

private:
    float base;				/**< starting value of the smoothing curve*/
    float diff;				/**< difference between the starting and ending points of the curve */
    float ramp;				/**< ramp value, which is incremented with every step or is 1 if the curve is not applied */
    float heldValue;		/**< incremented value with every step */
    float inc;				/**< increment value, which is constant once set */

	/** @brief    sets the increment value
     *  @param    samplingrate
	 *  @param    length of the smoothing curve in ms
    */
    void setInc(int sRate, float smTime)
    {
        inc = 5.f / (static_cast<float>(sRate) * smTime);
    }
};
