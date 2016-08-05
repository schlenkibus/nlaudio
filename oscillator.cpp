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
 *           Seed/ VoiceNumber:     1
*******************************************************************************/

Oscillator::Oscillator()
    : mSampleRate(48000)
    , mOscPhase(0.f)
    , mFluctAmnt(0.f)
    , mPhaseStateVar(0.f)
    , mEdge(true)
    , mChirpFilter()
{

    setSeed(1);
}



/******************************************************************************/
/** Main Oscillator Function
 *  @param  radiant for phase modulation
*******************************************************************************/

float Oscillator::applyOscillator(float _oscFreq)
{
    float output = 0.f;                                     // Output sample
    float currPhase = 0.f;

    if (mEdge)
    {
        float randVal = calcRandVal();
        float currFreq = randVal * mFluctAmnt * _oscFreq;
        mPhaseInc = (_oscFreq + currFreq) / mSampleRate;
    }

    mModRadians = mChirpFilter.applyFilter(mModRadians);

    mOscPhase += mPhaseInc;
    mOscPhase -= round(mOscPhase);                          // Wrap
///    OR
//    if (mOscPhase > 0.5f)
//    {
//        mOscPhase -= 1.0f;
//    }

    currPhase = mOscPhase + mModRadians;
    currPhase += (0.25f);

    currPhase -= round(currPhase);                        // Wrap

    if (fabs(mPhaseStateVar - currPhase) > 0.5f)
    {
        mEdge = true;
    }
    else
    {
        mEdge = false;
    }

    mPhaseStateVar = currPhase;
    output = oscSinP3(currPhase);

    return output;
}



/******************************************************************************/
/** sets the initial Value for the randomize() algorythm
 *  @param  _voiceNumber
*******************************************************************************/

void Oscillator::setSeed(signed int _voiceNumber)
{
    mRandValStateVar = _voiceNumber;
}



/******************************************************************************/
/** sets the initial value of the modulation radians
 *  @param  modulation radians
*******************************************************************************/

void Oscillator::setModulationRadians(float _modRadians)
{
    mModRadians = _modRadians;
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
/** Calculates a random Value every half phase cycle
 *
*******************************************************************************/

float Oscillator::calcRandVal()
{
    mRandValStateVar = mRandValStateVar * 1103515245 + 12345;

    return static_cast<float>(mRandValStateVar) * 4.5657f * pow(10.f,-10.f);
}




/******************************************************************************/
/** Chirp Filter Setup
 *  @param  Filter Cut Frequency in Hz
*******************************************************************************/

void Oscillator::setupChirpFilter(float _chirpFrequency)
{
    mChirpFilter.setFrequency(_chirpFrequency);
}



inline float Oscillator::oscSinP3(float _x)
{
    // _x += -0.25f;
    // _x -= round(_x);

    _x += _x;
    _x = fabs(_x);
    _x = 0.5f - _x;

    float x_square = _x * _x;
    _x = _x * ((2.26548 * x_square - 5.13274) * x_square + 3.14159);

    return _x;
}


void Oscillator::resetPhase(float _phase)
{
    mOscPhase = _phase;
}
