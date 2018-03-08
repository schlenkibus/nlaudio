/******************************************************************************/
/** @file		statevariablefilter.cpp
    @date		2016-02-16
    @version	0.1
    @author		Anton Schmied[2016-12-26]
    @brief		State Variable Filter Class member and method definitions
*******************************************************************************/

#include "statevariablefilter.h"

/******************************************************************************/
/** Outputmixer Default Constructor
 * @brief    initialization of the modules local variabels with default values
*******************************************************************************/

StateVariableFilter::StateVariableFilter()
{
    //******************************* Output *********************************//
    mSVFilterOut= 0.f;


    mABMix_1 = 0.5f;
    mABMix_0 = 1.f - mABMix_1;
    mCombMix_1 = 0.f;
    mCombMix_0 = 1.f - fabs(mCombMix_1);

    mCutPitch = 80.f;
    mCutKeyTracking = 0.f;

    mResonance = 0.5f;
    mResonanceKeyTracking = 0.f;

    mSpread = 0.f;

    mFreqMod = 0.f;
    mFreqModMix_1 = 0.5f;
    mFreqModMix_0 = 1.f - mFreqModMix_1;

    calcCutFrequency();
    calcAttenuation();

    mFirst_Prefade = 0.f;
    mSecond_Prefade = 1.f;
    mFirst_Postfade = 0.f;
    mSecond_Postfade = 1.f;

    mFirst_LPMix = 1.f;
    mFirst_BPMix = 0.f;
    mFirst_HPMix = 0.f;

    mSecond_LPMix = 1.f;
    mSecond_BPMix = 0.f;
    mSecond_HPMix = 0.f;

    //****************************** Smoothing *******************************//
    mSmootherMask = 0x0000;

    mABMix_ramp = 1.f;
    mCombMix_ramp = 1.f;
    mFreqModMix_ramp = 1.f;
    mCutPitch_ramp = 1.f;
    mFirst_Filtertype_ramp = 1.f;
    mSecond_Filtertype_ramp = 1.f;
    mFade_ramp = 1.f;
}



/******************************************************************************/
/** State Variable Filter Parameterized Constructor
 * @brief    initialization of the modules local variabels with custom
 *           parameters
*******************************************************************************/

StateVariableFilter::StateVariableFilter(float _ABMix,
                                         float _combMix,
                                         float _cutoffPitch,
                                         float _cutoffKeyTrk,
                                         float _resonance,
                                         float _resonanceKeyTrk,
                                         float _spread,
                                         float _filterType,
                                         float _parallel,
                                         float _freqMod,
                                         float _freqModABMix)
{
    //******************************* Output *********************************//
    mSVFilterOut= 0.f;

    mABMix_1 = _ABMix;
    mABMix_0 = 1.f - _ABMix;
    mCombMix_1 = _combMix;
    mCombMix_0 = 1.f - fabs(_combMix);

    mCutPitch = _cutoffPitch;
    mCutKeyTracking = _cutoffKeyTrk;

    mResonance = _resonance;
    mResonanceKeyTracking = _resonanceKeyTrk;

    mSpread = _spread;

    mFreqMod = _freqMod;
    mFreqModMix_1 = _freqModABMix;
    mFreqModMix_0 = 1.f - _freqModABMix;

    calcCutFrequency();
    calcAttenuation();

    mFirst_Prefade = fabs(_parallel);
    mSecond_Prefade = 1.f - mFirst_Prefade;
    mFirst_Postfade = 0.7f * mFirst_Prefade;
    mSecond_Postfade = 0.7f * _parallel + mSecond_Prefade;

    if (_filterType < 1.f)
    {
        mFirst_LPMix = 1.f;
        mFirst_BPMix = 0.f;
        mFirst_HPMix = 0.f;

        _filterType *= 2.f;

        if (_filterType < 1.f)
        {
            mSecond_LPMix = 1.f - _filterType;
            mSecond_BPMix = _filterType;
            mSecond_HPMix = 0.f;
        }
        else
        {
            mSecond_LPMix = 0.f;
            mSecond_BPMix = 2.f - _filterType;
            mSecond_HPMix = _filterType - 1.f;
        }
    }
    else
    {
        mSecond_LPMix = 1.f;
        mSecond_BPMix = 0.f;
        mSecond_HPMix = 0.f;

        _filterType = 2.f * _filterType - 2.f;

        if (_filterType < 1.f)
        {
            mFirst_LPMix = 1.f - _filterType;
            mFirst_BPMix = _filterType;
            mFirst_HPMix = 0.f;
        }
        else
        {
            mFirst_LPMix = 0.f;
            mFirst_BPMix = 2.f - _filterType;
            mFirst_HPMix = _filterType - 1.f;
        }
    }


    //****************************** Smoothing *******************************//
    mSmootherMask = 0x0000;

    mABMix_ramp = 1.f;
    mCombMix_ramp = 1.f;
    mFreqModMix_ramp = 1.f;
    mCutPitch_ramp = 1.f;
    mFirst_Filtertype_ramp = 1.f;
    mSecond_Filtertype_ramp = 1.f;
    mFade_ramp = 1.f;
}



/******************************************************************************/
/** @brief    main function which applies the State Variable filter on the
 *            incoming samples from the Soundgenerators and the Comb Filter
*******************************************************************************/

void StateVariableFilter::applyStateVariableFilter(float _sampleA, float _sampleB, float _sampleComb)
{
    //****************************** Smoothing ******************************//
    if (mSmootherMask != 0x0000)
    {
        applySmoothers();
    }


    //***************************** Sample Mix ***************************//
    float firstSample = _sampleA * mABMix_0 + _sampleB * mABMix_1;                // Mix of incoming samples
    firstSample = firstSample * mCombMix_0 + _sampleComb * mCombMix_1 ;

    float secondSample = firstSample * mFirst_Prefade;
    secondSample += (mFirst_SVSampleState * mSecond_Prefade);
    secondSample += (mSecond_ParabSatStateVar * 0.1f);                             /// for reasons this is not 0.05f!!!

    firstSample = firstSample + mFirst_ParabSatStateVar * 0.1f;                    /// for reasons this is not 0.05f!!!


    //************************** Frequency Modulation ***********************//
    float firstOmega = _sampleA * mFreqModMix_0 + _sampleB * mFreqModMix_1;            // Modulation mix
    float secondOmega = firstOmega * mSecond_FreqModConst;

    firstOmega = firstOmega * mFirst_FreqModConst;


    //************************* 1st Stage SV FILTER *************************//
    firstOmega = (mFirst_CutFreq + firstOmega) * WARPCONST_2PI;         // Warp F

    if (firstOmega > 1.9f)                                              // Clip Max
    {
        firstOmega = 1.9f;
    }

#if 0
    float filterSample = (mFirst_FirStateVar + firstSample) * 0.25f;    // FIR
    mFirst_FirStateVar = firstSample + DNC_CONST;

    filterSample = filterSample - (mFirst_Attenuation * mFirst_Int1StateVar + mFirst_Int2StateVar);

    mFirst_Int1StateVar = filterSample * firstOmega + mFirst_Int1StateVar;         // Int1 Out
    filterSample = mFirst_Int1StateVar * firstOmega + mFirst_Int2StateVar;         // Int2 Out

    float lowpassOut = filterSample + mFirst_Int2StateVar;
    float bandpassOut = mFirst_Int1StateVar + mFirst_Int1StateVar;
    float highpassOut = firstSample - (mFirst_Int1StateVar * mFirst_Attenuation + lowpassOut);

    mFirst_Int1StateVar += DNC_CONST;
    mFirst_Int2StateVar = filterSample + DNC_CONST;

#else
    float firOut = (mFirst_FirStateVar + firstSample) * 0.25f;              // FIR
    mFirst_FirStateVar = firstSample + DNC_CONST;

    mFirst_SVSampleState = firOut - (mFirst_Attenuation * mFirst_Int1StateVar + mFirst_Int2StateVar);

    float int1Out = mFirst_SVSampleState * firstOmega + mFirst_Int1StateVar;
    float int2Out = int1Out * firstOmega + mFirst_Int2StateVar;

    float lowpassOut  = int2Out + mFirst_Int2StateVar;
    float bandpassOut = int1Out + int1Out;
    float highpassOut = firstSample - (int1Out * mFirst_Attenuation + lowpassOut);

    mFirst_Int1StateVar = int1Out + DNC_CONST;
    mFirst_Int2StateVar = int2Out + DNC_CONST;
#endif

    mFirst_SVSampleState = lowpassOut * mFirst_LPMix;                               // Selector
    mFirst_SVSampleState += (bandpassOut * mFirst_BPMix);
    mFirst_SVSampleState += (highpassOut * mFirst_HPMix);


    //************************** 1st Stage Parabol Sat *********************//
    if (bandpassOut > 2.f)
    {
        mFirst_ParabSatStateVar = 2.f;
    }
    else if (bandpassOut < -2.f)
    {
        mFirst_ParabSatStateVar = -2.f;
    }
    else
    {
        mFirst_ParabSatStateVar = bandpassOut;
    }

    mFirst_ParabSatStateVar = mFirst_ParabSatStateVar * (1.f - fabs(mFirst_ParabSatStateVar) * 0.25f);


    //************************** 2nd Stage SV FILTER ************************//
    secondOmega = (mSecond_CutFreq + secondOmega) * WARPCONST_2PI;      // Warp F

    if (secondOmega > 1.9f)                                             // Clip Max
    {
        secondOmega = 1.9f;
    }

#if 0
    filterSample = (mSecond_FirStateVar + secondSample) * 0.25f;        // FIR
    mSecond_FirStateVar = secondSample + DNC_CONST;

    filterSample = filterSample - (mSecond_Attenuation * mSecond_Int1StateVar + mSecond_Int2StateVar);

    mSecond_Int1StateVar = filterSample * secondOmega + mSecond_Int1StateVar;   // Int1 Out
    filterSample = mSecond_Int1StateVar * secondOmega + mSecond_Int2StateVar;   // Int2 Out

    lowpassOut = filterSample + mSecond_Int2StateVar;
    bandpassOut = mSecond_Int1StateVar + mSecond_Int1StateVar;
    highpassOut = secondSample - (mSecond_Int1StateVar * mSecond_Attenuation + lowpassOut);

    mSecond_Int1StateVar += DNC_CONST;
    mSecond_Int2StateVar = filterSample + DNC_CONST;

    filterSample = lowpassOut * mSecond_LPMix;
    filterSample += (bandpassOut * mSecond_BPMix);
    filterSample += (highpassOut * mSecond_HPMix);

#else
    firOut = (mSecond_FirStateVar + secondSample) * 0.25f;              // FIR
    mSecond_FirStateVar = secondSample + DNC_CONST;

    float second_SVSample = firOut - (mSecond_Attenuation * mSecond_Int1StateVar + mSecond_Int2StateVar);

    int1Out = second_SVSample * secondOmega + mSecond_Int1StateVar;
    int2Out = int1Out * secondOmega + mSecond_Int2StateVar;

    lowpassOut  = int2Out + mSecond_Int2StateVar;
    bandpassOut = int1Out + int1Out;
    highpassOut = secondSample - (int1Out * mSecond_Attenuation + lowpassOut);

    mSecond_Int1StateVar = int1Out + DNC_CONST;
    mSecond_Int2StateVar = int2Out + DNC_CONST;

    second_SVSample = lowpassOut * mSecond_LPMix;
    second_SVSample += (bandpassOut * mSecond_BPMix);
    second_SVSample += (highpassOut * mSecond_HPMix);
#endif


    //************************** 2nd Stage Parabol Sat *********************//
    if (bandpassOut > 2.f)
    {
        mSecond_ParabSatStateVar = 2.f;                      // Clip Max 2.f
    }
    else if (bandpassOut < -2.f)
    {
        mSecond_ParabSatStateVar = -2.f;                     // Clip Min -2.f
    }
    else
    {
        mSecond_ParabSatStateVar = bandpassOut;
    }

    mSecond_ParabSatStateVar = mSecond_ParabSatStateVar * (1.f - fabs(mSecond_ParabSatStateVar) * 0.25f);


    //************************** Post Crossfade *********************//
    mSVFilterOut = mFirst_SVSampleState * mFirst_Postfade;
#if 0
    mSVFilterOut += (filterSample * mSecond_Postfade);
#else
    mSVFilterOut += (second_SVSample * mSecond_Postfade);
#endif
}



/******************************************************************************/
/** @brief    interface method which converts and scales the incoming midi
 *            values and passes these to the respective methods or parameters
 *  @param    midi control ID -> enum CtrlID (statevariablefilter.h)
 *  @param    midi control value [0 ... 127]
*******************************************************************************/

void StateVariableFilter::setStateVariableFilterParams(unsigned char _ctrlID, float _ctrlVal)
{
    switch (_ctrlID)
    {
        case CtrlId::ABSAMPLE_MIX:
            _ctrlVal = _ctrlVal / 126.f;

            if (_ctrlVal > 1.0)
            {
                _ctrlVal = 1.0;
            }
#ifdef PRINT_PARAMVALUES
            printf("SV Filter - AB Mix: %f\n", _ctrlVal);
#endif
            // Set Smoothing Ramp
            mABMix_target = _ctrlVal;
            mABMix_base = mABMix_1;
            mABMix_diff = mABMix_target - mABMix_base;
            mABMix_ramp = 0.f;

            mSmootherMask |= 0x0001;    // ID 1
            break;

        case CtrlId::COMBSAMPLE_MIX:

            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }
#ifdef PRINT_PARAMVALUES
            printf("SV Filter - Comb Mix: %f\n", _ctrlVal);
#endif
            // Set Smoothing Ramp
            mCombMix_target = _ctrlVal;
            mCombMix_base = mCombMix_1;
            mCombMix_diff = mCombMix_target - mCombMix_base;
            mCombMix_ramp = 0.f;

            mSmootherMask |= 0x0002;    // ID 2
            break;

        case CtrlId::CUTOFF_PITCH:
            _ctrlVal = (_ctrlVal / 1.05f) + 20.f;

            if (_ctrlVal > 140.f)
            {
                _ctrlVal = 140.f;
            }
#ifdef PRINT_PARAMVALUES
            printf("SV Filter - Cutoff: %f\n", _ctrlVal);
#endif
            // Set Smoothing Ramp
            mCutPitch_target = _ctrlVal;
            mCutPitch_base = mCutPitch;
            mCutPitch_diff = mCutPitch_target - mCutPitch_base;
            mCutPitch_ramp = 0.f;

            mSmootherMask |= 0x0008;    // ID 4
            break;

        case CtrlId::CUTOFF_KEYTRACKING:
            _ctrlVal = _ctrlVal / 127.f;
#ifdef PRINT_PARAMVALUES
            printf("SV Filter - Cutoff KeyTrk: %f\n", _ctrlVal);
#endif
            mCutKeyTracking = _ctrlVal;

            calcCutFrequency();
            break;

        case CtrlId::RESONANCE:
            _ctrlVal = _ctrlVal / 126.f;

            if (_ctrlVal > 1.f)
                _ctrlVal = 1.f;
#ifdef PRINT_PARAMVALUES
            printf("SV Filter - Resonance: %f\n", _ctrlVal);
#endif
            mResonance = _ctrlVal;
            calcAttenuation();
            break;

        case CtrlId::RESONANCE_KEYTRACKING:
            _ctrlVal = _ctrlVal / 63.f - 1.f;

            if (_ctrlVal > 1.f)
                _ctrlVal = 1.f;
#ifdef PRINT_PARAMVALUES
            printf("SV Filter - Resonance KeyTrk: %f\n", _ctrlVal);
#endif
            mResonanceKeyTracking = _ctrlVal;
            mResonanceKeyTracking *= 0.016667f;

            calcAttenuation();
            break;

        case CtrlId::SPREAD:
            _ctrlVal = (_ctrlVal / 1.05f) - 60.f;

            if (_ctrlVal > 60.f)
            {
                _ctrlVal = 60.f;
            }
#ifdef PRINT_PARAMVALUES
            printf("SV Filter - Spread: %f\n", _ctrlVal);
#endif
            mSpread = _ctrlVal * 0.5f;
            calcCutFrequency();
            break;

        case CtrlId::FILTER_TYPE:
            _ctrlVal = (_ctrlVal / 126.f);

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }
#ifdef PRINT_PARAMVALUES
            printf("SV Filter - Filter Type: %f\n", _ctrlVal);
#endif
            _ctrlVal *= 2.f;

            // Set Smoothing Ramps
            if (_ctrlVal < 1.f)                              // L-B-H Mix
            {
                mFirst_Filtertype = 0.f;
                mFirst_LPMix = 1.f;
                mFirst_BPMix = 0.f;
                mFirst_HPMix = 0.f;

                mSecond_Filtertype_target = 2.f * _ctrlVal;
                mSecond_Filtertype_base = mSecond_Filtertype;
                mSecond_Filtertype_diff = mSecond_Filtertype_target - mSecond_Filtertype_base;
                mSecond_Filtertype_ramp = 0.f;

                mSmootherMask |= 0x0020;    // ID 6
            }
            else
            {
                mSecond_Filtertype = 2.f;
                mSecond_LPMix = 0.f;
                mSecond_BPMix = 0.f;
                mSecond_HPMix = 1.f;

                mFirst_Filtertype_target = 2.f * _ctrlVal - 2.f;
                mFirst_Filtertype_base = mFirst_Filtertype;
                mFirst_Filtertype_diff = mFirst_Filtertype_target - mFirst_Filtertype_base;
                mFirst_Filtertype_ramp = 0.f;

                mSmootherMask |= 0x0010;    // ID 5
            }
            break;

        case CtrlId::FILTER_ARRANGEMENT:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }
#ifdef PRINT_PARAMVALUES
            printf("SV Filter - Parallel: %f\n", _ctrlVal);           
#endif
            // Set Smoothing Ramps
            mFade_ramp = 0.f;

            mFirst_Prefade_target = fabs(_ctrlVal);
            mFirst_Prefade_base = mFirst_Prefade;
            mFirst_Prefade_diff = mFirst_Prefade_target - mFirst_Prefade_base;

            mSecond_Prefade_target = 1.f - mFirst_Prefade_target;
            mSecond_Prefade_base = mSecond_Prefade;
            mSecond_Prefade_diff = mSecond_Prefade_target - mSecond_Prefade_base;

            mFirst_Postfade_target = 0.7f * mFirst_Prefade_target;
            mFirst_Postfade_base = mFirst_Postfade;
            mFirst_Postfade_diff = mFirst_Postfade_target - mFirst_Postfade_base;

            mSecond_Postfade_target = 0.7f * _ctrlVal + mSecond_Prefade_target;
            mSecond_Postfade_base = mSecond_Postfade;
            mSecond_Postfade_diff = mSecond_Postfade_target - mSecond_Postfade_base;

            mSmootherMask |= 0x0040;    // ID 7
            break;

        case FREQUENCYMOD:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }
#ifdef PRINT_PARAMVALUES
            printf("SV Filter - FM: %f\n", _ctrlVal);
#endif
            mFreqMod = _ctrlVal * 5.f;
            mFirst_FreqModConst = mFirst_CutFreq * mFreqMod;
            mSecond_FreqModConst = mSecond_CutFreq * mFreqMod;
            break;

        case FREQUENCYMOD_MIX:
            _ctrlVal = (_ctrlVal / 126.f);

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }
#ifdef PRINT_PARAMVALUES
            printf("SV Filter - Frequency Mod Mix: %f\n", _ctrlVal);
#endif
            // Set Smoothing Ramp
            mFreqModMix_target = _ctrlVal;
            mFreqModMix_base = mFreqModMix_1;
            mFreqModMix_diff = mFreqModMix_target - mFreqModMix_base;
            mFreqModMix_ramp = 0.f;

            mSmootherMask |= 0x0004;    // ID 3
            break;
    }
}




/*****************************************************************************/
/** @brief  the incoming pitch from the played note is passed on to member
 *          functions which calculate variables, influenced by the pitch itself
******************************************************************************/

void StateVariableFilter::setPitch(float _pitch)
{
    mPitch = _pitch;

    calcCutFrequency();
    calcAttenuation();
}



/******************************************************************************/
/** @brief  Cut-Frequency calculation for both filter stages, depending on
 *          previously set keytracking and the incoming pitch of the voice
*******************************************************************************/

void StateVariableFilter::calcCutFrequency()
{
    mCutPitch = mCutPitch + (mPitch * mCutKeyTracking);

    mFirst_CutFreq = NlToolbox::Conversion::pitch2freq(mCutPitch + mSpread);
    mFirst_FreqModConst = mFirst_CutFreq * mFreqMod;

    mSecond_CutFreq = NlToolbox::Conversion::pitch2freq(mCutPitch - mSpread);
    mSecond_FreqModConst = mSecond_CutFreq * mFreqMod;

    calcAttenuation();
}



/******************************************************************************/
/** @brief  Attenuation calculation for both filter stages, depending on
 *          previously set keytracking, resonance and the incoming
 *          pitch of the voice
*******************************************************************************/

void StateVariableFilter::calcAttenuation()
{
    float resonance = (mResonanceKeyTracking * mPitch) + mResonance;        // resonance

    /// Envelope Einfluss fehlt!

    if (resonance > 1.f)          // Clip Max
    {
        resonance = 0.99f;
    }
    else if (resonance < 0.f)     // Clip Min
    {
        resonance = 0.f;
    }

    if (resonance > 0.f && resonance < 0.33f)         // Ctrl. Shaper 2
    {
        resonance *= 1.47f;
    }
    else if (resonance > 0.33f && resonance < 0.66f)
    {
        resonance = 0.19f + resonance * 0.91f;
    }
    else if (resonance > 0.66f && resonance < 1.f)
    {
        resonance = 0.4996f + resonance * 0.44f;
    }

    resonance = 1.f - resonance;               // Res2Attenuation
    resonance += resonance;

    if (resonance < 0.02f)                     // Clip Min
    {
        resonance = 0.02f;
    }


    //****************************** 1st Stage ******************************//

    float omega = mFirst_CutFreq * WARPCONST_2PI;         // Warp Freq2Omega

    if (omega > 1.9f)                     // Clip Max
    {
        omega = 1.9f;
    }

    mFirst_Attenuation = ((2.f + omega) * (2.f - omega) * resonance) / (((resonance * omega) + (2.f - omega)) * 2.f);   // WarpD


    //****************************** 2nd Stage ******************************//

    omega = mSecond_CutFreq * WARPCONST_2PI;              // Warp F

    if (omega > 1.9f)                     // Clip Max
    {
        omega = 1.9f;
    }

    mSecond_Attenuation = ((2.f + omega) * (2 - omega) * resonance) / (((resonance * omega) + (2.f - omega)) * 2.f);   // WarpD
}



/******************************************************************************/
/** @brief  Smoother application function. If any ramp is smaller than 1.f
 *          smoothing is applied on the coresponding variable
*******************************************************************************/

void StateVariableFilter::applySmoothers()
{
    //****************************** ID 1: AB Mix ************************************//

    if (mABMix_ramp < 1.0f)
    {
        mABMix_ramp += SMOOTHER_INC;

        if (mABMix_ramp > 1.0f)
        {
            mABMix_1 = mABMix_target;
            mABMix_0 = 1.f - mABMix_1;
            mSmootherMask &= 0xFFFE;            // switch 1st bit to 0
        }
        else
        {
            mABMix_1 = mABMix_base + mABMix_diff * mABMix_ramp;
            mABMix_0 = 1.f - mABMix_1;
        }
    }


    //***************************** ID 2: Comb Mix *************************************//

    if (mCombMix_ramp < 1.0f)
    {
        mCombMix_ramp += SMOOTHER_INC;

        if (mCombMix_ramp > 1.0f)
        {
            mCombMix_1 = mCombMix_target;
            mCombMix_0 = 1.f - fabs(mCombMix_1);
            mSmootherMask &= 0xFFFD;            // switch 2nd bit to 0
        }
        else
        {
            mCombMix_1 = mCombMix_base + mCombMix_diff * mCombMix_ramp;
            mCombMix_0 = 1.f - fabs(mCombMix_1);
        }
    }


    //********************* 3: Frequency Modulation Mix **********************************//

    if (mFreqModMix_ramp < 1.0f)
    {
        mFreqModMix_ramp += SMOOTHER_INC;

        if (mFreqModMix_ramp > 1.0f)
        {
            mFreqModMix_1 = mFreqModMix_target;
            mSmootherMask &= 0xFFFB;            // switch 3rd bit to 0
        }
        else
        {
            mFreqModMix_1 = mFreqModMix_base + mFreqModMix_diff * mFreqModMix_ramp;
        }
        mFreqModMix_0 = 1.f - mFreqModMix_1;
    }


    //************************ ID 4: Cutoff Pitch Smoother *******************************//

    if (mCutPitch_ramp < 1.0f)
    {
        mCutPitch_ramp += SMOOTHER_INC;

        if (mCutPitch_ramp > 1.0f)
        {
            mCutPitch = mCutPitch_target;
            mSmootherMask &= 0xFFF7;            // switch 4th bit to 0
        }
        else
        {
            mCutPitch = mCutPitch_base + mCutPitch_diff * mCutPitch_ramp;
        }

        calcCutFrequency();
    }

    //************************ ID 5: First Stage Filtertype ******************************//

    if (mFirst_Filtertype_ramp < 1.0f)
    {
        mFirst_Filtertype_ramp += SMOOTHER_INC;

        if (mFirst_Filtertype_ramp > 1.0f)
        {
            mFirst_Filtertype = mFirst_Filtertype_target;
            mSmootherMask &= 0xFFEF;            // switch 5th bit to 0
        }
        else
        {
            mFirst_Filtertype = mFirst_Filtertype_base + mFirst_Filtertype_diff * mFirst_Filtertype_ramp;
        }

        if (mFirst_Filtertype < 1.f)
        {
            mFirst_LPMix = 1.f - mFirst_Filtertype;
            mFirst_BPMix = mFirst_Filtertype;
            mFirst_HPMix = 0.f;
        }
        else
        {
            mFirst_LPMix = 0.f;
            mFirst_BPMix = 2.f - mFirst_Filtertype;
            mFirst_HPMix = mFirst_Filtertype - 1.f;
        }

    }


    //************************ ID 6: Second Stage Filtertype *****************************//

    if (mSecond_Filtertype_ramp < 1.0f)
    {
        mSecond_Filtertype_ramp += SMOOTHER_INC;

        if (mSecond_Filtertype_ramp > 1.0f)
        {
            mSecond_Filtertype = mSecond_Filtertype_target;
            mSmootherMask &= 0xFFDF;            // switch 6th bit to 0
        }
        else
        {
            mSecond_Filtertype = mSecond_Filtertype_base + mSecond_Filtertype_diff * mSecond_Filtertype_ramp;
        }

        if (mSecond_Filtertype < 1.f)
        {
            mSecond_LPMix = 1.f - mSecond_Filtertype;
            mSecond_BPMix = mSecond_Filtertype;
            mSecond_HPMix = 0.f;
        }
        else
        {
            mSecond_LPMix = 0.f;
            mSecond_BPMix = 2.f - mSecond_Filtertype;
            mSecond_HPMix = mSecond_Filtertype - 1.f;
        }
    }


    //************************** ID 7: Prefades and Postfades *****************************//

    if (mFade_ramp < 1.0)
    {
        mFade_ramp   += SMOOTHER_INC;

        if (mFade_ramp > 1.0f)
        {
            mFirst_Prefade   = mFirst_Prefade_target;
            mSecond_Prefade  = mSecond_Prefade_target;
            mFirst_Postfade  = mFirst_Postfade_target;
            mSecond_Postfade = mSecond_Postfade_target;

            mSmootherMask &= 0xFFBF;            // switch 7th bit to 0
        }
        else
        {
            mFirst_Prefade   = mFirst_Prefade_base + mFirst_Prefade_diff * mFade_ramp;
            mSecond_Prefade  = mSecond_Prefade_base + mSecond_Prefade_diff * mFade_ramp;
            mFirst_Postfade  = mFirst_Postfade_base + mFirst_Postfade_diff * mFade_ramp;
            mSecond_Postfade = mSecond_Postfade_base + mSecond_Postfade_diff * mFade_ramp;
        }
    }
}


