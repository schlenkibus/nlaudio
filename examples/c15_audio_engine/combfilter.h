/******************************************************************************/
/** @file		combfilter.h
    @date		2017-05-18
    @version	1.0
    @author		Anton Schmied[2016-12-26]
    @brief		An implementation of the Comb Filter
                as used in the C15 and implemented in Reaktor

    @todo       Envelope C influence is missing
                Gate influece is missing
                Global Defines - Clipping Borders!?
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include <array>
#include "onepolefilters.h"

#define COMB_BUFFERSIZE 8192
#define COMB_BUFFERSIZE_M1 8191
#define COMB_BUFFERSIZE_M3 8189

class CombFilter                        // Combfilter Class
{
public:

    CombFilter();                       // Default Constructor

    CombFilter(float _ABMix,            // Parameterized Constructor
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
               float _phaseModMix);


    ~CombFilter();              // Class Destructor

    float mCombFilterOut;       // public processed output sample

    void applyCombFilter(float _sampleA, float _sampleB);
    void setPitch(float _pitch);
    void setCombFilterParams(unsigned char _ctrlID, float _ctrlVal);

    void calcMainFreq();
    void calcLowpassFreq();
    void calcAllpassFreq();
    void calcDecayGain();
    void calcDelayTime();

private:
    float mPitch;               // Incoming Pitch from a key
    float mMainFreq;            // Frequency after Pitch Edit

    float mABMix_0;             // A Sample Mix (1 - mABMix_1);
    float mABMix_1;             // B Sample Mix


    //**************************** Tune/ Pitch Edit ******************************//
    float mPitchEdit;           // Pitch Offset
    float mPitchKeyTrk;         // Key Tracking amount on Pitch
#if 0
    float mPitchEnvC;           // Env C amount on Pitch
#endif


    //**************************** Decay/ Feedback *******************************//
    float mDecay;               // Decay amount depending on the decay amount and key pitch
    float mDecayGain;           // Resulting decay gain aplied to the state variable
    float mDecayKeyTrk;         // Key Tracking amount on Decay
#if 0
    float mDecayGate;           // Gate amount on Decay
#endif
    float mDecayStateVar;       // Decay State Variable, which is added to every new incoming sample


    //******************************* Highpass ***********************************//
    OnePoleFilters* pHighpass;


    //******************************** Allpass ***********************************//
    float mAllpassTune;
    float mAllpassKeyTrk;
#if 0
    float mAllpassEnvC;
#endif
    float mAllpassRes;

    float mAllpassStateVar_1;        // Allpass State Variables
    float mAllpassStateVar_2;
    float mAllpassStateVar_3;
    float mAllpassStateVar_4;

    float mAllpassCoeff_1;           // Allpass Coefficients
    float mAllpassCoeff_2;

    float mNormPhase;                // Normalized Phase for Delay Sample Calculation


    //******************************* Lowpass ************************************//
    float mLowpassHiCut;
    float mLowpassKeyTrk;
#if 0
    float mLowpassEnvC;
#endif

    float mLowpassStateVar;         // Lowpass State Variable
    float mLowpassCoeff_A1;         // Lowpass Coefficient

    float mNegPhase;                // Negative Phase for Delay Sample Calculation


    //**************************** Phase Modulation *******************************//
    float mPhaseMod;                // Phase Modulation Amount
    float mPhaseModMix_0;           // A Sample Phase Modulation Mix (1 - mABMix_1);
    float mPhaseModMix_1;           // B Sample Phase Modulation Mix


    //***************************** Delay ************************************//
    uint32_t mSampleBufferIndex;
    std::array<float, COMB_BUFFERSIZE> mSampleBuffer;

    float mDelayClipMin;
    float mDelaySamples;
    float mDelayStateVar;


    //************************* Envelope *************************************//
//    float mEnv;                     // current vallue of the Envelope C


    //************************** Smoothing Variables *************************//
    // Smoother Mask    ID 1: AB MIx
    //                  ID 2: Phase MOdulation Mix
    //                  ID 3: Pitch Key Tracking
    //************************************************************************//
    void applySmoothers();
    uint32_t mSmootherMask;         // Smoother Mask

    // Mask ID: 1
    float mABMix_base;
    float mABMix_target;
    float mABMix_diff;
    float mABMix_ramp;

    // Mask ID: 2
    float mPhaseModMix_base;
    float mPhaseModMix_target;
    float mPhaseModMix_diff;
    float mPhaseModMix_ramp;

    // Mask ID: 3
    float mPitchKeyTrk_base;
    float mPitchKeyTrk_target;
    float mPitchKeyTrk_diff;
    float mPitchKeyTrk_ramp;


    //***************************** Controls IDs *****************************//
    enum CtrlId: unsigned char
    {
#ifdef REMOTE61                         // ReMote 61
        ABSAMPLE_MIX        = 0x15,

        PITCH_EDIT          = 0x16,
        PITCH_KEYTRACKING   = 0x20,
#if 0
        PITCH_ENVCAMOUNT    ,
#endif
        DECAY               = 0x17,
        DECAY_KEYTRACKING   = 0x21,
#if 0
        DECAY_GATE          ,
#endif
        ALLPASS_FREQ        = 0x18,
        ALLPASS_RES         = 0x19,
        ALLPASS_KEYTRACKING = 0x22,
#if 0
        ALLPASS_ENVCAMOUNT  ,
#endif
        LOWPASS_FREQ        = 0x1A,
        LOWPASS_KEYTRACKING = 0x24,
#if 0
        LOWPASS_ENVCAMOUNT  ,
#endif
        PHASEMOD_MIX        = 0x25,
        PHASEMOD            = 0x1B
#else
        ABSAMPLE_MIX,

        PITCH_EDIT,
        PITCH_KEYTRACKING,
        PITCH_ENVCAMOUNT    ,

        DECAY,
        DECAY_KEYTRACKING,
        DECAY_GATE          ,

        ALLPASS_FREQ,
        ALLPASS_RES,
        ALLPASS_KEYTRACKING,
        ALLPASS_ENVCAMOUNT  ,

        LOWPASS_FREQ,
        LOWPASS_KEYTRACKING,
        LOWPASS_ENVCAMOUNT  ,

        PHASEMOD_MIX,
        PHASEMOD
#endif
    };
};
