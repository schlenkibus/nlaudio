#pragma once

class Smoother
{
public:
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

    void initSmoother(float currValue)          /*Initialize values, if for example the fader has been moved*/
    {
        base = heldValue;
        diff = currValue - heldValue;
        ramp = 0.f;
    }

    float smooth()                              /*apply ramp to the value*/
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
    float base;
    float diff;
    float ramp;
    float heldValue;
    float inc;

    void setInc(int sRate, float smTime)        /*sets the increment size for the ramp*/
    {
        inc = 5.f / (static_cast<float>(sRate) * smTime);
    }
};
