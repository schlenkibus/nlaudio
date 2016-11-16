/******************************************************************************/
/** @file		onepolefilters.cpp
    @date		2016-06-28
    @version    0.2
    @author		Anton Schmied[2016-03-18]
    @brief		OnePoleFilter Class member and method definitions
*******************************************************************************/

#include "onepolefilters.h"



/******************************************************************************/
/** OnePoleFilters Default Constructor
 * @brief    initialization of the modules local variabels with default values
 *           SampleRate:            48 kHz
 *           Cut Frequency:         22 kHz
 *           Shelf Amplification:   0
 *           FiltetrType:           Lowpass
*******************************************************************************/

OnePoleFilters::OnePoleFilters()
{
    mSampleRate = 48000.f;

    setCutFreq(22000.f);
    setShelfAmp(0.f);
    setFilterType(OnePoleFilterType::LOWPASS);
    resetStateVariables();

    mFilterCounter = 0;
}



/******************************************************************************/
/** OnePoleFilters Parameterized Constructor
 * @brief    initialization of the modules local variabels with custom
 *           parameters
*******************************************************************************/

OnePoleFilters::OnePoleFilters(int _sampleRate,
                               float _cutFreq,
                               float _shelfAmp,
                               OnePoleFilterType _filterType)
{
    mSampleRate = static_cast<float>(_sampleRate);

    setCutFreq(_cutFreq);
    setShelfAmp(_shelfAmp);
    setFilterType(_filterType);
    resetStateVariables();

    mFilterCounter = 0.f;
}



/*****************************************************************************/
/** @brief    sets cut frequency
 *  @param    cut frequency in Hz
******************************************************************************/

void OnePoleFilters::setCutFreq(float _cutFreq)
{
    switch (mFilterType)
    {
        case OnePoleFilterType::LOWSHELF:
            _cutFreq /= mShelfAmp;
            break;

        case OnePoleFilterType::HIGHSHELF:
            _cutFreq *= mShelfAmp;
            break;

        case OnePoleFilterType::HIGHPASS:
            break;

        case OnePoleFilterType::LOWPASS:
            break;
    }

    if (_cutFreq < (mSampleRate / 24000.f))             // clipping check
    {
        _cutFreq = mSampleRate / 24000.f;
    }

    if (_cutFreq > (mSampleRate / 2.18f))
    {
        _cutFreq = mSampleRate / 2.18f;
    }

    _cutFreq *= (M_PI / mSampleRate);                   // Frequency warp
    mOmegaTan = NlToolbox::Math::tan(_cutFreq);                      // alternative to tan(cutFreq) -> tools.h;

    calcCoeff();
}



/*****************************************************************************/
/** @brief    sets shelf amplification
 *  @param    shelf amplification in dB
******************************************************************************/

void OnePoleFilters::setShelfAmp(float _shelfAmp)
{
    mShelfAmp = pow(1.059f, _shelfAmp);                 // alternative to pow(10, (_mShelfAmp / 40.f));
    mShelfAmpSquare = mShelfAmp * mShelfAmp;

    calcCoeff();
}



/*****************************************************************************/
/** @brief    sets filter Type
 *  @param    filter Type <OnePoleFilterType>
******************************************************************************/

void OnePoleFilters::setFilterType(OnePoleFilterType _filterType)
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

float OnePoleFilters::applyFilter(float _currSample)
{
    float output = 0.f;

    output += mB0 * _currSample;
    output += mB1 * mInStateVar;
    output += mA1 * mOutStateVar;

    mInStateVar = _currSample;
    mOutStateVar = output;

    return output;
}



/*****************************************************************************/
/** @brief    interface method which converts and scales the incoming midi
 *            values and passes these to the respective methods
 *  @param    midi control value [0 ... 127]
 *  @param    midi control ID -> enum CtrlID (onepolefilters.h)
******************************************************************************/

void OnePoleFilters::setFilterParams(float _ctrlVal, unsigned char _ctrlId)
{
    switch (_ctrlId)
    {
        case CtrlId::CUTFREQ:
            _ctrlVal = 20.f * pow(2.f, _ctrlVal / 12.75f);              // Midi to Freq [20Hz .. 19930Hz]
            setCutFreq(_ctrlVal);
            break;

        case CtrlId::SHELFAMP:
            _ctrlVal = ((_ctrlVal - 64.f) * 12.f) / 64.f;               // Midi to [-12dB .. 12dB] linear
            setShelfAmp(_ctrlVal);
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

                switch (mFilterCounter)
                {
                    case 0:
                        setFilterType(OnePoleFilterType::LOWPASS);
                        printf("lowpass on\n");
                        break;

                    case 1:
                        setFilterType(OnePoleFilterType::HIGHPASS);
                        printf("highpass on\n");
                        break;

                    case 2:
                        setFilterType(OnePoleFilterType::LOWSHELF);
                        printf("lowshelf on\n");
                        break;

                    case 3:
                        setFilterType(OnePoleFilterType::HIGHSHELF);
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

void OnePoleFilters::calcCoeff()
{
    switch (mFilterType)
    {
        case OnePoleFilterType::LOWPASS:
            mA1 = (1.f - mOmegaTan) / (1.f + mOmegaTan);
            mB0 = mOmegaTan / (1.f + mOmegaTan);
            mB1 = mOmegaTan / (1.f + mOmegaTan);
            break;

        case OnePoleFilterType::HIGHPASS:
            mA1 = (1.f - mOmegaTan) / (1.f + mOmegaTan);
            mB0 = 1.f / (1.f + mOmegaTan);
            mB1 = (1.f / (1.f + mOmegaTan)) * -1.f;
            break;

        case OnePoleFilterType::LOWSHELF:
            mA1 = (1.f - mOmegaTan) / (1.f + mOmegaTan);
            mB0 = ((mOmegaTan / (1.f + mOmegaTan)) * (mShelfAmpSquare + -1.f)) + 1.f;
            mB1 = ((mOmegaTan / (1.f + mOmegaTan)) * (mShelfAmpSquare + -1.f)) - mA1;
            break;

        case OnePoleFilterType::HIGHSHELF:
            mA1 = (1.f - mOmegaTan) / (1.f + mOmegaTan);
            mB0 = ((mShelfAmpSquare + -1.f) / (1.f + mOmegaTan)) + 1.f;
            mB1 = (((mShelfAmpSquare + -1.f) / (1.f + mOmegaTan)) + mA1) * -1.f;
            break;
    }
}



/*****************************************************************************/
/** @brief    resetes the state variables if the filter Type is changed
******************************************************************************/

void OnePoleFilters::resetStateVariables()
{
    mInStateVar = 0.f;
    mOutStateVar = 0.f;
}
