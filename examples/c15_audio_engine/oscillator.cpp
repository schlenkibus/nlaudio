/******************************************************************************/
/** @file		oscillator.cpp
    @date		2016-06-28
    @version    0.1
    @author		Anton Schmied[2016-07-05]
    @brief		Oscillator Class member and method definitions
*******************************************************************************/

#include "oscillator.h"

/******************************************************************************/
/** Oscillator Default Constructor
 * @brief    initialization of the modules local variabels with default values
 *           SampleRate:            48 kHz
 *           Chirp Cut Frequency:   7677 Hz
*******************************************************************************/

Oscillator::Oscillator()
{
    mSampleRate = 48000.f;
    mOscFreq = 0.f;
    mOscPhase = 0.f;
    mModPhase = 0.f;
    mPhaseStateVar = 0.f;
    mPhaseInc = 0.f;
    mFluctAmnt = 0.f;
    mRandValStateVar = 0;
    mChirpFilter = NlToolbox::Filters::ChirpFilter();
}


/******************************************************************************/
/** Oscillator Parameterized Constructor
 * @brief    initialization of the modules local variabels with predefined values
 *
*******************************************************************************/

Oscillator::Oscillator(uint32_t _samplerate,
                       float _oscFreq,
                       float _oscPhase,
                       float _modPhase,
                       float _fluctAmnt,
                       float _chirpFreq)
{
    mSampleRate = static_cast<float>(_samplerate);
    mOscFreq = _oscFreq;
    mOscPhase = _oscPhase;
    mModPhase = _modPhase;
    mPhaseStateVar = 0.f;
    mPhaseInc = 0.f;
    mFluctAmnt = _fluctAmnt;
    mRandValStateVar = 0;
    mChirpFilter = NlToolbox::Filters::ChirpFilter(_samplerate, _chirpFreq);
}


/******************************************************************************/
/** Main Oscillator Function
 *  @return sine sample
*******************************************************************************/

float Oscillator::applyOscillator()
{
    float currPhase = mOscPhase + mModPhase;

    currPhase += (-0.25f);

    currPhase -= round(currPhase);                        // Wrap

    if (fabs(mPhaseStateVar - currPhase) > 0.5f)          // Check edge
    {
        calcInc();
    }

    mPhaseStateVar = currPhase;

    mOscPhase += mPhaseInc;
    mOscPhase -= round(mOscPhase);                          // Wrap
    ///    OR
    //    if (mOscPhase > 0.5f)
    //    {
    //        mOscPhase -= 1.0f;
    //    }

    return oscSinP3(currPhase);
}


/******************************************************************************/
/** sets the oscillator frequency
 *  @param  _oscFreq
*******************************************************************************/

void Oscillator::setOscFreq(float _oscFreq)
{
    mOscFreq = _oscFreq;
}



/******************************************************************************/
/** resets the Phase if changed
 *  @param  phase [-0.5 .. 0.5]
*******************************************************************************/

void Oscillator::resetPhase(float _phase)
{
    mOscPhase = _phase;
}



/******************************************************************************/
/** sets the modulated part of the phase
 *  @param  modulation radians
*******************************************************************************/

void Oscillator::setModPhase(float _modPhase)
{
    mModPhase = mChirpFilter.applyFilter(_modPhase);
}



/******************************************************************************/
/** calculation of the increment factor
*******************************************************************************/

void Oscillator::calcInc()
{
    float modFreq = calcRandVal() * mFluctAmnt * mOscFreq;
    mPhaseInc = (mOscFreq + modFreq) / mSampleRate;
}



/******************************************************************************/
/** sets fluctuation amount
 *  @param  fluctuation amount
*******************************************************************************/

void Oscillator::setFluctuation(float _oscFluctAmnt)
{
    mFluctAmnt = _oscFluctAmnt;
}



/******************************************************************************/
/** sets the initial value of the randomValue for future calculation
 *  @param  any integer
*******************************************************************************/

void Oscillator::setSeed(uint32_t _randVal)
{
    mRandValStateVar = _randVal;
}



/******************************************************************************/
/** Calculates a random Value every half phase cycle
 *
*******************************************************************************/

float Oscillator::calcRandVal()
{
    mRandValStateVar = mRandValStateVar * 1103515245 + 12345;

    return static_cast<float>(mRandValStateVar) * 4.5657f * pow(10.f,-10.f);
}



/******************************************************************************/
/** Chirp filter frequency setup
 *  @param  Filter Cut Frequency in Hz
*******************************************************************************/

void Oscillator::setChirpFreq(float _chirpFreq)
{
    mChirpFilter.setFrequency(_chirpFreq);
}



/******************************************************************************/
/** Sine value calculation
 *  @param  current phase value
 *  @return a sine value
*******************************************************************************/

inline float Oscillator::oscSinP3(float _currPhase)
{
    // _x += -0.25f;
    // _x -= round(_x);

    float x = _currPhase + _currPhase;
    x = fabs(x);
    x = 0.5f - x;

    float x_square = x * x;

    return x * ((2.26548 * x_square - 5.13274) * x_square + 3.14159);
}
