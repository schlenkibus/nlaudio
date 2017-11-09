/******************************************************************************/
/** @file		combfilter.cpp
    @date		2017-05-18
    @version	1.0
    @author		Anton Schmied[2016-12-26]
    @brief		Comb Filter Class member and method definitions
*******************************************************************************/

#include "combfilter.h"

/******************************************************************************/
/** Outputmixer Default Constructor
 * @brief    initialization of the modules local variabels with default values
*******************************************************************************/

CombFilter::CombFilter()
{
    mCombFilterOut = 0.f;

    mABMix_1 = 0.5f;
    mABMix_0 = 0.5;
    mPitchEdit = 60.f;
    mPitchKeyTrk = 1.f;

    mDecay = 0.f;
    mDecayKeyTrk = 0.33f;
    mDecayStateVar = 0.f;

    mAllpassTune = 140.f;
    mAllpassKeyTrk = 1.f;
    mAllpassRes = 0.33f;
    mAllpassStateVar_1 = 0.f;
    mAllpassStateVar_2 = 0.f;
    mAllpassStateVar_3 = 0.f;
    mAllpassStateVar_4 = 0.f;
    mNormPhase = 0.f;

    pHighpass = new OnePoleFilters(SAMPLERATE, 60.f, 0.f, OnePoleFilterType::HIGHPASS);

    mLowpassHiCut = 140.f;
    mLowpassKeyTrk = 1.f;
    mLowpassStateVar = 0.f;

    mNegPhase = 0.f;

    mPhaseMod = 0.f;
    mPhaseModMix_1 = 0.5f;
    mPhaseModMix_0 = 0.5f;

    mDelayClipMin = SAMPLERATE / (mSampleBuffer.size() - 2);
    mDelaySamples = 0.f;

    mSampleBufferSize = mSampleBuffer.size() - 1;
    mSampleBufferIndex = 0;

    mSmootherMask = 0x0000;
    mABMix_ramp = 1.f;
    mPhaseModMix_ramp = 1.f;
    mPitchKeyTrk_ramp = 1.f;
}



/******************************************************************************/
/** Comb Filter Parameterized Constructor
 * @brief    initialization of the modules local variabels with custom
 *           parameters
*******************************************************************************/

CombFilter::CombFilter(float _ABMix,
                       float _pitchEdit,
                       float _pitchKeyTrk,
                       float _decay,
                       float _decayKeyTrk,
                       float _allpassTune,
                       float _allpassKeyTrk,
                       float _allpassRes,
                       float _lowpassHiCut,
                       float _lowpassKeyTrk,
                       float _phaseMod,
                       float _phaseModMix)
{
    mCombFilterOut = 0.f;

    mABMix_1 = _ABMix;
    mABMix_0 = 1.f - _ABMix;
    mPitchEdit = _pitchEdit;
    mPitchKeyTrk = _pitchKeyTrk;

    mDecay = _decay;
    mDecayKeyTrk = _decayKeyTrk;
    mDecayStateVar = 0.f;

    mAllpassTune = _allpassTune;
    mAllpassKeyTrk = _allpassKeyTrk;
    mAllpassRes = _allpassRes;
    mAllpassStateVar_1 = 0.f;
    mAllpassStateVar_2 = 0.f;
    mAllpassStateVar_3 = 0.f;
    mAllpassStateVar_4 = 0.f;
    mNormPhase = 0.f;

    pHighpass = new OnePoleFilters(SAMPLERATE, 60.f, 0.f, OnePoleFilterType::HIGHPASS);

    mLowpassHiCut = _lowpassHiCut;
    mLowpassKeyTrk = _lowpassKeyTrk;
    mLowpassStateVar = 0.f;

    mNegPhase = 0.f;

    mPhaseMod = _phaseMod;
    mPhaseModMix_1 = _phaseModMix;
    mPhaseModMix_0 = 1.f - _phaseModMix;

    mDelayClipMin = SAMPLERATE / (mSampleBuffer.size() - 2);
    mDelaySamples = 0.f;

    mSampleBufferSize = mSampleBuffer.size() - 1;
    mSampleBufferIndex = 0;

    mSmootherMask = 0x0000;
    mABMix_ramp = 1.f;
    mPhaseModMix_ramp = 1.f;
    mPitchKeyTrk_ramp = 1.f;
}



/******************************************************************************/
/** Comb Filter Destructor
 * @brief
*******************************************************************************/

CombFilter::~CombFilter()
{
    delete pHighpass;
}



/******************************************************************************/
/** @brief    main function which applies the comb filter on the incoming
 *            samples from the Soundgenerators
*******************************************************************************/

void CombFilter::applyCombFilter(float _sampleA, float _sampleB)
{
    //****************************** Smoothing ******************************//
    if (mSmootherMask)
    {
        applySmoothers();
    }


    //**************************** AB Sample Mix ****************************//

    mCombFilterOut = _sampleA * mABMix_0 + _sampleB * mABMix_1;                   // mix of both incoming samples depending on select amount
    mCombFilterOut += mDecayStateVar;


    //****************** AB Ssample Phase Mdulation Mix ********************//

    float phaseMod = _sampleA * mPhaseModMix_0 + _sampleB * mPhaseModMix_1;       // mix of both incoming samples depending on modulation amount
    phaseMod *= mPhaseMod;


    //************************** 1-Pole Highpass ****************************//

    mCombFilterOut = pHighpass->applyFilter(mCombFilterOut);

    //*************************** 1-Pole Lowpass ****************************//

    mCombFilterOut = mCombFilterOut * (1.f - mLowpassCoeff_A1);
    mCombFilterOut += (mLowpassCoeff_A1 * mLowpassStateVar);

    mCombFilterOut += DNC_CONST;

    mLowpassStateVar = mCombFilterOut;


    //****************************** Allpass ********************************//

    float holdSample = mCombFilterOut;

    mCombFilterOut  = mCombFilterOut * mAllpassCoeff_2;
    mCombFilterOut += (mAllpassStateVar_1 * mAllpassCoeff_1);
    mCombFilterOut += mAllpassStateVar_2;

    mCombFilterOut -= (mAllpassStateVar_3 * mAllpassCoeff_1);
    mCombFilterOut -= (mAllpassStateVar_4 * mAllpassCoeff_2);

    mCombFilterOut += DNC_CONST;

    mAllpassStateVar_2 = mAllpassStateVar_1;
    mAllpassStateVar_1 = holdSample;

    mAllpassStateVar_4 = mAllpassStateVar_3;
    mAllpassStateVar_3 = mCombFilterOut;

    //****************************** Para D ********************************//

    if (fabs(mCombFilterOut) > 0.501187f)
    {
        if (mCombFilterOut > 0.f)
        {
            mCombFilterOut -= 0.501187f;
            float sampleState = mCombFilterOut;

            if (mCombFilterOut > 2.98815f)
            {
                mCombFilterOut = 2.98815f;
            }

            mCombFilterOut = mCombFilterOut * (1.f - mCombFilterOut * 0.167328f);

            mCombFilterOut *= 0.7488f;
            sampleState *= 0.2512f;

            mCombFilterOut = mCombFilterOut + sampleState + 0.501187f;
        }
        else
        {
            mCombFilterOut += 0.501187f;
            float sampleState = mCombFilterOut;

            if (mCombFilterOut < -2.98815f)
            {
                mCombFilterOut = -2.98815f;
            }

            mCombFilterOut = mCombFilterOut * (1.f - fabs(mCombFilterOut) * 0.167328f);

            mCombFilterOut *= 0.7488f;
            sampleState *= 0.2512f;

            mCombFilterOut = mCombFilterOut + sampleState - 0.501187f;
        }
    }


    //***************************** SmoothB ********************************//

    mDelaySamples -= mDelayStateVar;
    mDelaySamples *= 0.00577623f;
    mDelaySamples += mDelayStateVar;

    mDelayStateVar = mDelaySamples;

    /// Hier fehlt der Einfluss vom Envelope


    //****************************** Delay ********************************//

    mSampleBuffer[mSampleBufferIndex] = mCombFilterOut;                 // write into the SampleBuffer

    float modedDelaySamples = mDelaySamples * phaseMod + mDelaySamples;     // phM

    modedDelaySamples = modedDelaySamples - 1.f;

    if (modedDelaySamples > (mSampleBufferSize - 2))                // Clip 1, size-3
    {
        modedDelaySamples = mSampleBufferSize - 2;
    }
    else if (modedDelaySamples < 1.f)
    {
        modedDelaySamples = 1.f;
    }

    float delaySamples_int = round(modedDelaySamples - 0.5f);               // integer and fraction speration
    float delaySamples_fract = modedDelaySamples - delaySamples_int;

    int32_t ind_tm1 = delaySamples_int - 1;
    int32_t ind_t0  = delaySamples_int;
    int32_t ind_tp1 = delaySamples_int + 1;
    int32_t ind_tp2 = delaySamples_int + 2;

    ind_tm1 = mSampleBufferIndex - ind_tm1;
    ind_t0  = mSampleBufferIndex - ind_t0;
    ind_tp1 = mSampleBufferIndex - ind_tp1;
    ind_tp2 = mSampleBufferIndex - ind_tp2;

    ind_tm1 &= mSampleBufferSize;                             // Wrap with a mask sampleBuffer.size()-1
    ind_t0  &= mSampleBufferSize;
    ind_tp1 &= mSampleBufferSize;
    ind_tp2 &= mSampleBufferSize;

    mCombFilterOut = NlToolbox::Math::interpolRT(delaySamples_fract,          // Interpolation
                                                 mSampleBuffer[ind_tm1],
                                                 mSampleBuffer[ind_t0],
                                                 mSampleBuffer[ind_tp1],
                                                 mSampleBuffer[ind_tp2]);

    /// Hier wird am ende och mal mit EnvC multiplieziert
    /// mCombFilterOut *= env;

    mSampleBufferIndex = (mSampleBufferIndex + 1) & mSampleBufferSize;      // increase index and check boundaries


    //****************************** Decay ********************************//

    mDecayStateVar = mCombFilterOut * mDecayGain;
}



/******************************************************************************/
/** @brief    checks the mask, if any bit is 1, the smoother is applied fo the
 *            corrsponding value
*******************************************************************************/

void CombFilter::applySmoothers()
{
    //************************ ID 1: AB Sample Mix ***************************//
    if (mABMix_ramp < 1.0)
    {
        mABMix_ramp += SMOOTHER_INC;

        if (mABMix_ramp > 1.0)
        {
            mABMix_1 = mABMix_target;
            mABMix_0 = 1.f - mABMix_1;
            mSmootherMask &= 0xFFFE;                  // switch first bit to 0
        }
        else
        {
            mABMix_1 = mABMix_base + mABMix_diff * mABMix_ramp;
            mABMix_0 = 1.f - mABMix_1;
        }
    }


    //****************** ID 2: AB Sample Phase Mod Mix **********************//
    if (mPhaseModMix_ramp < 1.0)
    {
        mPhaseModMix_ramp += SMOOTHER_INC;

        if (mPhaseModMix_ramp > 1.0)
        {
            mPhaseModMix_1 = mPhaseModMix_target;
            mPhaseModMix_0 = 1.f - mPhaseModMix_1;
            mSmootherMask &= 0xFFFD;                  // switch second bit to 0
        }
        else
        {
            mPhaseModMix_1 = mPhaseModMix_base + mPhaseModMix_diff * mPhaseModMix_ramp;
            mPhaseModMix_0 = 1.f - mPhaseModMix_1;
        }
    }


    //******************** ID 3: Pitch  Key Tracking ************************//
    if (mPitchKeyTrk_ramp < 1.0)
    {
        mPitchKeyTrk_ramp += SMOOTHER_INC;

        if (mPitchKeyTrk_ramp > 1.0)
        {
            mPitchKeyTrk = mPitchKeyTrk_target;
            mSmootherMask &= 0xFFFB;                  // switch third bit to 0
        }
        else
        {
            mPitchKeyTrk = mPitchKeyTrk_base + mPitchKeyTrk_diff * mPitchKeyTrk_ramp;
            setMainFreq();
        }
    }

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
        case CtrlId::ABSAMPLE_MIX:
            _ctrlVal = _ctrlVal / 126.f;

            if (_ctrlVal > 1.0)
            {
                _ctrlVal = 1.0;
            }

            printf("Comb Filter - AB Mix: %f\n", _ctrlVal);

            // Set Smoothing Ramp
            mABMix_target = NlToolbox::Curves::applySineCurve(_ctrlVal);
            mABMix_base = mABMix_1;
            mABMix_diff = mABMix_target - mABMix_base;
            mABMix_ramp = 0.f;

            mSmootherMask |= 0x0001;      // ID: 1
            break;

        case CtrlId::PITCH_EDIT:
            _ctrlVal = (_ctrlVal / 126.f) * 120.f;

            if (_ctrlVal > 120.f)
            {
                _ctrlVal = 119.99f;
            }

            printf("Comb Filter - Pitch Edit: %f\n", _ctrlVal);

            mPitchEdit = _ctrlVal;
            setMainFreq();
            break;

        case CtrlId::PITCH_KEYTRACKING:
            _ctrlVal = (_ctrlVal / 127.f) * 1.05f;

            printf("Comb Filter - Pitch Key Trk: %f\n", _ctrlVal);

            // Set Smoothing Ramp
            mPitchKeyTrk_target = _ctrlVal;
            mPitchKeyTrk_base = mPitchKeyTrk;
            mPitchKeyTrk_diff = mPitchKeyTrk_target - mPitchKeyTrk_base;
            mPitchKeyTrk_ramp = 0.f;

            mSmootherMask |= 0x0004;      // ID: 3
            break;

#if 0
        case CtrlId::PITCH_ENVCAMOUNT:
            _ctrlVal = (_ctrlVal / 0.7875f) - 80.f;

            if (_ctrlVal > 80.f)
            {
                _ctrlVal = 80.f;
            }

            printf("Pitch EnvC: %f\n", _ctrlVal);

            mPitchEnvC = _ctrlVal;
            break;
#endif

        case CtrlId::DECAY:
            _ctrlVal = (_ctrlVal / .63f) - 100.f;

            if (_ctrlVal > 100.f)
            {
                _ctrlVal = 100.f;
            }

            printf("Comb Filter - Decay Time: %f\n", _ctrlVal);

            mDecay = _ctrlVal;
            setDecayGain();
            break;

        case CtrlId::DECAY_KEYTRACKING:
            _ctrlVal = _ctrlVal / 127.f;

            printf("Comb Filter - Decay Key Trk: %f\n", _ctrlVal);

            mDecayKeyTrk = _ctrlVal;
            setDecayGain();
            break;
#if 0
        case CtrlId::DECAY_GATE:
            _ctrlVal = _ctrlVal / 127.f;

            printf("Decay Gate Amnt: %f\n", _ctrlVal);

            mDecayGate = _ctrlVal;
            setDecayGain();
            break;
#endif
        case CtrlId::ALLPASS_FREQ:
            _ctrlVal = _ctrlVal/ 0.9f;

            if (_ctrlVal > 140.f)
            {
                _ctrlVal = 140.f;
            }

            printf("Comb Filter - Allpass Frequency: %f\n", _ctrlVal);

            mAllpassTune = _ctrlVal;
            setAllpassFreq();
            break;

        case CtrlId::ALLPASS_RES:
            _ctrlVal = _ctrlVal / 127.f;

            printf("Comb Filter - Allpass Resonance: %f\n", _ctrlVal);

            mAllpassRes = (_ctrlVal * 1.99f) + -1.f;
            setAllpassFreq();
            break;

        case CtrlId::ALLPASS_KEYTRACKING:
            if (_ctrlVal > 80.f)
            {
                _ctrlVal = 80.f;
            }

            _ctrlVal = _ctrlVal / 127.f;

            printf("Comb Filter - Allpass Key Trk: %f\n", _ctrlVal);

            mAllpassKeyTrk = _ctrlVal;
            setAllpassFreq();
            break;
#if 0
        case CtrlId::ALLPASS_ENVCAMOUNT:
            _ctrlVal = (_ctrlVal / 0.7875f) - 80.f;

            if (_ctrlVal > 80.f)
            {
                _ctrlVal = 80.f;
            }

            printf("Allpass EnvC: %f\n", _ctrlVal);

            float mAllpassEnvC =  _ctrlVal;
            break;
#endif
        case CtrlId::LOWPASS_FREQ:
            _ctrlVal = (_ctrlVal / 1.27f) + 40.f;

            printf("Comb Filter - Lowpass Frequency: %f\n", _ctrlVal);

            mLowpassHiCut = _ctrlVal;
            setLowpassFreq();
            break;

        case CtrlId::LOWPASS_KEYTRACKING:
            _ctrlVal = _ctrlVal / 127.f;

            printf("Comb Filter - Lowpass Key Trk: %f\n", _ctrlVal);

            mLowpassKeyTrk = _ctrlVal;
            setLowpassFreq();
            break;
#if 0
        case CtrlId::LOWPASS_ENVCAMOUNT:
            _ctrlVal = (_ctrlVal / 0.7875f) - 100.f;

            if (_ctrlVal > 100.f)
            {
                _ctrlVal = 100.f;
            }
            printf("Lowpass EnvC: %f\n", _ctrlVal);

            mLowpassEnvC = _ctrlVal;
            setLowpassFreq();
            break;
#endif

        case CtrlId::PHASEMOD:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }
            if (_ctrlVal > 80.f)
            {
                _ctrlVal = 80.f;
            }


            printf("Comb Filter - Phase Mod Amount %f\n", _ctrlVal);

            mPhaseMod = _ctrlVal * fabs(_ctrlVal) * 0.9f;
            break;

        case CtrlId::PHASEMOD_MIX:
            _ctrlVal = (_ctrlVal / 126.f);

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("Comb Filter - Phase Mod Mix %f\n", _ctrlVal);

            // Set Smoothing Ramp
            mPhaseModMix_target = _ctrlVal;
            mPhaseModMix_base = mPhaseModMix_1;
            mPhaseModMix_diff = mPhaseModMix_target - mPhaseModMix_base;
            mPhaseModMix_ramp = 0.f;

            mSmootherMask |= 0x0002;      // ID: 2
            break;
    }
}



/*****************************************************************************/
/** @brief
******************************************************************************/

void CombFilter::setPitch(float _pitch)
{
    mPitch = _pitch;

    setMainFreq();              // Calculate mMainFreq and dependant variables
}



/*****************************************************************************/
/** @brief
******************************************************************************/

void CombFilter::setMainFreq()
{
    mMainFreq = NlToolbox::Conversion::pitch2freq(mPitch * mPitchKeyTrk + mPitchEdit);

    setLowpassFreq();                   // calculates mNegPhase, with new mMainFreq
    setAllpassFreq();                   // calculates mNormPhase, with new mMainFreq

    setDecayGain();                     // calculates mDecayGain, with new mMainFreq

    pHighpass->setCutFreq(mMainFreq * 0.125f);
}



/*****************************************************************************/
/** @brief  setter function for highcut frequency of the internal lowpass filter.
 *          the frequency is calculated depending on set HiCut, Key Tracking
 *          and modulation amount of the Envelope C
******************************************************************************/

void CombFilter::setLowpassFreq()
{
    float cutFrequency = ((mPitch * mLowpassKeyTrk) + mLowpassHiCut); /// + (mEnv * mLowpassEnvC);      // Cut Frequency Calculation

    cutFrequency = NlToolbox::Conversion::pitch2freq(cutFrequency);

    if (cutFrequency > FREQCLIP_12000HZ)           // Clip check Max
    {
        cutFrequency = FREQCLIP_12000HZ;
    }

    if (cutFrequency < FREQCLIP_2HZ)           // Clip check Min
    {
        cutFrequency = FREQCLIP_2HZ;
    }


    //********************* Lowpass coefficient calculation *****************//

    cutFrequency = cutFrequency * (WARPCONST_PI);

    cutFrequency *= 0.159155f;                          // 2Pi wrap
    cutFrequency = cutFrequency - round(cutFrequency);
    cutFrequency *= 6.28319f;

    float omegaTan = NlToolbox::Math::sin(cutFrequency) / NlToolbox::Math::cos(cutFrequency);     // tan -pi..pi

    mLowpassCoeff_A1 = (1.f - omegaTan) / (1.f + omegaTan);


    //********************** Negative Phase Calculation ********************//

    float w_norm = mMainFreq * (1.f / SAMPLERATE);

    float stateVar_r = NlToolbox::Math::sinP3(w_norm);
    float stateVar_i = NlToolbox::Math::sinP3(w_norm + 0.25f);

    stateVar_r = stateVar_r * mLowpassCoeff_A1;
    stateVar_i = stateVar_i * -mLowpassCoeff_A1 + 1.f;

    mNegPhase = NlToolbox::Math::arctan(stateVar_r / stateVar_i) * (1.f / -6.28319f);

    setDelayTime();                             // Delay Samples must be calculated anew
}



/*****************************************************************************/
/** @brief
******************************************************************************/

void CombFilter::setAllpassFreq()
{
    float cutFrequency = (mPitch * mAllpassKeyTrk) + mAllpassTune; /// + (mEnv * mLowpassEnvC);     // Cut Frequency Calculation

    cutFrequency = NlToolbox::Conversion::pitch2freq(cutFrequency);

    if (cutFrequency > FREQCLIP_22000HZ)           // Clip check Max
    {
        cutFrequency = FREQCLIP_22000HZ;
    }

    if (cutFrequency < FREQCLIP_2HZ)           // Clip check Min
    {
        cutFrequency = FREQCLIP_2HZ;
    }


    //******************** Allpass coefficient calculation ******************//

    float omega = cutFrequency * (WARPCONST_2PI);

    float omegaSin = NlToolbox::Math::sin(omega);
    float omegaCos = NlToolbox::Math::cos(omega);

    float alpha = omegaSin * (1.f - mAllpassRes);
    float normVar = 1.f / (1.f + alpha);

    mAllpassCoeff_1 = (-2.f * omegaCos) * normVar;
    mAllpassCoeff_2 =  (1.f - alpha) * normVar;


    //******************** Norm Phase Calculation ***************************//

    float w_norm = mMainFreq * (SAMPLING_INTERVAL);

    float stateVar1_i = NlToolbox::Math::sinP3(w_norm);
    float stateVar1_r = NlToolbox::Math::sinP3(w_norm + 0.25f);

    float stateVar2_i = NlToolbox::Math::sinP3(w_norm + w_norm);
    float stateVar2_r = NlToolbox::Math::sinP3(w_norm + w_norm + 0.25f);

    float var1_r = (mAllpassCoeff_1 * stateVar1_r) + stateVar2_r + mAllpassCoeff_2;
    float var1_i = (-1.f * mAllpassCoeff_1 * stateVar1_i) - stateVar2_i;

    float var2_r = (mAllpassCoeff_1 * stateVar1_r) + (mAllpassCoeff_2 * stateVar2_r) + 1.f;
    float var2_i = ((-1.f * mAllpassCoeff_1 * stateVar1_i) - (mAllpassCoeff_2 * stateVar2_i)) * -1.f;

    float var_X = (var1_r * var2_r) - (var1_i * var2_i);        // kmplx mul
    float var_Y = (var1_r * var2_i) + (var2_r * var1_i);

    if (var_X > 0.f)                                            // safe
    {
        var_X += 1e-12;
    }
    else
    {
        var_X -= 1e-12;
    }

    mNormPhase = NlToolbox::Math::arctan(var_Y / var_X);        // arctan

    if (var_X < 0.f)
    {
        if (var_Y > 0.f)
        {
            mNormPhase += 3.14159f;
        }
        else
        {
            mNormPhase -= 3.14159f;
        }
    }

    if (mNormPhase > 0.f)                                       // forced unwrap > 0
    {
        mNormPhase += -6.28319f;
    }

    mNormPhase *= 0.159155f;

    setDelayTime();                                             // Delay Samples must be calculated anew
}



/*****************************************************************************/
/** @brief
******************************************************************************/

void CombFilter::setDecayGain()
{
    float sign;

    if (mDecay < 0.f)
    {
        sign = -1.f;
    }
    else
    {
        sign = 1.f;
    }

    float decayTime = mPitch * -0.5 * mDecayKeyTrk + fabs(mDecay);  /// hier fehlt noch der Gate-Faktor

    decayTime = NlToolbox::Conversion::db2af(decayTime);
    decayTime *= 0.001f;

    decayTime *= sign;


    //********************************* g ************************************//

    if (decayTime < 0.f)
    {
        sign = -1.f;
    }
    else
    {
        sign = 1.f;
    }

    mDecayGain = fabs(decayTime) * mMainFreq;

    if (mDecayGain < DNC_CONST)         // Min-Clip
    {
        mDecayGain = DNC_CONST;
    }

    mDecayGain = (1.f / mDecayGain) * -6.2831f;

    if (mDecayGain > 0)                 // Exp Clipped
    {
        mDecayGain = 1.f;
    }
    else if (-27.631f > mDecayGain)
    {
        mDecayGain = 0.f;
    }
    else
    {
        mDecayGain = pow(2.71828f, mDecayGain);
    }

    mDecayGain *= sign;
}



/*****************************************************************************/
/** @brief
******************************************************************************/

void CombFilter::setDelayTime()
{
    if (mMainFreq < mDelayClipMin)
    {
        mDelaySamples = SAMPLERATE / mDelayClipMin;
    }
    else
    {
        mDelaySamples = SAMPLERATE / mMainFreq;
    }

    mDelaySamples = mDelaySamples * mNegPhase + mDelaySamples;      // Lowpass influence

    mDelaySamples = mDelaySamples * mNormPhase + mDelaySamples;     // Allpass influence

    mDelayStateVar = mDelaySamples;     // set Delay State Variable for the smoothB
}
