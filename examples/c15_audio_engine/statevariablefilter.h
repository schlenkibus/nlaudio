/******************************************************************************/
/** @file		statevariabkefilter.h
    @date		2016-02-16
    @version	0.1
    @author		Anton Schmied[2016-02-16]
    @brief		An implementation of the Stae VAriable Filter
                as used in the C15 and implemented in Reaktor

    @todo       Envelope Influence
*******************************************************************************/

#pragma once
#include "nltoolbox.h"


class StateVariableFilter
{
public:
    StateVariableFilter();                          // Default Contructor

    StateVariableFilter(float _ABMix,               // Parameterized Contructor
                        float _combMix,
                        float _cutoffPitch,
                        float _cutoffKeyTrk,
                        float _resonance,
                        float _resonanceKeyTrk,
                        float _spread,
                        float _filterType,
                        float _parallel,
                        float _freqMod,
                        float _freqModABMix);

    ~StateVariableFilter(){}                // Class Destructor

    float mSVFilterOut;                     // public processed output sample

    void applyStateVariableFilter(float _sampleA, float _sampleB, float _sampleComb);
    void setStateVariableFilterParams(unsigned char _ctrlID, float _ctrlVal);
    void setPitch(float _pitch);

private:
    float mPitch;

    //*********************** Control Variables ******************************//
    float mABMix_0;              // A Sample Mix (1 - mABMix_1);
    float mABMix_1;              // B Sample Mix

    float mCombMix_0;            // AB-Sample Mit (1 - (abs(mCombMix_1)))
    float mCombMix_1;            // Comb Sample Mix

    float mCutPitch;
    float mCutKeyTracking;

    float mResonance;
    float mResonanceKeyTracking;

    float mSpread;

    float mFirst_Prefade;
    float mSecond_Prefade;

    float mFirst_Postfade;
    float mSecond_Postfade;

    float mFreqMod;                 // Frequency Modulation Amount
    float mFreqModMix_0;            // A Sample Freq Modulation (1 - mFreqModMix_1)
    float mFreqModMix_1;            // B Sample Freq Modulation


    float mFirst_Filtertype, mSecond_Filtertype;                // Filter Type for each stage;
    float mFirst_LPMix, mSecond_LPMix;
    float mFirst_BPMix, mSecond_BPMix;
    float mFirst_HPMix, mSecond_HPMix;

    float mFirst_CutFreq, mSecond_CutFreq;                      // Resulting Cutoff Frequency (Pitch, Cutoff, EnvC and +Spread Influence)
    float mFirst_FreqModConst, mSecond_FreqModConst;            // Constant which results from (mCutFreq * mFreqModAmnt)
    float mFirst_Attenuation, mSecond_Attenuation;              // Attentuation for both filter stages
    float mFirst_FirStateVar, mSecond_FirStateVar;              // FIR state variable
    float mFirst_SVSampleState;                                 // first SV sample state
    float mFirst_Int1StateVar, mSecond_Int1StateVar;            // first state variable
    float mFirst_Int2StateVar, mSecond_Int2StateVar;            // second state variable
    float mFirst_ParabSatStateVar, mSecond_ParabSatStateVar;    // Output from the  Parabol Saturator


    //************************** Smoothing Variables *************************//
    // Smoother Mask    ID 1: AB Mix
    //                  ID 2: Comb Filter Fix
    //                  ID 3: Frequency Modulation Mix
    //                  ID 4: Cut Pitch
    //                  ID 5: First Filtertype
    //                  ID 6: Second Filtertype
    //                  ID 7: Fades
    //************************************************************************//
    void applySmoothers();
    uint32_t mSmootherMask;             // Smoother Mask

    // Mask ID: 1
    float mABMix_base;
    float mABMix_target;
    float mABMix_diff;
    float mABMix_ramp;

    // Mask ID: 2
    float mCombMix_base;
    float mCombMix_target;
    float mCombMix_diff;
    float mCombMix_ramp;

    // Mask ID: 3
    float mFreqModMix_base;
    float mFreqModMix_target;
    float mFreqModMix_diff;
    float mFreqModMix_ramp;

    // Mask ID: 4
    float mCutPitch_base;
    float mCutPitch_target;
    float mCutPitch_diff;
    float mCutPitch_ramp;

    // Mask ID: 5
    float mFirst_Filtertype_base;
    float mFirst_Filtertype_target;
    float mFirst_Filtertype_diff;
    float mFirst_Filtertype_ramp;

    // Mask ID: 6
    float mSecond_Filtertype_base;
    float mSecond_Filtertype_target;
    float mSecond_Filtertype_diff;
    float mSecond_Filtertype_ramp;

    // Mask ID: 7
    float mFade_ramp;               // one ramp to rule them all

    float mFirst_Prefade_base;
    float mFirst_Prefade_target;
    float mFirst_Prefade_diff;

    float mSecond_Prefade_base;
    float mSecond_Prefade_target;
    float mSecond_Prefade_diff;

    float mFirst_Postfade_base;
    float mFirst_Postfade_target;
    float mFirst_Postfade_diff;

    float mSecond_Postfade_base;
    float mSecond_Postfade_target;
    float mSecond_Postfade_diff;


    //*************************** Helper Functions ***************************//
    void calcCutFrequency();
    void calcAttenuation();


    //***************************** Controls IDs *****************************//
    enum CtrlId: unsigned char
    {
#ifdef REMOTE61                         // ReMote 61
        ABSAMPLE_MIX            = 0x15,
        COMBSAMPLE_MIX          = 0x16,

        CUTOFF_PITCH            = 0x17,
        CUTOFF_KEYTRACKING      = 0x20,
#if 0
        CUTOFF_ENVCAMONT,
#endif

        RESONANCE               = 0x18,
        RESONANCE_KEYTRACKING   = 0x21,
#if 0
        RESONANCE_ENVCAMOUNT,
#endif

        SPREAD                  = 0x19,

        FILTER_TYPE             = 0x1A,
        FILTER_ARRANGEMENT      = 0x24,

        FREQUENCYMOD            = 0x1B,
        FREQUENCYMOD_MIX        = 0x25
#else
        ABSAMPLE_MIX,
        COMBSAMPLE_MIX,

        CUTOFF_PITCH,
        CUTOFF_KEYTRACKING,
        CUTOFF_ENVCAMONT,

        RESONANCE_AMOUNT,
        RESONANCE_KEYTRACKING,
        RESONANCE_ENVCAMOUNT,

        SPREAD_AMOUNT,

        FILTER_TYPE,
        FILTER_ARRANGEMENT,

        FREQUENCYMOD_AMOUNT,
        FREQUENCYMOD_MIX
#endif
    };
};

