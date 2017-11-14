/******************************************************************************/
/** @file		flanger.h
    @date		2017-06-08
    @version	0.1
    @author		Anton Schmied[2017-06-08]
    @brief		An implementation of the Flanger Effect
                as used in the C15 and implemented in Reaktor

    @todo       parameterized constructor
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "nlglobaldefines.h"
#include "onepolefilters.h"
#include <array>

//******************************* Buffer Arrays ******************************//
#define FLANGER_BUFFERSIZE 8192
#define FLANGER_BUFFERSIZE_M1 8191

class Flanger
{
public:

    Flanger();                      // Default Constructor

    Flanger(float _rate,            // Parmetrized Constructor
            float _env,
            float _phase,
            float _tMod,
            float _time,
            float _stereo,
            float _apMod,
            float _hiCut,
            float _apTune,
            float _feedback,
            float _crossFeedback,
            float _mix);

    ~Flanger();                     // Destructor

    float mFlangerOut_L, mFlangerOut_R;

    void applyFlanger(float _rawSample_L, float _rawSample_R);
    void setFlangerParams(unsigned char _ctrlID, float _ctrlVal);
    void triggerLFO(float _velocity);

private:
    //*************************** Control Variables **************************//
    float mMixWet;
    float mMixDry;

    float mEnvWet;
    float mEnvDry;

    float mLFRate;
    float mLFDecayWarpedRate;
    float mLFDepth;
    float mLFPhase;

    float mLFStateVar;
    float mLFDecayStateVar;

    float mAPMod;
    float mAPTune;

    float mFeedback;
    float mXFeedback;
    float mLocalFeedback;
    float mCrossFeedback;

    float mStereo;
    float mTime;
    float mFlangerTime_L;
    float mFlangerTime_R;

    //***************** Channel State Variables and Filters ******************//
    float mChannelStateVar_L;
    float mChannelStateVar_R;

    uint32_t mSampleBufferIndx;
    std::array<float, FLANGER_BUFFERSIZE> mSampleBuffer_L;
    std::array<float, FLANGER_BUFFERSIZE> mSampleBuffer_R;

    OnePoleFilters* pLowpass_L;
    OnePoleFilters* pLowpass_R;

    OnePoleFilters* pHighpass_L;
    OnePoleFilters* pHighpass_R;

    NlToolbox::Filters::Lowpass2Hz* pLowpass2Hz_L;
    NlToolbox::Filters::Lowpass2Hz* pLowpass2Hz_R;

    NlToolbox::Filters::Lowpass2Hz* pLowpass2Hz_Depth;

    //**************************** Allpass 4 Pole ****************************//
    struct Allpass
    {
        //*************************** Constructor ****************************//
        Allpass()
        {
            mFirstInStateVar_1 = 0.f;
            mFirstInStateVar_2 = 0.f;
            mFirstOutStateVar_1 = 0.f;
            mFirstOutStateVar_2 = 0.f;

            mSecondInStateVar_1 = 0.f;
            mSecondInStateVar_2 = 0.f;
            mSecondOutStateVar_1 = 0.f;
            mSecondOutStateVar_2 = 0.f;
        }

        //************************ Filter Variables **************************//
        float mCoeff_1, mCoeff_2;
        float mFirstInStateVar_1, mFirstInStateVar_2, mFirstOutStateVar_1, mFirstOutStateVar_2;
        float mSecondInStateVar_1, mSecondInStateVar_2, mSecondOutStateVar_1, mSecondOutStateVar_2;

        //******************** Coefficient Calculation ***********************//
        inline void setCoeffs(float _freq)
        {
            if (_freq > FREQCLIP_22000HZ)
            {
                _freq = FREQCLIP_22000HZ;
            }

            if (_freq < FREQCLIP_2HZ)
            {
                _freq = FREQCLIP_2HZ;
            }

            _freq = _freq * WARPCONST_2PI;

            float omegaSin = NlToolbox::Math::sin(_freq);
            float omegaCos = NlToolbox::Math::cos(_freq);
            float normVar = 1.f / (1.f + omegaSin);

            mCoeff_1 = (omegaCos * -2.f) * normVar;
            mCoeff_2 = (1.f - omegaSin) * normVar;
        }

        //************************ Filter Application ************************//
        inline float applyAllpass(float _sampleIn)
        {
            float sampleOut = _sampleIn * mCoeff_2;
            sampleOut = sampleOut + mFirstInStateVar_1 * mCoeff_1;
            sampleOut = sampleOut + mFirstInStateVar_2;

            sampleOut = sampleOut - mFirstOutStateVar_1 * mCoeff_1;
            sampleOut = sampleOut - mFirstOutStateVar_2 * mCoeff_2;

            mFirstInStateVar_2 = mFirstInStateVar_1;
            mFirstInStateVar_1 = _sampleIn;
            _sampleIn = sampleOut;
            mFirstOutStateVar_2 = mFirstOutStateVar_1;
            mFirstOutStateVar_1 = sampleOut + DNC_CONST;

            sampleOut = sampleOut * mCoeff_2;
            sampleOut = sampleOut + mSecondInStateVar_1 * mCoeff_1;
            sampleOut = sampleOut + mSecondInStateVar_2;

            sampleOut = sampleOut - mSecondOutStateVar_1 * mCoeff_1;
            sampleOut = sampleOut - mSecondOutStateVar_2 * mCoeff_2;

            mSecondInStateVar_2 = mSecondInStateVar_1;
            mSecondInStateVar_1 = _sampleIn;
            mSecondOutStateVar_2 = mSecondOutStateVar_1;
            mSecondOutStateVar_1 = sampleOut + DNC_CONST;

            return sampleOut;
        }
    } mAllpass_L, mAllpass_R;


    //************************** Smoothing Variables *************************//
    // Smoother Mask    ID 1: Envelope Wet
    //                  ID 2: Local Feedback
    //                  ID 3: Cross Feedback
    //                  ID 4: Mix Wet
    //                  ID 5: LF PHase
    //************************************************************************//
    void applySmoother();

    uint32_t mSmootherMask;

    // Mask ID: 1
    float mEnvWet_base;
    float mEnvWet_target;
    float mEnvWet_diff;
    float mEnvWet_ramp;

    // Mask ID: 2
    float mLFeedback_base;
    float mLFeedback_target;
    float mLFeedback_diff;
    float mLFeedback_ramp;

    // Mask ID: 3
    float mCFeedback_base;
    float mCFeedback_target;
    float mCFeedback_diff;
    float mCFeedback_ramp;

    // Mask ID: 4
    float mMixWet_base;
    float mMixWet_target;
    float mMixWet_diff;
    float mMixWet_ramp;

    // Mask ID: 5
    float mLFPhase_base;
    float mLFPhase_target;
    float mLFPhase_diff;
    float mLFPhase_ramp;


    //****************************** Controls IDs ****************************//
    enum CtrlID: unsigned char
    {
#ifdef REMOTE61                         // ReMote 61
        RATE                = 0x15,
        PHASE               = 0x20,
        TIME_MODULTAION     = 0x17,
        ENVELOPE            = 0x16,
        TIME                = 0x18,
        STEREO              = 0x22,
        AP_MODULATOIN       = 0x19,
        AP_TUNE             = 0x1A,
        HI_CUT              = 0x23,
        FEEDBACK            = 0x1B,
        CROSS_FEEDBACK      = 0x25,
        MIX                 = 0x1C
#endif
#ifndef REMOTE61
        RATE                = ,
        ENVELOPE            = ,
        PHASE               = ,
        TIME_MODULTAION     = ,
        TIME                = ,
        STEREO              = ,
        AP_MODULATOIN       = ,
        HI_CUT              = ,
        AP_TUNE             = ,
        FEEDBACK            = ,
        CROSS_FEEDBACK      = ,
        MIX                 =
#endif
    };
};
