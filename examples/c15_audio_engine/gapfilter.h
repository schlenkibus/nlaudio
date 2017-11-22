/******************************************************************************/
/** @file		gapfilter.h
    @date		2017-08-30
    @version	0.1
    @author		Anton Schmied[2017-08-30]
    @brief		An implementation of the Gap Filter
                as used in the C15 and implemented in Reaktor
    @todo
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "nlglobaldefines.h"
#include "biquadfilters.h"

class GapFilter
{
public:
    GapFilter();                    // Default Constructor

    GapFilter(float _center,        // Parametized Contructor
              float _gap,
              float _balance,
              float _mix,
              float _stereo,
              float _resonance);

    ~GapFilter();                   // Destructor

    float mGapFilterOut_L, mGapFilterOut_R;     // public processed samples

    void applyGapFilter(float _rawSample_L, float _rawSample_R);
    void setGapFilterParams(unsigned char _ctrlID, float _ctrlVal);

private:
    //******************************* Controls *******************************//
    float mCenter;
    float mGap;
    float mBalance;
    float mMix;
    float mMixSign;
    float mStereo;
    float mResonance;

    //************************* Filters and Variables ************************//
    float mHighpassFreq_L;
    float mHighpassFreq_R;
    float mLowpassFreq_L;
    float mLowpassFreq_R;

    float mScaledFreqHP_L;
    float mScaledFreqHP_R;
    float mScaledFreqLP_L;
    float mScaledFreqLP_R;

    float mHpLpMix;
    float mInLpMix;
    float mHpOutMix;
    float mLpOutMix;
    float mInOutMix;

    BiquadFilters* pHighpass_L1;
    BiquadFilters* pHighpass_L2;
    BiquadFilters* pHighpass_R1;
    BiquadFilters* pHighpass_R2;

    BiquadFilters* pLowpass_L1;
    BiquadFilters* pLowpass_L2;
    BiquadFilters* pLowpass_R1;
    BiquadFilters* pLowpass_R2;


    //************************** Smoothing Variables *************************//
    // Smoother Mask    ID 1: Filter Frequency
    //                  ID 2: Resonance
    //                  ID 3: Filter Mix
    //************************************************************************//
    void applySmoother();
    uint32_t mSmootherMask;

    // Mask ID: 1
    float mFilterFreq_ramp;             // one ramp to rule them all filters

    float mLowpassFreq_L_base;
    float mLowpassFreq_L_target;
    float mLowpassFreq_L_diff;

    float mLowpassFreq_R_base;
    float mLowpassFreq_R_target;
    float mLowpassFreq_R_diff;

    float mHighpassFreq_L_base;
    float mHighpassFreq_L_target;
    float mHighpassFreq_L_diff;

    float mHighpassFreq_R_base;
    float mHighpassFreq_R_target;
    float mHighpassFreq_R_diff;

    // Mask ID: 2
    float mResonance_base;
    float mResonance_target;
    float mResonance_diff;
    float mResonance_ramp;

    // Mask ID: 3
    float mFilterMix_ramp;                // one ramp to rule them all mixes

    float mHpLpMix_base;
    float mHpLpMix_target;
    float mHpLpMix_diff;

    float mInLpMix_base;
    float mInLpMix_target;
    float mInLpMix_diff;

    float mHpOutMix_base;
    float mHpOutMix_target;
    float mHpOutMix_diff;

    float mLpOutMix_base;
    float mLpOutMix_target;
    float mLpOutMix_diff;

    float mInOutMix_base;
    float mInOutMix_target;
    float mInOutMix_diff;


    //****************************** Controls IDs ****************************//
    enum CtrlID: unsigned char
    {
#ifdef REMOTE61                         // ReMote 61
        CENTER              = 0x15,
        GAP                 = 0x16,
        BALANCE             = 0x17,
        MIX                 = 0x1C,
        STEREO              = 0x1F,
        RESONANCE           = 0x20
#endif
#ifndef REMOTE61
        CENTER              = ,
        GAP                 = ,
        BALANCE             = ,
        MIX                 = ,
        STEREO              = ,
        RESONANCE           =
#endif
    };


    //*************************** Helper Functions ***************************//
    void calcGapFreq();
    void calcFilterMix();
};

