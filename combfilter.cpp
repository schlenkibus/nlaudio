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
    , mCBLowpass()
{

}



/******************************************************************************/
/** @brief    main function which applies the comb filter on the incoming
 *            samples from the Soundgenerators
*******************************************************************************/

void CombFilter::applyCombFilter(float _sampleA, float _sampleB)
{
    float mixSample;

    /// 1-Pole Lowpass
    mixSmaple = mCBLowpass.applyFilter(mixSample);

    /// Allpass

    /// Delay
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
//            setABCrossfadeamount(_ctrlVal);
            break;

        case CtrlId::PITCHEDIT:

            break;

        case CtrlId::PITCH_KEYTRACKING:
            _ctrlVal = (_ctrlVal / 127.f) * 1.05f;

            printf("Pitch Key Trk: %f\n", _ctrlVal);
//            setPitchKeytrackingAmnt(_ctrlVal);
            break;

        case CtrlId::PITCH_ENVCAMOUNT:
            _ctrlVal = (_ctrlVal / 0.7875f) - 80.f;

            if (_ctrlVal > 80.f)
            {
                _ctrlVal = 80.f;
            }

            printf("Pitch EnvC: %f\n", _ctrlVal);
//            setPitchEnvCAmnt(_ctrlVal);
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
            _ctrlVal = (_ctrVal / 1.26f) + 40.f;

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
//            setPhasemodAmnt(_ctrlVal);
            break;

        case CtrlId::AB_PHASEMOD:
            _ctrlVal = (_ctrlVal / 126.f);

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("AB Phasemod Amount %f\n", _ctrlVal);
    //            setABPhasemodAmnt(_ctrlVal);
            break;
    }
}



/*****************************************************************************/
/** @brief  setter function for highcut frequency of the internal lowpass filter.
 *          the frequency is calculated depending on set HiCut, Key Tracking
 *          and modulation amount of the Envelope C
******************************************************************************/

void CombFilter::setLowpassFreq()
{
    float cutFrequency;

    cutFrequency = ((mPitch * mLowpassKeyTrk) + mLowpassPitch) + (mEnv * mLowpassEnvC);

    cutFrequency = NlToolbox::Conversion::pitch2freq(cutFrequency);

    mCBLowpass.setCoefficient(cutFrequency);
}

