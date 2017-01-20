/******************************************************************************/
/** @file		combfilter.cpp
    @date		2016-12-26
    @version	0.1
    @author		Anton Schmied[2016-12-26]
    @brief		Comb Filter Class member and method definitions
*******************************************************************************/

#include "combfilter.h"

/******************************************************************************/
/** Outputmixer Default Constructor
 * @brief    initialization of the modules local variabels with default values
*******************************************************************************/

CombFilter::CombFilter()
    : mSampleRate(48000.f)
    , mCombFilterOut(0.f)
    , mCFSmootherMask(0x000)
    , mInc(5.f / (static_cast<float>(48000.f) * 0.032))
    , mCBLowpass()
{

}


/******************************************************************************/
/** @brief    checks the mask, if any bit is 1, the smoother is applied fo the
 * corrsponding value
*******************************************************************************/

void CombFilter::applySmoothers()
{
    // 0: AB Select Amount
    if (mABSelectAmnt_ramp < 1.0)
    {
        mABSelectAmnt_ramp += mInc;

        if (mABSelectAmnt_ramp > 1.0)
        {
            mABSelectAmnt = mABSelectAmnt_target;
            mCFSmootherMask &= 0xFFFE;                  // switch first bit to 0
        }
        else
        {
            mABSelectAmnt = mABSelectAmnt_base + mABSelectAmnt_diff * mABSelectAmnt_ramp;
        }
    }

    // 1:
    if (mABPhasemodAmnt_ramp < 1.0)
    {
        mABPhasemodAmnt_ramp += mInc;

        if (mABPhasemodAmnt_ramp > 1.0)
        {
            mABPhasemodAmnt = mABPhasemodAmnt_target;
            mCFSmootherMask &= 0xFFFD;                  // switch second bit to 0
        }
        else
        {
            mABPhasemodAmnt = mABPhasemodAmnt_base + mABPhasemodAmnt_diff * mABPhasemodAmnt_ramp;
        }
    }

    // 2:

    // 3:

}


/******************************************************************************/
/** @brief    main function which applies the comb filter on the incoming
 *            samples from the Soundgenerators
*******************************************************************************/

void CombFilter::applyCombFilter(float _sampleA, float _sampleB)
{
    float outputSample;

    applySmoothers();           // Smoothing

//    mABPhasemod = _sampleA * (1.f - mABPhasemodAmnt) + _sampleB * mABPhasemodAmnt;
    outputSample = _sampleA * (1.f - mABSelectAmnt) + _sampleB * mABSelectAmnt;       // mix of both incoming samples

    /// 1-Pole Lowpass
//    outputSample = mCBLowpass.applyFilter(outputSample);

    /// Allpass

    /////////////////////////// Delay /////////////////////////////////////////////////////////////////
    mSampleBuffer[mSampleBufferIndex] = outputSample;               // write into the SampleBuffer

    mInd_tm1 = mSampleBufferIndex - mInd_tm1;
    mInd_t0  = mSampleBufferIndex - mInd_t0;
    mInd_tp1 = mSampleBufferIndex - mInd_tp1;
    mInd_tp2 = mSampleBufferIndex - mInd_tp2;

    mInd_tm1 &= (mSampleBuffer.size() - 1);                         // Wrap with a mask sampleBuffer.size()-1
    mInd_t0  &= (mSampleBuffer.size() - 1);
    mInd_tp1 &= (mSampleBuffer.size() - 1);
    mInd_tp2 &= (mSampleBuffer.size() - 1);

    outputSample = NlToolbox::Math::interpolRT(mDelaySamples_fract,          // Interpolate
                                               mSampleBuffer[mInd_tm1],
                                               mSampleBuffer[mInd_t0],
                                               mSampleBuffer[mInd_tp1],
                                               mSampleBuffer[mInd_tp2]);

//    outputSample * envC;                                                  /// Hier wird am ende och mal mit EnvC multiplieziert

    mSampleBufferIndex = (mSampleBufferIndex + 1) & (mSampleBuffer.size() - 1);     // increase index and chck boundaries
    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    mCombFilterOut = outputSample;
}



/*****************************************************************************/
/** @brief    interface method which converts and scales the incoming midi
 *            values and passes these to the respective methods or parameters
 *  @param    midi control ID -> enum CtrlID (combfilter.h)
 *  @param    midi control value [0 ... 127]
******************************************************************************/

void CombFilter::setCombFilterParams(unsigned char _ctrlID, float _ctrlVal)
{
    switch (_ctrlID)
    {
        case CtrlId::ABCROSSFADE:
            _ctrlVal = _ctrlVal / 127.f;

            printf("AB Crossfade: %f\n", _ctrlVal);
            setABSelectAmnt(_ctrlVal);
            break;

        case CtrlId::PITCHEDIT:
            _ctrlVal = (_ctrlVal / 126.f) * 120.f;

            if (_ctrlVal > 120.f)
            {
                _ctrlVal = 199.99f;
            }

            printf("Pitch Edit: %f\n", _ctrlVal);
//            setPitchEdit(_ctrlVal);
            break;

        case CtrlId::PITCH_KEYTRACKING:
            _ctrlVal = (_ctrlVal / 127.f) * 1.05f;

            printf("Pitch Key Trk: %f\n", _ctrlVal);
            setPitchKeytrackingAmnt(_ctrlVal);
            break;

        case CtrlId::PITCH_ENVCAMOUNT:
            _ctrlVal = (_ctrlVal / 0.7875f) - 80.f;

            if (_ctrlVal > 80.f)
            {
                _ctrlVal = 80.f;
            }

            printf("Pitch EnvC: %f\n", _ctrlVal);
            setPitchEnvCAmnt(_ctrlVal);
            break;

        case CtrlId::DECAYTIME:
            _ctrlVal = (_ctrlVal / .63f) - 100.f;

            if (_ctrlVal > 100.f)
            {
                _ctrlVal = 100.f;
            }

            printf("Decay Time: A Pan: %f\n", _ctrlVal);
//            setDecayTime(_ctrlVal);
            break;

        case CtrlId::DECAY_KEYTRACKING:
            _ctrlVal = _ctrlVal / 127.f;

            printf("Decay Key Trk: %f\n", _ctrlVal);
//            setDecayTimeKeytrackingAmnt(_ctrlVal);
            break;

        case CtrlId::DECAY_GATE:
            _ctrlVal = _ctrlVal / 127.f;

            printf("Decay Gate Amnt: %f\n", _ctrlVal);
//            setDecayTimeGateAmnt(_ctrlVal);
            break;

        case CtrlId::ALLPASSFREQ:
            _ctrlVal = _ctrlVal/ 0.9f;

            printf("Allpass Frequency: %f\n", _ctrlVal);
//            setAPFreq(_ctrlVal);
            break;

        case CtrlId::ALLPASSRES:
            _ctrlVal = _ctrlVal / 127.f;

            printf("Allpass Resonance: %f\n", _ctrlVal);
//            setAPRes(_ctrlVal);
            break;

        case CtrlId::ALLPASS_KEYTRACKING:
            _ctrlVal = _ctrlVal / 127.f;

            printf("Allpass Key Trk: %f\n", _ctrlVal);
//            setAllpassKeytrackingAmnt(_ctrlVal);
            break;

        case CtrlId::ALLPASS_ENVCAMOUNT:
            _ctrlVal = (_ctrlVal / 0.7875f) - 80.f;

            if (_ctrlVal > 80.f)
            {
                _ctrlVal = 80.f;
            }

            printf("Allpass EnvC: %f\n", _ctrlVal);
//            setAllpassEnvCAmnt(_ctrlVal);
            break;

        case CtrlId::LOWPASSFREQ:
            _ctrlVal = (_ctrlVal / 1.26f) + 40.f;

            printf("Lowpass Frequency: %f/n", _ctrlVal);

            mLowpassPitch = _ctrlVal;
            setLowpassFreq();
            break;

        case CtrlId::LOWPASS_KEYTRACKING:
            _ctrlVal = _ctrlVal / 127.f;

            printf("Lowpass Key Trk: %f\n", _ctrlVal);

            mLowpassKeyTrk = _ctrlVal;
            setLowpassFreq();
            break;

        case CtrlId::LOWPASS_ENVCAMOUNT:
            _ctrlVal = (_ctrlVal / 0.7875f) - 100.f;

            if (_ctrlVal > 100.f)
            {
                _ctrlVal = 100.f;
            }

            printf("Lowpass EnvC: %f\n", _ctrlVal);

            mLowpassPitch = _ctrlVal;
            setLowpassFreq();
            break;

        case CtrlId::PHASEMODAMOUNT:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("Phasemod Amount %f\n", _ctrlVal);
            setPhasemodAmnt(_ctrlVal);
            break;

        case CtrlId::AB_PHASEMOD:
            _ctrlVal = (_ctrlVal / 126.f);

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("AB Phasemod Amount %f\n", _ctrlVal);
            setABPhasemodAmnt(_ctrlVal);
            break;
    }
}



/*****************************************************************************/
/** @brief
******************************************************************************/

void CombFilter::setMainFreq(float _keyPitch)
{
    /// hier die Reihenfolge beachten... oder auch nicht!?!
    /// Tune Controls
    mMainFreq = NlToolbox::Conversion::pitch2freq(_keyPitch - 60.f);       /// hier fehlt PitchEdit und KeyTtracking

    /// DelaySample calculation
    float minClipVal = mSampleRate / (mSampleBuffer.size() - 2);

    if (mMainFreq < minClipVal) {mDelaySamples = mSampleRate / minClipVal;}
    else {mDelaySamples = mSampleRate / mMainFreq;}


    /// Delay Setup - eigene Funktion!?
    mDelaySamples -= 1.f;

    if (mDelaySamples > (mSampleBuffer.size() - 3))          // Clip 1, size-3
        {mDelaySamples = mSampleBuffer.size() - 3;}
    else if (mDelaySamples < 1.f)
        {mDelaySamples = 1.f;}

    mDelaySamples_int = round(mDelaySamples - 0.5f);          // integer and fraction speration
    mDelaySamples_fract = mDelaySamples - mDelaySamples_int;

    mInd_tm1 = mDelaySamples_int - 1;
    mInd_t0  = mDelaySamples_int;
    mInd_tp1 = mDelaySamples_int + 1;
    mInd_tp2 = mDelaySamples_int + 2;

    /// Allpass Controls

    /// LowPass Controls

    /// Feedback Controls

}

/*****************************************************************************/
/** @brief  Select Amount between the 2 Main Samples coming from the Soundgenerator
 *          the incoming value [0 ..1] is converted from a linear to a sine shape
 *          befor becoming a member of the Com Filter class
******************************************************************************/

void CombFilter::setABSelectAmnt(float _ctrlVal)
{
    mABSelectAmnt_target = NlToolbox::Curves::applySineCurve(_ctrlVal);

    mABSelectAmnt_base = mABSelectAmnt;
    mABSelectAmnt_diff = mABSelectAmnt_target - mABSelectAmnt_base;

    mCFSmootherMask |= 0x0001;      // ID: 0
    mABSelectAmnt_ramp = 0.f;
}



/*****************************************************************************/
/** @brief
******************************************************************************/

void CombFilter::setPitchEdit(float _ctrlVal)
{

}



/*****************************************************************************/
/** @brief
******************************************************************************/

void CombFilter::setPitchKeytrackingAmnt(float _ctrlVal)
{


}



/*****************************************************************************/
/** @brief
******************************************************************************/

void CombFilter::setPitchEnvCAmnt(float _ctrlVal)
{


}




/*****************************************************************************/
/** @brief
******************************************************************************/

void CombFilter::setPhasemodAmnt(float _ctrlVal)
{
    mPhasemodAmount = _ctrlVal;
}



/*****************************************************************************/
/** @brief
******************************************************************************/

void CombFilter::setABPhasemodAmnt(float _ctrlVal)
{
    mABPhasemodAmnt_target = _ctrlVal;

    mABPhasemodAmnt_base = mABPhasemodAmnt;
    mABPhasemodAmnt_diff = mABPhasemodAmnt_target - mABPhasemodAmnt_base;

    mCFSmootherMask |= 0x0002;      // ID: 1
    mABPhasemodAmnt_ramp = 0.f;

}



/*****************************************************************************/
/** &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
******************************************************************************/

/*****************************************************************************/
/** @brief  setter function for highcut frequency of the internal lowpass filter.
 *          the frequency is calculated depending on set HiCut, Key Tracking
 *          and modulation amount of the Envelope C
******************************************************************************/

void CombFilter::setLowpassFreq()
{
//    float cutFrequency;

//    cutFrequency = ((mPitch * mLowpassKeyTrk) + mLowpassPitch) + (mEnv * mLowpassEnvC);

//    cutFrequency = NlToolbox::Conversion::pitch2freq(cutFrequency);

//    mCBLowpass.setCoefficient(cutFrequency);
}





/*****************************************************************************/
/** @brief
******************************************************************************/

void CombFilter::setMainFreq()
{

}

