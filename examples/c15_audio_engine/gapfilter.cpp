/******************************************************************************/
/** @file		gapfilter.cpp
    @date		2017-08-30
    @version	0.1
    @author		Anton Schmied[2017-08-30]
    @brief		Gap Filter Class member and method definitions
*******************************************************************************/

#include "gapfilter.h"

/******************************************************************************/
/** GAp Filter Default Constructor
 * @brief   initialization of the modules local variabels with default values
 *          Center:         72
 *          Gap:            12
 *          Balance:        0
 *          Mix:            0
 *          Stereo:         0
 *          Resonance:      0.5
*******************************************************************************/

GapFilter::GapFilter()
{
    //******************************* Outputs ********************************//
    mGapFilterOut_L = 0.f;
    mGapFilterOut_R = 0.f;

    //****************************** Controls ********************************//
    mCenter = 72.f;
    mGap = 12.f * 0.5f;
    mBalance = 0.f;
    mMix = 0.f;
    mStereo = 0.f * 0.5f;
    mResonance = 0.5f * 0.9f;

    if (mMix > 0.f)
    {
        mMixSign = 1.f;
    }
    else
    {
        mMixSign = -1.f;
    }

    calcGapFreq();
    calcFilterMix();

    //******************************* Filters ********************************//
    pHighpass_L1 = new BiquadFilters(SAMPLERATE, 740.f, 0.f, 0.45f, BiquadFilterType::HIGHPASS);
    pHighpass_L2 = new BiquadFilters(SAMPLERATE, 740.f * 0.75f, 0.f, 0.45f, BiquadFilterType::HIGHPASS);
    pHighpass_R1 = new BiquadFilters(SAMPLERATE, 740.f, 0.f, 0.45f, BiquadFilterType::HIGHPASS);
    pHighpass_R2 = new BiquadFilters(SAMPLERATE, 740.f * 0.75f, 0.f, 0.45f, BiquadFilterType::HIGHPASS);

    pLowpass_L1 = new BiquadFilters(SAMPLERATE, 370.f, 0.f, 0.45f, BiquadFilterType::LOWPASS);
    pLowpass_L2 = new BiquadFilters(SAMPLERATE, 370.f * 1.33f, 0.f, 0.45f, BiquadFilterType::LOWPASS);
    pLowpass_R1 = new BiquadFilters(SAMPLERATE, 370.f, 0.f, 0.45f, BiquadFilterType::LOWPASS);
    pLowpass_R2 = new BiquadFilters(SAMPLERATE, 370.f * 1.33f, 0.f, 0.45f, BiquadFilterType::LOWPASS);

    //***************************** Smoothing ********************************//
    while (mSmootherMask)
    {
        applySmoother();
    }

    mSmootherMask = 0x0000;
    mFilterFreq_ramp = 1.f;
    mResonance_ramp = 1.f;
    mFilterMix_ramp = 1.f;
}



/******************************************************************************/
/** Gap Filter Parameterized Constructor
 * @brief    initialization of the modules local variabels with default values
*******************************************************************************/

GapFilter::GapFilter(float _center,
                     float _gap,
                     float _balance,
                     float _mix,
                     float _stereo,
                     float _resonance)
{
    //******************************* Outputs ********************************//
    mGapFilterOut_L = 0.f;
    mGapFilterOut_R = 0.f;

    //****************************** Controls ********************************//
    mCenter = _center;
    mGap = _gap * 0.5f;
    mBalance = _balance;
    mMix = _mix;
    mStereo = _stereo * 0.5f;
    mResonance = _resonance * 0.9f;

    if (mMix > 0.f)
    {
        mMixSign = 1.f;
    }
    else
    {
        mMixSign = -1.f;
    }

    calcGapFreq();
    calcFilterMix();

    //******************************* Filters ********************************//
    pHighpass_L1 = new BiquadFilters(SAMPLERATE, 0.f, 0.f, 0.f, BiquadFilterType::HIGHPASS);
    pHighpass_L2 = new BiquadFilters(SAMPLERATE, 0.f, 0.f, 0.f, BiquadFilterType::HIGHPASS);
    pHighpass_R1 = new BiquadFilters(SAMPLERATE, 0.f, 0.f, 0.f, BiquadFilterType::HIGHPASS);
    pHighpass_R2 = new BiquadFilters(SAMPLERATE, 0.f, 0.f, 0.f, BiquadFilterType::HIGHPASS);

    pLowpass_L1 = new BiquadFilters(SAMPLERATE, 0.f, 0.f, 0.f, BiquadFilterType::LOWPASS);
    pLowpass_L2 = new BiquadFilters(SAMPLERATE, 0.f, 0.f, 0.f, BiquadFilterType::LOWPASS);
    pLowpass_R1 = new BiquadFilters(SAMPLERATE, 0.f, 0.f, 0.f, BiquadFilterType::LOWPASS);
    pLowpass_R2 = new BiquadFilters(SAMPLERATE, 0.f, 0.f, 0.f, BiquadFilterType::LOWPASS);

    //***************************** Smoothing ********************************//
    while (mSmootherMask)
    {
        applySmoother();
    }

    mSmootherMask = 0x0000;
    mFilterFreq_ramp = 1.f;
    mResonance_ramp = 1.f;
    mFilterMix_ramp = 1.f;
}



/******************************************************************************/
/** Gap Filter Destructor
 * @brief
*******************************************************************************/

GapFilter::~GapFilter()
{
    delete pHighpass_L1;
    delete pHighpass_L2;
    delete pHighpass_R1;
    delete pHighpass_R2;
    delete pLowpass_L1;
    delete pLowpass_L2;
    delete pLowpass_R1;
    delete pLowpass_R2;
}



/*****************************************************************************/
/** @brief    processes the incoming samples of both channels
 *  @param    raw left Sample, raw right Sample
******************************************************************************/

void GapFilter::applyGapFilter(float _rawSample_L, float _rawSample_R)
{
    //***************************** Smoothing *******************************//
    if(mSmootherMask)
    {
        applySmoother();
    }


    //*************************** Left Highpass *****************************//
    float highpassSample = pHighpass_L1->applyFilter(_rawSample_L);
    highpassSample = pHighpass_L2->applyFilter(highpassSample);
    highpassSample *= mHpOutMix;

    //**************************** Left Lowpass *****************************//
    float lowpassSample = pLowpass_L1->applyFilter(highpassSample * mHpLpMix + _rawSample_L * mInLpMix);
    lowpassSample = pLowpass_L2->applyFilter(lowpassSample);
    lowpassSample *= mLpOutMix;

    mGapFilterOut_L = highpassSample + lowpassSample + (_rawSample_L * mInOutMix);


    //*************************** Right Highpass ****************************//
    highpassSample = pHighpass_R1->applyFilter(_rawSample_R);
    highpassSample = pHighpass_R2->applyFilter(highpassSample);

    highpassSample *= mHpOutMix;

    //**************************** Right Lowpass ****************************//
    lowpassSample = pLowpass_R1->applyFilter(highpassSample * mHpLpMix + _rawSample_R * mInLpMix);
    lowpassSample = pLowpass_R2->applyFilter(lowpassSample);

    lowpassSample *= mLpOutMix;

    mGapFilterOut_R = highpassSample + lowpassSample + (_rawSample_R * mInOutMix);
}



/*****************************************************************************/
/** @brief    interface method which converts and scales the incoming midi
 *            values and passes these to the respective methods
 *  @param    midi control value [0 ... 127]
 *  @param    midi control ID -> enum CtrlID (flanger.h)
******************************************************************************/

void GapFilter::setGapFilterParams(unsigned char _ctrlID, float _ctrlVal)
{
    switch (_ctrlID)
    {
        case CtrlID::CENTER:
            _ctrlVal = (_ctrlVal / 126.f) * 96.f + 24.f;

            if (_ctrlVal > 120.f)
            {
                _ctrlVal = 120.f;
            }

            printf("Gap Filter - Center: %f\n", _ctrlVal);

            mCenter = _ctrlVal;
            calcGapFreq();
            break;

        case CtrlID::GAP:
            _ctrlVal = _ctrlVal * 0.756f;

            if (_ctrlVal > 96.f)
            {
                _ctrlVal = 96.f;
            }

            printf("Gap Filter - Gap: %f\n", _ctrlVal);

            mGap = _ctrlVal * 0.5f;
            calcGapFreq();
            break;

        case CtrlID::BALANCE:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("Gap Filter - Balance: %f\n", _ctrlVal);

            mBalance = _ctrlVal;
            calcFilterMix();
            break;

        case CtrlID::MIX:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("Gap Filter - Mix: %f\n", _ctrlVal);

            mMix = _ctrlVal;

            if (mMix > 0.f)
            {
                mMixSign = 1.f;
            }
            else
            {
                mMixSign = -1.f;
            }

            calcGapFreq();
            calcFilterMix();
            break;

        case CtrlID::STEREO:
            _ctrlVal = ((_ctrlVal / 63.f) - 1.f) * 36.f;

            if (_ctrlVal > 36.f)
            {
                _ctrlVal = 36.f;
            }

            printf("Gap Filter - Stereo: %f\n", _ctrlVal);

            mStereo = _ctrlVal * 0.5f;
            calcGapFreq();
            break;

        case CtrlID::RESONANCE:
            _ctrlVal = (_ctrlVal / 126.f);

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("Gap Filter - Resonance: %f\n", _ctrlVal);

            mResonance_target = _ctrlVal * 0.9f;
            mResonance_base = mResonance;
            mResonance_diff = mResonance_target - mResonance_base;

            mSmootherMask |= 0x0002;
            mResonance_ramp = 1.f;
            break;
    }
}



/*****************************************************************************/
/** @brief Cut Frequency Calculation for all Filters depending on the Mix Sign,
 *         Gap Amount, Center Center Position and Stereo Amount
 *         Since the Frequencies must be smoothed, the rest of the calculation
 *         is to be found on applySmoother() under ID 1
******************************************************************************/

void GapFilter::calcGapFreq()
{
    float gapMixVar = mMixSign * mGap;
    float gapMixStereoVar = mCenter + gapMixVar;

    // HFL
    mHighpassFreq_L_target = gapMixStereoVar - mStereo;
    mHighpassFreq_L_target = NlToolbox::Conversion::pitch2freq(mHighpassFreq_L_target);
    mHighpassFreq_L_base = mHighpassFreq_L;
    mHighpassFreq_L_diff = mHighpassFreq_L_target - mHighpassFreq_L_base;

    // HFR
    mHighpassFreq_R_target = gapMixStereoVar + mStereo;
    mHighpassFreq_R_target = NlToolbox::Conversion::pitch2freq(mHighpassFreq_R_target);
    mHighpassFreq_R_base = mHighpassFreq_R;
    mHighpassFreq_R_diff = mHighpassFreq_R_target - mHighpassFreq_R_base;

    gapMixStereoVar = mCenter - gapMixVar;

    // LFL
    mLowpassFreq_L_target = gapMixStereoVar - mStereo;
    mLowpassFreq_L_target = NlToolbox::Conversion::pitch2freq(mLowpassFreq_L_target);
    mLowpassFreq_L_base = mLowpassFreq_L;
    mLowpassFreq_L_diff = mLowpassFreq_L_target - mLowpassFreq_L_base;

    // LFR
    mLowpassFreq_R_target = gapMixStereoVar + mStereo;
    mLowpassFreq_R_target = NlToolbox::Conversion::pitch2freq(mLowpassFreq_R_target);
    mLowpassFreq_R_base = mLowpassFreq_R;
    mLowpassFreq_R_diff = mLowpassFreq_R_target - mLowpassFreq_R_base;

    mSmootherMask |= 0x0001;
    mFilterFreq_ramp = 0.f;
}



/*****************************************************************************/
/** @brief Filter Mix Calculation depending on Mix and Balance amounts
******************************************************************************/

void GapFilter::calcFilterMix()
{
    float wet = fabs(mMix);
    float dry = wet;

    wet = NlToolbox::Math::sin(wet * CONST_HALF_PI);
    dry = NlToolbox::Math::sin((1.f - dry) * CONST_HALF_PI);

    float a_hi, a_lo;

    if (mMix > 0.f)                     // Parallel
    {
        float balance = mBalance * 0.5f + 0.5f;

        a_hi = NlToolbox::Math::sin(balance * CONST_HALF_PI);
        a_hi *= 1.41421f;

        a_lo = NlToolbox::Math::sin((1.f - balance) * CONST_HALF_PI);
        a_lo *= 1.41421f;

        mHpLpMix_target = 0.f;
        mInLpMix_target = 1.f;
        mHpOutMix_target = a_hi * wet;
        mLpOutMix_target = a_lo * wet;
        mInOutMix_target = dry;
    }
    else                                // Serial
    {
        if (mBalance > 0.f)
        {
            a_hi = mBalance * mBalance;
            a_lo = 0.f;
        }
        else
        {
            a_hi = 0.f;
            a_lo = mBalance * mBalance;
        }

        mHpLpMix_target = 1.f - a_lo;
        mInLpMix_target = a_lo;
        mHpOutMix_target = (1.f - a_lo) * a_hi * wet;
        mLpOutMix_target = wet - (a_hi * wet);
        mInOutMix_target = (a_hi * wet) * a_lo + dry;
    }

    mHpLpMix_base = mHpLpMix;
    mInLpMix_base = mInLpMix;
    mHpOutMix_base = mHpOutMix;
    mLpOutMix_base = mLpOutMix;
    mInOutMix_base = mInOutMix;

    mHpLpMix_diff = mHpLpMix_target - mHpLpMix_base;
    mInLpMix_diff = mInLpMix_target - mInLpMix_base;
    mHpOutMix_diff = mHpOutMix_target - mHpOutMix_base;
    mLpOutMix_diff = mLpOutMix_target - mLpOutMix_base;
    mInOutMix_diff = mInOutMix_target - mInOutMix_base;

    mSmootherMask |= 0x0004;
    mFilterMix_ramp = 0.f;
}



/*****************************************************************************/
/** @brief    applies the smoothers of the cabinet module, depending if the
 *            corresponding bit of the mask is set to 1
******************************************************************************/

void GapFilter::applySmoother()
{
    //************************ ID 1: Filter Frequencies ******************************//

    if (mFilterFreq_ramp < 1.0f)
    {
        mFilterFreq_ramp += SMOOTHER_INC;

        if (mFilterFreq_ramp > 1.0f)
        {
            mHighpassFreq_L = mHighpassFreq_L_target;
            mHighpassFreq_R = mHighpassFreq_R_target;
            mLowpassFreq_L = mLowpassFreq_L_target;
            mLowpassFreq_R = mLowpassFreq_R_target;

            mSmootherMask &= 0xFFFE;        // switch 1st bit to 0
        }
        else
        {
            mHighpassFreq_L = mHighpassFreq_L_base + mHighpassFreq_L_diff * mFilterFreq_ramp;
            mHighpassFreq_R = mHighpassFreq_R_base + mHighpassFreq_R_diff * mFilterFreq_ramp;
            mLowpassFreq_L = mLowpassFreq_L_base + mLowpassFreq_L_diff * mFilterFreq_ramp;
            mLowpassFreq_R = mLowpassFreq_R_base + mLowpassFreq_R_diff * mFilterFreq_ramp;
        }

        pHighpass_L1->setCutFreq(mHighpassFreq_L);
        pHighpass_L2->setCutFreq(mHighpassFreq_L * 0.75f);

        mScaledFreqHP_L = (1.f / (FREQCLIP_14700HZ - FREQCLIP_22050HZ)) * (mHighpassFreq_L - FREQCLIP_22050HZ);

        if (mScaledFreqHP_L > 1.f)
        {
            mScaledFreqHP_L = 1.f;
        }
        else if (mScaledFreqHP_L < -1.f)
        {
            mScaledFreqHP_L = -1.f;
        }

        float resonance = mResonance * mScaledFreqHP_L;
        pHighpass_L1->setResonance(resonance);
        pHighpass_L2->setResonance(resonance);


        pHighpass_R1->setCutFreq(mHighpassFreq_R);
        pHighpass_R2->setCutFreq(mHighpassFreq_R * 0.75f);

        mScaledFreqHP_R = (1.f / (FREQCLIP_14700HZ - FREQCLIP_22050HZ)) * (mHighpassFreq_R - FREQCLIP_22050HZ);

        if (mScaledFreqHP_R > 1.f)
        {
            mScaledFreqHP_R = 1.f;
        }
        else if (mScaledFreqHP_R < -1.f)
        {
            mScaledFreqHP_R = -1.f;
        }

        resonance = mResonance * mScaledFreqHP_R;
        pHighpass_R1->setResonance(resonance);
        pHighpass_R2->setResonance(resonance);


        pLowpass_L1->setCutFreq(mLowpassFreq_L);
        pLowpass_L2->setCutFreq(mLowpassFreq_L * 1.33f);

        mScaledFreqLP_L = (1.f / (FREQCLIP_14700HZ - FREQCLIP_22050HZ)) * (mLowpassFreq_L - FREQCLIP_22050HZ);

        if (mScaledFreqLP_L > 1.f)
        {
            mScaledFreqLP_L = 1.f;
        }
        else if (mScaledFreqLP_L < -1.f)
        {
            mScaledFreqLP_L = -1.f;
        }

        resonance = mResonance * mScaledFreqLP_L;
        pLowpass_L1->setResonance(resonance);
        pLowpass_L2->setResonance(resonance);


        pLowpass_R1->setCutFreq(mLowpassFreq_R);
        pLowpass_R2->setCutFreq(mLowpassFreq_R * 1.33f);

        mScaledFreqLP_R = (1.f / (FREQCLIP_14700HZ - FREQCLIP_22050HZ)) * (mLowpassFreq_R - FREQCLIP_22050HZ);

        if (mScaledFreqLP_R > 1.f)
        {
            mScaledFreqLP_R = 1.f;
        }
        else if (mScaledFreqLP_R < -1.f)
        {
            mScaledFreqLP_R = -1.f;
        }

        resonance = mResonance * mScaledFreqLP_R;
        pLowpass_R1->setResonance(resonance);
        pLowpass_R2->setResonance(resonance);

    }


    //**************************** ID 2: Resonance ***********************************//

    if (mResonance_ramp < 1.0f)
    {
        mResonance_ramp += SMOOTHER_INC;

        if (mResonance_ramp > 1.0f)
        {
            mResonance = mResonance_target;
            mSmootherMask &= 0xFFFD;        // switch 2nd bit to 0
        }
        else
        {
            mResonance = mResonance_base + mResonance_diff * mResonance_ramp;
        }

        float resonance = mResonance * mScaledFreqHP_L;
        pHighpass_L1->setResonance(resonance);
        pHighpass_L2->setResonance(resonance);

        resonance = mResonance * mScaledFreqHP_R;
        pHighpass_R1->setResonance(resonance);
        pHighpass_R2->setResonance(resonance);

        resonance = mResonance * mScaledFreqLP_L;
        pLowpass_L1->setResonance(resonance);
        pLowpass_L2->setResonance(resonance);

        resonance = mResonance * mScaledFreqLP_R;
        pLowpass_R1->setResonance(resonance);
        pLowpass_R2->setResonance(resonance);
    }

    //**************************** ID 3: Filter Mix **********************************//

    if (mFilterMix_ramp < 1.0f)
    {
        mFilterMix_ramp += SMOOTHER_INC;

        if (mFilterMix_ramp > 1.0f)
        {
            mHpLpMix = mHpLpMix_target;
            mInLpMix = mInLpMix_target;
            mHpOutMix = mHpOutMix_target;
            mLpOutMix = mLpOutMix_target;
            mInOutMix = mInOutMix_target;
            mSmootherMask &= 0xFFFB;
        }
        else
        {
            mHpLpMix = mHpLpMix_base + mHpLpMix_diff * mFilterMix_ramp;
            mInLpMix = mInLpMix_base + mInLpMix_diff * mFilterMix_ramp;
            mHpOutMix = mHpOutMix_base + mHpOutMix_diff * mFilterMix_ramp;
            mLpOutMix = mLpOutMix_base + mLpOutMix_diff * mFilterMix_ramp;
            mInOutMix = mInOutMix_base + mInOutMix_diff * mFilterMix_ramp;
        }
    }
}
