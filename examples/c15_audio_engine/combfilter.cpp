/******************************************************************************/
/** @file		combfilter.cpp
    @date		2017-05-18
    @version	1.0
    @author		Anton Schmied[2016-12-26]
    @brief		Comb Filter Class member and method definitions
*******************************************************************************/

#include "combfilter.h"

/******************************************************************************/
/** Combfilter Default Constructor
 * @brief   initialization of the modules local variabels with default values
 *          SampleRate:             48 kHz
 *          AB Mix:                 0.5
 *          Pitch Edit:             60.00
 *          Pitch EnvC:             0
 *          Pitch Keytracking:      1
 *          Decay:                  0
 *          Decay Gate:             0
 *          Decay Key Tracking:     0.33
 *          AP Tune:                140
 *          AP Tune EnvC:           0
 *          AP Tune Key Traking:    1
 *          AP Resonance:           0.5
 *          HI Cut:                 140
 *          HI Cut EnvC:            0
 *          HI Cut Key Traking:     1
 *          Phase Modulation:       0
 *          Phase Mod Mix:          0.5
*******************************************************************************/

CombFilter::CombFilter()
{
    //******************************* Outputs ********************************//
    mCombFilterOut = 0.f;

    //******************************** AB Mix ********************************//
    mABMix_1 = 0.5f;
    mABMix_0 = 0.5f;

    //********************************* Pitch ********************************//
    mPitchEdit = 60.f;
    mPitchKeyTrk = 1.f;

    //********************************* Decay ********************************//
    mDecay = 0.f;
    mDecayKeyTrk = 0.33f;
    mDecayStateVar = 0.f;

    //***************************** Highpass *********************************//
    pHighpass = new OnePoleFilters(60.f, 0.f, OnePoleFilterType::HIGHPASS);

    //****************************** Allpass *********************************//
    mAllpassTune = 140.f;
    mAllpassKeyTrk = 1.f;
    mAllpassRes = (0.5f * 1.99f) + -1.f;
    mAllpassStateVar_1 = 0.f;
    mAllpassStateVar_2 = 0.f;
    mAllpassStateVar_3 = 0.f;
    mAllpassStateVar_4 = 0.f;
    mNormPhase = 0.f;

    //***************************** Lowpass **********************************//
    mLowpassHiCut = 140.f;
    mLowpassKeyTrk = 1.f;
    mLowpassStateVar = 0.f;

    mNegPhase = 0.f;

    //*************************** Phase MOd **********************************//
    mPhaseMod = 0.f;
    mPhaseModMix_1 = 0.5f;
    mPhaseModMix_0 = 0.5f;

    //***************************** Delay ************************************//
    mSampleBufferIndex = 0;
    mSampleBuffer = {0.f};

    mDelayClipMin = SAMPLERATE / (COMB_BUFFERSIZE - 2);
    mDelaySamples = 0.f;
    mDelayStateVar = 0.f;

    //******************************* Smoothing ******************************//
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
    //******************************* Outputs ********************************//
    mCombFilterOut = 0.f;

    //******************************** AB Mix ********************************//
    mABMix_1 = NlToolbox::Curves::applySineCurve(_ABMix);
    mABMix_0 = 1.f - mABMix_1;

    //********************************* Pitch ********************************//
    if (_pitchEdit > 120.f)
    {
        _pitchEdit = 119.99f;
    }
    mPitchEdit = _pitchEdit;
    mPitchKeyTrk = _pitchKeyTrk;

    //********************************* Decay ********************************//
    mDecay = _decay;
    mDecayKeyTrk = _decayKeyTrk;
    mDecayStateVar = 0.f;

    //***************************** Highpass *********************************//
    pHighpass = new OnePoleFilters(60.f, 0.f, OnePoleFilterType::HIGHPASS);

    //****************************** Allpass *********************************//
    mAllpassTune = _allpassTune;
    mAllpassKeyTrk = _allpassKeyTrk;
    mAllpassRes = (_allpassRes * 1.99f) + -1.f;
    mAllpassStateVar_1 = 0.f;
    mAllpassStateVar_2 = 0.f;
    mAllpassStateVar_3 = 0.f;
    mAllpassStateVar_4 = 0.f;
    mNormPhase = 0.f;

    //***************************** Lowpass **********************************//
    mLowpassHiCut = _lowpassHiCut;
    mLowpassKeyTrk = _lowpassKeyTrk;
    mLowpassStateVar = 0.f;

    mNegPhase = 0.f;

    //*************************** Phase MOd **********************************//
    mPhaseMod = _phaseMod * fabs(_phaseMod) * 0.9f;
    mPhaseModMix_1 = _phaseModMix;
    mPhaseModMix_0 = 1.f - _phaseModMix;

    //***************************** Delay ************************************//
    mSampleBufferIndex = 0;
    mSampleBuffer = {0.f};

    mDelayClipMin = SAMPLERATE / (COMB_BUFFERSIZE - 2);
    mDelaySamples = 0.f;
    mDelayStateVar = 0.f;

    //******************************* Smoothing ******************************//
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
    float sampleVar = 0.f;                              // for multiple Purposes

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
    sampleVar = mCombFilterOut;

    mCombFilterOut  = mCombFilterOut * mAllpassCoeff_2;
    mCombFilterOut += (mAllpassStateVar_1 * mAllpassCoeff_1);
    mCombFilterOut += mAllpassStateVar_2;

    mCombFilterOut -= (mAllpassStateVar_3 * mAllpassCoeff_1);
    mCombFilterOut -= (mAllpassStateVar_4 * mAllpassCoeff_2);

    mCombFilterOut += DNC_CONST;

    mAllpassStateVar_2 = mAllpassStateVar_1;
    mAllpassStateVar_1 = sampleVar;

    mAllpassStateVar_4 = mAllpassStateVar_3;
    mAllpassStateVar_3 = mCombFilterOut;


    //****************************** Para D ********************************//
    if (fabs(mCombFilterOut) > 0.501187f)
    {
        if (mCombFilterOut > 0.f)
        {
            mCombFilterOut -= 0.501187f;
            sampleVar = mCombFilterOut;

            if (mCombFilterOut > 2.98815f)
            {
                mCombFilterOut = 2.98815f;
            }

            mCombFilterOut = mCombFilterOut * (1.f - mCombFilterOut * 0.167328f);

            mCombFilterOut *= 0.7488f;
            sampleVar *= 0.2512f;

            mCombFilterOut = mCombFilterOut + sampleVar + 0.501187f;
        }
        else
        {
            mCombFilterOut += 0.501187f;
            sampleVar = mCombFilterOut;

            if (mCombFilterOut < -2.98815f)
            {
                mCombFilterOut = -2.98815f;
            }

            mCombFilterOut = mCombFilterOut * (1.f - fabs(mCombFilterOut) * 0.167328f);

            mCombFilterOut *= 0.7488f;
            sampleVar *= 0.2512f;

            mCombFilterOut = mCombFilterOut + sampleVar - 0.501187f;
        }
    }


    //***************************** SmoothB ********************************//
    sampleVar = mDelaySamples - mDelayStateVar;
    sampleVar *= 0.00577623f;
    sampleVar += mDelayStateVar;

    mDelayStateVar = sampleVar;

    /// Hier fehlt der Einfluss vom Envelope

    //****************************** Delay ********************************//
    mSampleBuffer[mSampleBufferIndex] = mCombFilterOut;             // write into the SampleBuffer

    sampleVar = sampleVar * phaseMod + sampleVar;   // phM
    sampleVar = sampleVar - 1.f;

    if (sampleVar > COMB_BUFFERSIZE_M3)                // Clip 1, size-3
    {
        sampleVar = COMB_BUFFERSIZE_M3;
    }
    else if (sampleVar < 1.f)
    {
        sampleVar = 1.f;
    }

    float delaySamples_int = round(sampleVar - 0.5f);               // integer and fraction speration
    float delaySamples_fract = sampleVar - delaySamples_int;

    int32_t ind_tm1 = delaySamples_int - 1;
    int32_t ind_t0  = delaySamples_int;
    int32_t ind_tp1 = delaySamples_int + 1;
    int32_t ind_tp2 = delaySamples_int + 2;

    ind_tm1 = mSampleBufferIndex - ind_tm1;
    ind_t0  = mSampleBufferIndex - ind_t0;
    ind_tp1 = mSampleBufferIndex - ind_tp1;
    ind_tp2 = mSampleBufferIndex - ind_tp2;

    ind_tm1 &= COMB_BUFFERSIZE_M1;                             // Wrap with a mask sampleBuffer.size()-1
    ind_t0  &= COMB_BUFFERSIZE_M1;
    ind_tp1 &= COMB_BUFFERSIZE_M1;
    ind_tp2 &= COMB_BUFFERSIZE_M1;

    mCombFilterOut = NlToolbox::Math::interpolRT(delaySamples_fract,          // Interpolation
                                                 mSampleBuffer[ind_tm1],
                                                 mSampleBuffer[ind_t0],
                                                 mSampleBuffer[ind_tp1],
                                                 mSampleBuffer[ind_tp2]);

    /// Hier wird am ende och mal mit EnvC multiplieziert
    /// mCombFilterOut *= env;

    mSampleBufferIndex = (mSampleBufferIndex + 1) & COMB_BUFFERSIZE_M1;      // increase index and check boundaries


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
            mSmootherMask &= 0xFFFE;                  // switch first bit to 0
        }
        else
        {
            mABMix_1 = mABMix_base + mABMix_diff * mABMix_ramp;
        }

        mABMix_0 = 1.f - mABMix_1;
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
        }
        calcMainFreq();
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
            calcMainFreq();
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
            calcDecayGain();
            break;

        case CtrlId::DECAY_KEYTRACKING:
            _ctrlVal = _ctrlVal / 127.f;

            printf("Comb Filter - Decay Key Trk: %f\n", _ctrlVal);

            mDecayKeyTrk = _ctrlVal;
            calcDecayGain();
            break;
#if 0
        case CtrlId::DECAY_GATE:
            _ctrlVal = _ctrlVal / 127.f;

            printf("Decay Gate Amnt: %f\n", _ctrlVal);

            mDecayGate = _ctrlVal;
            calcDecayGain();
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
            calcAllpassFreq();
            break;

        case CtrlId::ALLPASS_RES:
            _ctrlVal = _ctrlVal / 127.f;

            printf("Comb Filter - Allpass Resonance: %f\n", _ctrlVal);

            mAllpassRes = (_ctrlVal * 1.99f) + -1.f;
            calcAllpassFreq();
            break;

        case CtrlId::ALLPASS_KEYTRACKING:
            if (_ctrlVal > 80.f)
            {
                _ctrlVal = 80.f;
            }

            _ctrlVal = _ctrlVal / 127.f;

            printf("Comb Filter - Allpass Key Trk: %f\n", _ctrlVal);

            mAllpassKeyTrk = _ctrlVal;
            calcAllpassFreq();
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
            calcAllpassFreq();
            break;
#endif
        case CtrlId::LOWPASS_FREQ:
            _ctrlVal = (_ctrlVal / 1.27f) + 40.f;

            printf("Comb Filter - Lowpass Frequency: %f\n", _ctrlVal);

            mLowpassHiCut = _ctrlVal;
            calcLowpassFreq();
            break;

        case CtrlId::LOWPASS_KEYTRACKING:
            _ctrlVal = _ctrlVal / 127.f;

            printf("Comb Filter - Lowpass Key Trk: %f\n", _ctrlVal);

            mLowpassKeyTrk = _ctrlVal;
            calcLowpassFreq();
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
            calcLowpassFreq();
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
/** @brief  Once a "Note On" event occurs, the Pitch of the currently
 *          played is updated and the calcMainFre() is called to recalculate
 *          the main frequency.
 *          Also the "SmootherB" is triggerd, setting the state variable
 *          with latest calculated delaysamles number
******************************************************************************/

void CombFilter::setPitch(float _pitch)
{
    mPitch = _pitch;
    calcMainFreq();
    mDelayStateVar = mDelaySamples;
}



/*****************************************************************************/
/** @brief  Main frequency calculation, which is dependant on Pitch,
 *          Pitch Key tracking and Pitch Edit. Also serves as trigger
 *          to recalculate the fitler coefficients and decay gain anew
******************************************************************************/

void CombFilter::calcMainFreq()
{
    mMainFreq = NlToolbox::Conversion::pitch2freq(mPitch * mPitchKeyTrk + mPitchEdit);

    calcLowpassFreq();                   // calculates mNegPhase, with new mMainFreq
    calcAllpassFreq();                   // calculates mNormPhase, with new mMainFreq

    calcDecayGain();                     // calculates mDecayGain, with new mMainFreq

    pHighpass->setCutFreq(mMainFreq * 0.125f);
}



/*****************************************************************************/
/** @brief  Calculation of the highcut frequency of the internal lowpass filter.
 *          The frequency is dependant on the Note Pitch, Key Tracking amount
 *          and the set Cut Frequncy of the filter itself
 *          Also triggers the calculation of the new Delaytime, since
 *          the filter coefficents affect it
******************************************************************************/

void CombFilter::calcLowpassFreq()
{
    float tmpVar = ((mPitch * mLowpassKeyTrk) + mLowpassHiCut); /// + (mEnv * mLowpassEnvC);      // Cut Frequency Calculation

    tmpVar = NlToolbox::Conversion::pitch2freq(tmpVar);

    if (tmpVar > FREQCLIP_MIN_2)           // Clip check Max
    {
        tmpVar = FREQCLIP_MIN_2;
    }

    if (tmpVar < FREQCLIP_MAX_1)           // Clip check Min
    {
        tmpVar = FREQCLIP_MAX_1;
    }


    //********************* Lowpass coefficient calculation *****************//
    tmpVar = tmpVar * WARPCONST_PI;

    tmpVar *= 0.159155f;                          // 2Pi wrap
    tmpVar = tmpVar - round(tmpVar);
    tmpVar *= 6.28319f;

    tmpVar = NlToolbox::Math::sin(tmpVar) / NlToolbox::Math::cos(tmpVar);     // tan -pi..pi

    mLowpassCoeff_A1 = (1.f - tmpVar) / (1.f + tmpVar);


    //********************** Negative Phase Calculation ********************//
    tmpVar = mMainFreq * SAMPLE_INTERVAL;

    float stateVar_r = NlToolbox::Math::sinP3(tmpVar);
    float stateVar_i = NlToolbox::Math::sinP3(tmpVar + 0.25f);

    stateVar_r = stateVar_r * mLowpassCoeff_A1;
    stateVar_i = stateVar_i * -mLowpassCoeff_A1 + 1.f;

    mNegPhase = NlToolbox::Math::arctan(stateVar_r / stateVar_i) * (1.f / -CONST_DOUBLE_PI);

    calcDelayTime();
}



/*****************************************************************************/
/** @brief  Calculation of the cut frequency of the internal allpass filter.
 *          The frequency is dependant on Note Pitch, Key Tracking amount
 *          and the set Cut Frequncy of the filter itself
 *          Also triggers the calculation of the new Delaytime, since
 *          the filter coefficents affect it
******************************************************************************/

void CombFilter::calcAllpassFreq()
{
    float tmpVar = (mPitch * mAllpassKeyTrk) + mAllpassTune; /// + (mEnv * mLowpassEnvC);     // Cut Frequency Calculation

    tmpVar = NlToolbox::Conversion::pitch2freq(tmpVar);

    if (tmpVar > FREQCLIP_MIN_2)           // Clip check Max
    {
        tmpVar = FREQCLIP_MIN_2;
    }

    if (tmpVar < FREQCLIP_MAX_1)           // Clip check Min
    {
        tmpVar = FREQCLIP_MAX_1;
    }


    //******************** Allpass coefficient calculation ******************//
    tmpVar = tmpVar * WARPCONST_2PI;

//    float omegaSin = NlToolbox::Math::sin(tmpVar);
//    float omegaCos = NlToolbox::Math::cos(tmpVar);

    float alpha = NlToolbox::Math::sin(tmpVar) * (1.f - mAllpassRes);
    float normVar = 1.f / (1.f + alpha);

    mAllpassCoeff_1 = (-2.f * NlToolbox::Math::cos(tmpVar)) * normVar;
    mAllpassCoeff_2 =  (1.f - alpha) * normVar;


    //******************** Norm Phase Calculation ***************************//
    tmpVar = mMainFreq * SAMPLE_INTERVAL;

    float stateVar1_i = NlToolbox::Math::sinP3(tmpVar) * -1.f * mAllpassCoeff_1;
    float stateVar2_i = NlToolbox::Math::sinP3(tmpVar + tmpVar);
    float stateVar1_r = NlToolbox::Math::sinP3(tmpVar + 0.25f) * mAllpassCoeff_1;
    float stateVar2_r = NlToolbox::Math::sinP3(tmpVar + tmpVar + 0.25f);


    float var1_i = stateVar1_i - stateVar2_i;
    float var2_i = (stateVar1_i - (mAllpassCoeff_2 * stateVar2_i)) * -1.f;
    float var1_r = stateVar1_r + stateVar2_r + mAllpassCoeff_2;
    float var2_r = stateVar1_r + (stateVar2_r * mAllpassCoeff_2) + 1.f;


    stateVar1_i = (var1_r * var2_r) - (var1_i * var2_i);        // kmplx mul
    stateVar1_r = (var1_r * var2_i) + (var2_r * var1_i);

    if (stateVar1_i > 0.f)                                            // safe
    {
        stateVar1_i += 1e-12;
    }
    else
    {
        stateVar1_i -= 1e-12;
    }

    mNormPhase = NlToolbox::Math::arctan(stateVar1_r / stateVar1_i);        // arctan

    if (stateVar1_i < 0.f)
    {
        if (stateVar1_r > 0.f)
        {
            mNormPhase += CONST_PI;
        }
        else
        {
            mNormPhase -= CONST_PI;
        }
    }

    if (mNormPhase > 0.f)                                       // forced unwrap > 0
    {
        mNormPhase += -CONST_DOUBLE_PI;
    }

    mNormPhase *= 0.159155f;

    calcDelayTime();
}



/*****************************************************************************/
/** @brief  Decay Gain Calculation wich is depenadant on Pitch, Key Tracking
 *          Amount and the Main Frequency
******************************************************************************/

void CombFilter::calcDecayGain()
{
//    float sign;

//    if (mDecay < 0.f)
//    {
//        sign = -1.f;
//    }
//    else
//    {
//        sign = 1.f;
//    }

    float decayTime = mPitch * -0.5 * mDecayKeyTrk + fabs(mDecay);  /// hier fehlt noch der Gate-Faktor

    decayTime = NlToolbox::Conversion::db2af(decayTime);
    decayTime *= 0.001f;

//    decayTime *= sign;
    if (mDecay < 0.f)
    {
        decayTime *= -1.f;
    }


    //********************************* g ************************************//
//    if (decayTime < 0.f)
//    {
//        sign = -1.f;
//    }
//    else
//    {
//        sign = 1.f;
//    }

    mDecayGain = fabs(decayTime) * mMainFreq;

    if (mDecayGain < DNC_CONST)         // Min-Clip
    {
        mDecayGain = DNC_CONST;
    }

    mDecayGain = (1.f / mDecayGain) * -CONST_DOUBLE_PI;

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

//    mDecayGain *= sign;
    if (decayTime < 0.f)
    {
        mDecayGain *= -1.f;
    }
}



/*****************************************************************************/
/** @brief  Delay Time Calculation which is dependant on the MainFrequency
 *          (Clipping) and the previously calculated phase modulation amounts
 *          of the Lowpass and Allpass filter.
******************************************************************************/

void CombFilter::calcDelayTime()
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
}
