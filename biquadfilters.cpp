/******************************************************************************/
/** @file		biquadfilters.cpp
    @date		2016-06-28
    @version	0.2
    @author		Anton Schmied[2016-03-18]
    @brief		BiquadFilters Class member and method definitions
*******************************************************************************/

#include "biquadfilters.h"



/******************************************************************************/
/** BiquadFilters Default Constructor
 * @brief    initialization of the modules local variabels with default values
 *           SampleRate:            48 kHz
 *           Cut Frequency:         22 kHz
 *           Shelf Amplification:   0
 *           Resonance:             0.5
 *           Filtetr Type:          Lowpass
*******************************************************************************/

BiquadFilters::BiquadFilters()
    : mSampleRate(48000.f)
    , mFilterCounter(0)
{
    setCutFreq(22000.f);
    setShelfAmp(0.f);
    setResonance(0.5);
    setFiltertype(BiquadFilterType::LOWPASS);
    resetStateVariables();
}



/******************************************************************************/
/** BiquadFilters Parameterized Constructor
 * @brief    initialization of the modules local variabels with custom
 *           parameters
*******************************************************************************/

BiquadFilters::BiquadFilters(int _sampleRate,
                             float _cutFreq,
                             float _shelfAmp,
                             float _resonance,
                             BiquadFilterType _filterType)
    : mSampleRate(static_cast<float>(_sampleRate))
    , mFilterCounter(0)
{
    setCutFreq(_cutFreq);
    setShelfAmp(_shelfAmp);
    setResonance(_resonance);
    setFiltertype(_filterType);
    resetStateVariables();
}



/*****************************************************************************/
/** @brief    sets cut frequency
 *  @param    cut frequency in Hz
******************************************************************************/

void BiquadFilters::setCutFreq(float _cutFreq)
{
    if (_cutFreq < (mSampleRate / 24576.f))					//clipping check
    {
        _cutFreq = mSampleRate / 24576.f;
    }

    if (_cutFreq > (mSampleRate / 2.125f))
    {
        _cutFreq = mSampleRate / 2.125f;
    }

    float omega = _cutFreq * (2.f * M_PI / mSampleRate);     //Frequency to Omega (Warp)

    mOmegaCos = NlToolbox::Math::cos(omega);                 //alternative to cos(omega) -> tools.h
    mOmegaSin = NlToolbox::Math::sin(omega);                 //alternative to sin(omega) -> tools.h

    setAlpha();
    calcCoeff();
}



/*****************************************************************************/
/** @brief    sets shelf amplification
 *  @param    shelf amplification in dB
******************************************************************************/

void BiquadFilters::setShelfAmp(float _shelfAmp)
{
    mShelfAmp = pow(1.059f, _shelfAmp);          		//alternative to pow(10, (_shelfAmp / 40.f))
    mBeta = 2.f * sqrt(mShelfAmp);

    calcCoeff();
}



/*****************************************************************************/
/** @brief    sets resonance
 *  @param    resonance
******************************************************************************/

void BiquadFilters::setResonance(float _resonance)
{
    mResonance = _resonance;

    if (mResonance > 0.999f)                     		//clipping check
    {
        mResonance = 0.999f;
    }

    else if (mResonance < -0.999f)
    {
        mResonance = -0.999f;
    }

    setAlpha();
    calcCoeff();
}



/*****************************************************************************/
/** @brief    sets filter type
 *  @param    filter type <BiquadFilterType>
******************************************************************************/

void BiquadFilters::setFiltertype(BiquadFilterType _filterType)
{
    mFilterType = _filterType;
    resetStateVariables();
    calcCoeff();
}



/*****************************************************************************/
/** @brief    applies the specified filter to the incoming sample
 *  @param    raw Sample
 *  @return   processed sample
******************************************************************************/

float BiquadFilters::applyFilter(float _currSample)
{
    float output = 0.f;

    output += mB0 * _currSample;
    output += mB1 * mInStateVar1;
    output += mB2 * mInStateVar2;

    output += mA1 * mOutStateVar1;
    output += mA2 * mOutStateVar2;

    mInStateVar2 = mInStateVar1;
    mInStateVar1 = _currSample;

    mOutStateVar2 = mOutStateVar1;
    mOutStateVar1 = output;

    return output;
}



/*****************************************************************************/
/** @brief    interface method which converts and scales the incoming midi
 *            values and passes these to the respective methods
 *  @param    midi control value [0 ... 127]
 *  @param    midi control ID -> enum CtrlID (biquadfilters.h)
******************************************************************************/

void BiquadFilters::setFilterParams(float _ctrlVal, unsigned char _ctrlId)
{
    switch (_ctrlId)
    {
        case CtrlId::CUTFREQ:
        _ctrlVal = 20.f * pow(2.f, _ctrlVal / 12.75f);                    //Midi to Freq [20Hz .. 19930Hz]
        setCutFreq(_ctrlVal);
        break;

        case CtrlId::SHELFAMP:
        _ctrlVal = ((_ctrlVal - 64.f) * 12.f) / 64.f;                     //Midi to [-12dB .. 12dB] linear
        setShelfAmp(_ctrlVal);
        break;

        case CtrlId::RESONANCE:
        _ctrlVal = (_ctrlVal - 64.f) / 32.f;                              //Midi to [-1 .. 1]
        setResonance(_ctrlVal);
        break;

        case CtrlId::FILTERTYPE:
        {
            if (static_cast<int>(_ctrlVal) > 0)
            {
                ++mFilterCounter;

                if (mFilterCounter > 3)
                {
                    mFilterCounter = 0;
                }

                switch(mFilterCounter)
                {
                    case 0:
                    setFiltertype(BiquadFilterType::LOWPASS);
                    printf("lowpass on\n");
                    break;

                    case 1:
                    setFiltertype(BiquadFilterType::HIGHPASS);
                    printf("highpass on\n");
                    break;

                    case 2:
                    setFiltertype(BiquadFilterType::LOWSHELF);
                    printf("lowshelf on\n");
                    break;

                    case 3:
                    setFiltertype(BiquadFilterType::HIGHSHELF);
                    printf("highshelf on\n");
                    break;
                }
            }
        }
        break;
    }
}



/*****************************************************************************/
/** @brief    calculates the coefficients depending on the chosen filter type
******************************************************************************/

void BiquadFilters::calcCoeff()
{
    float coeff;

    switch(mFilterType)
    {
        case BiquadFilterType::LOWPASS:

        mA0 = 1.f + mAlpha;
        mA1 = mOmegaCos * -2.f;
        mA2 = 1.f - mAlpha;
        mB0 = (1.f - mOmegaCos) / 2.f;
        mB1 = 1.f - mOmegaCos;
        mB2 = mB0;
        break;

        case BiquadFilterType::HIGHPASS:

        mA0 = 1.f + mAlpha;
        mA1 = mOmegaCos * -2.f;
        mA2 = 1.f - mAlpha;
        mB0 = (1.f + mOmegaCos) / 2.f;
        mB1 = (1.f + mOmegaCos) * -1.f;
        mB2 = mB0;
        break;

        case BiquadFilterType::LOWSHELF:
        coeff = mBeta * mAlpha;

        mA0 = (mShelfAmp + 1.f) + (mOmegaCos * (mShelfAmp - 1.f)) + coeff;
        mA1 = ((mShelfAmp - 1.f) + (mOmegaCos * (mShelfAmp + 1.f))) * -2.f;
        mA2 = (mShelfAmp + 1.f) + (mOmegaCos * (mShelfAmp - 1.f)) - coeff;
        mB0 = ((mShelfAmp + 1.f) - (mOmegaCos * (mShelfAmp - 1.f)) + coeff) * mShelfAmp;
        mB1 = ((mShelfAmp - 1.f) - (mOmegaCos * (mShelfAmp + 1.f))) * 2.f * mShelfAmp;
        mB2 = ((mShelfAmp + 1.f) - (mOmegaCos * (mShelfAmp - 1.f)) - coeff) * mShelfAmp;
        break;

        case BiquadFilterType::HIGHSHELF:
        coeff = mBeta * mAlpha;

        mA0 = (mShelfAmp + 1.f) - (mOmegaCos * (mShelfAmp - 1.f)) + coeff;
        mA1 = ((mShelfAmp - 1.f) - (mOmegaCos * (mShelfAmp + 1.f))) * 2.f;
        mA2 = (mShelfAmp + 1.f) - (mOmegaCos * (mShelfAmp - 1.f)) - coeff;
        mB0 = ((mShelfAmp + 1.f) + (mOmegaCos * (mShelfAmp - 1.f)) + coeff) * mShelfAmp;
        mB1 = ((mShelfAmp - 1.f) + (mOmegaCos * (mShelfAmp + 1.f))) * -2.f * mShelfAmp;
        mB2 = ((mShelfAmp + 1.f) + (mOmegaCos * (mShelfAmp - 1.f)) - coeff) * mShelfAmp;
        break;
    }

    mA1 /= (-1.f * mA0);              //normalize
    mA2 /= (-1.f * mA0);
    mB0 /= mA0;
    mB1 /= mA0;
    mB2 /= mA0;
}



/*****************************************************************************/
/** @brief    alpha calculation
******************************************************************************/

inline void BiquadFilters::setAlpha()
{
    mAlpha = mOmegaSin * (1.f - mResonance);
}



/*****************************************************************************/
/** @brief    resetes the state variables if the filter type is changed
******************************************************************************/

void BiquadFilters::resetStateVariables()
{
    mInStateVar1 = 0.f;
    mInStateVar2 = 0.f;

    mOutStateVar1 = 0.f;
    mOutStateVar2 = 0.f;
}
