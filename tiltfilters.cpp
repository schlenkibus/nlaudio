/******************************************************************************/
/** @file		tiltfilters.cpp
    @date		2016-06-28
    @version	0.2
    @author		Anton Schmied [2016-03-18]
    @brief		Biquad Tiltfilter Class member and method definitions
*******************************************************************************/

#include "tiltfilters.h"



/******************************************************************************/
/** TiltFilters Default Constructor
 * @brief    initialization of the modules local variabels with default values
 *           SampleRate:            48 kHz
 *           Cut Frequency:         22 kHz
 *           Tilt:                  0
 *           Slope Width:           2
 *           Resonance:             0.5
 *           Filtetr Type:          Lowpass
*******************************************************************************/

TiltFilters::TiltFilters()
    : mSampleRate(48000.f)
    , mFilterCounter(0)
{
    setCutFreq(22000.f);
    setTilt(0.f);
    setSlopeWidth(2.f);
    setResonance(0.5f);
    setFilterType(TiltFilterType::LOWPASS);
    resetStateVariables();
}



/******************************************************************************/
/** TiltFilters Parameterized Constructor
 * @brief    initialization of the modules local variabels with custom
 *           parameters
*******************************************************************************/

TiltFilters::TiltFilters(uint32_t _sampleRate,
                         float _cutFreq,
                         float _tilt,
                         float _slopeWidth,
                         float _resonance,
                         TiltFilterType _filterType)
    : mSampleRate(static_cast<float>(_sampleRate))
    , mFilterCounter(0)
{
    setCutFreq(_cutFreq);
    setTilt(_tilt);
    setSlopeWidth(_slopeWidth);
    setResonance(_resonance);
    setFilterType(_filterType);
    resetStateVariables();
}



/*****************************************************************************/
/** @brief    sets cut frequency
 *  @param    cut frequency in Hz
******************************************************************************/

void TiltFilters::setCutFreq(float _cutFreq)
{
    if (_cutFreq < (mSampleRate / 24576.f))                   //Frequency clipping
    {
        _cutFreq = mSampleRate / 24576.f;
    }

    else if (_cutFreq > (mSampleRate / 2.125f))
    {
        _cutFreq = mSampleRate / 2.125f;
    }

    float omega = _cutFreq * (2.f * M_PI / mSampleRate);      //Freqnecy to omega (warp)

    mOmegaSin = NlToolbox::Math::sin(omega);                         //alternative to sin(omega) -> tools.h
    mOmegaCos = NlToolbox::Math::cos(omega);                         //alternative to cos(omega) -> tools.h

    setAlpha();
    calcCoeff();
}



/*****************************************************************************/
/** @brief    sets tilt amount, somewhat similar to shelf amplification
 *  @param    tilt in dB
******************************************************************************/

void TiltFilters::setTilt(float _tilt)
{
    mTilt = pow(1.059f, _tilt);                          //alterative to pow(10, (tilt/ 40.f))
    mBeta = 2.f * sqrt(mTilt);

    setAlpha();
    calcCoeff();
}



/*****************************************************************************/
/** @brief    sets resonance
 *  @param    resonance
******************************************************************************/

void TiltFilters::setResonance(float _resonance)
{
    mResonance = _resonance;

    if (mResonance > 0.999f)                             //Resonance clipping
    {
        mResonance = 0.999f;
    }

    setAlpha();
    calcCoeff();
}



/*****************************************************************************/
/** @brief    sets shelf width
 *  @param    slope width
******************************************************************************/

void TiltFilters::setSlopeWidth(float _slopeWidth)
{
    mSlopeWidth = _slopeWidth < 1.f                      //min clip check
            ? 1.f
            : _slopeWidth;

    setAlpha();
    calcCoeff();
}



/*****************************************************************************/
/** @brief    sets filter Type
 *  @param    filter Type <TiltFilterType>
******************************************************************************/

void TiltFilters::setFilterType(TiltFilterType _filterType)
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

float TiltFilters::applyFilter(float _currSample)
{
    float output;

    output  = mB0 * _currSample;
    output += mB1 * mInStateVar1;
    output += mB2 * mInStateVar2;

    output += mA1 * mOutStateVar1;
    output += mA2 * mOutStateVar2;

    mInStateVar2 = mInStateVar1 + DNC_CONST;
    mInStateVar1 = _currSample + DNC_CONST;

    mOutStateVar2 = mOutStateVar1 + DNC_CONST;
    mOutStateVar1 = output + DNC_CONST;

    return output;
}




/*****************************************************************************/
/** @brief    interface method which converts and scales the incoming midi
 *            values and passes these to the respective methods
 *  @param    midi control value [0 ... 127]
 *  @param    midi control ID -> enum CtrlID (tiltfilters.h)
******************************************************************************/

void TiltFilters::setFilterParams(unsigned char _ctrlID, float _ctrlVal)
{
    switch (_ctrlID)
    {
        case CtrlId::CUTFREQ:
            _ctrlVal = 20.f * pow(2.f, _ctrlVal / 12.75f);                    //Midi to Freq [20Hz .. 19930Hz]
            setCutFreq(_ctrlVal);
            break;

        case CtrlId::TILT:
            _ctrlVal = ((_ctrlVal - 64.f) * 12.f) / 64.f;                     //Midi to [-12dB .. 12dB] linear
            setTilt(_ctrlVal);
            break;

        case CtrlId::RESONANCE:
            _ctrlVal = (_ctrlVal - 64.f) / 32.f;                              //Midi to [-1 .. 1]
            setResonance(_ctrlVal);
            break;

        case CtrlId::SLOPEWIDTH:
            _ctrlVal = (_ctrlVal / 127.f * 4.f) + 1.f;                        //Midi to [1 .. 5]
            setSlopeWidth(_ctrlVal);
            break;

        case CtrlId::FILTERTYPE:
        {

            if (static_cast<uint32_t>(_ctrlVal) > 0)
            {

                ++mFilterCounter;

                if (mFilterCounter > 3)
                {
                    mFilterCounter = 0;}


                switch (mFilterCounter)
                {
                    case 0:
                        setFilterType(TiltFilterType::LOWPASS);
                        printf("lowpass on\n");
                        break;

                    case 1:
                        setFilterType(TiltFilterType::HIGHPASS);
                        printf("highpass on\n");
                        break;

                    case 2:
                        setFilterType(TiltFilterType::LOWSHELF);
                        printf("lowshelf on\n");
                        break;

                    case 3:
                        setFilterType(TiltFilterType::HIGHSHELF);
                        printf("highshelf on\n");
                        break;
                }
            }
        }
        break;
    }
}



/*****************************************************************************/
/** @brief    calculates the coefficients depending on the chosen filter Type
******************************************************************************/

void TiltFilters::calcCoeff()
{
    float coeff;

    switch (mFilterType)
    {
        case TiltFilterType::LOWPASS:
            mA0 = 1.f + mAlpha;
            mA1 = mOmegaCos * -2.f;
            mA2 = 1.f - mAlpha;
            mB0 = (1 - mOmegaCos) / 2.f;
            mB1 = 1.f - mOmegaCos;
            mB2 = mB0;
            break;

        case TiltFilterType::HIGHPASS:
            mA0 = 1.f + mAlpha;
            mA1 = mOmegaCos * -2.f;
            mA2 = 1.f - mAlpha;
            mB0 = (1.f + mOmegaCos) / 2.f;
            mB1 = (1.f + mOmegaCos) * -1.f;
            mB2 = mB0;
            break;

        case TiltFilterType::LOWSHELF:
            coeff = mBeta * mAlpha;
            mA0 = (mTilt + 1.f) + (mOmegaCos * (mTilt - 1.f)) + coeff;
            mA1 = ((mTilt - 1.f) + (mOmegaCos * (mTilt + 1.f))) * -2.f;
            mA2 = (mTilt + 1.f) + (mOmegaCos * (mTilt - 1.f)) - coeff;
            mB0 = ((mTilt + 1.f) - (mOmegaCos * (mTilt - 1.f)) + coeff) * mTilt;
            mB1 = ((mTilt - 1.f) - (mOmegaCos * (mTilt + 1.f))) * 2.f * mTilt;
            mB2 = ((mTilt + 1.f) - (mOmegaCos * (mTilt - 1.f)) - coeff) * mTilt;
            break;

        case TiltFilterType::HIGHSHELF:
            coeff = mBeta * mAlpha;
            mA0 = (mTilt + 1.f) - (mOmegaCos * (mTilt - 1.f)) + coeff;
            mA1 = ((mTilt - 1.f) - (mOmegaCos * (mTilt + 1.f))) * 2.f;
            mA2 = (mTilt + 1.f) - (mOmegaCos * (mTilt - 1.f)) - coeff;
            mB0 = ((mTilt + 1.f) + (mOmegaCos * (mTilt - 1.f)) + coeff) * mTilt;
            mB1 = ((mTilt - 1.f) + (mOmegaCos * (mTilt + 1.f))) * -2.f * mTilt;
            mB2 = ((mTilt + 1.f) + (mOmegaCos * (mTilt - 1.f)) - coeff) * mTilt;
            break;
    }

    mA1 /= (-1.f * mA0);          //normalize
    mA2 /= (-1.f * mA0);
    mB0 /= mA0;
    mB1 /= mA0;
    mB2 /= mA0;
}



/*****************************************************************************/
/** @brief    alpha calculation
******************************************************************************/

void TiltFilters::setAlpha()
{
    mAlpha = (mTilt + (1.f / mTilt)) * (mSlopeWidth - 1.f);
    mAlpha = sqrt(mAlpha + 2.f) * mOmegaSin * (1.f - mResonance);
}



/*****************************************************************************/
/** @brief    resetes the state variables if the filter Type is changed
******************************************************************************/

void TiltFilters::resetStateVariables()
{
    mInStateVar1 = 0.f;
    mInStateVar2 = 0.f;

    mOutStateVar1 = 0.f;
    mOutStateVar2 = 0.f;
}
