/******************************************************************************/
/** @file		feedbackmixer.h
    @date		2017-06-07
    @version	0.1
    @author		Anton Schmied[2017-06-07]
    @brief		An implementation of the Feedbackmixer Class
                as used in the C15 and implemented in Reaktor

    @todo
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "onepolefilters.h"


class FeedbackMixer
{

public:
    FeedbackMixer();                        // Default Contructor

    FeedbackMixer(float _CombLevel,         // Parameterized Consgtructor
                  float _SVFilterLevel,
                  float _EffectsLevel,
                  float _ReverbLevel,
                  float _drive,
                  float _fold,
                  float _asym,
                  float _mainLevel,
                  float _keyTracking);

    ~FeedbackMixer();                       // Destructor

    float mFeedbackOut;                     // Resulting Sample
    float mReverbLevel;                     // Parameter for the Reverb

    void applyFeedbackMixer(float _CombSample, float _SVFilterSample, float _FeedbackSample);

    void setPitchInfluence(float _keyPitch);
    void setFeedbackMixerParams(unsigned char _ctrlID, float _ctrlVal);

private:
    void applySmoothers();

    float mPitch;
    float mPitchInfluence;

    float mCombLevel;
    float mSVFilterLevel;
    float mEffectsLevel;

    float mMainLevel;
    float mKeyTracking;

    float mDrive;
    float mFold;
    float mAsym;

    float mShaperStateVar;

    //***************************** Filter ****************************************//

    OnePoleFilters* pHighpass;

    //**************************** Smoothing **************************************//

    uint32_t mSmootherMask;

    // Mask ID: 1
    float mCombLevel_base;
    float mCombLevel_target;
    float mCombLevel_diff;
    float mCombLevel_ramp;

    // Mask ID: 2
    float mSVFilterLevel_base;
    float mSVFilterLevel_target;
    float mSVFilterLevel_diff;
    float mSVFilterLevel_ramp;

    // Mask ID: 3
    float mEffectsLevel_base;
    float mEffectsLevel_target;
    float mEffectsLevel_diff;
    float mEffectsLevel_ramp;

    // Mask ID: 4
    float mReverbLevel_base;
    float mReverbLevel_target;
    float mReverbLevel_diff;
    float mReverbLevel_ramp;

    // Mask ID: 5
    float mDrive_base;
    float mDrive_target;
    float mDrive_diff;
    float mDrive_ramp;

    // Mask ID: 6
    float mFold_base;
    float mFold_target;
    float mFold_diff;
    float mFold_ramp;

    // Mask ID: 7
    float mAsym_base;
    float mAsym_target;
    float mAsym_diff;
    float mAsym_ramp;

    // Mask ID: 8
    float mLevel_base;
    float mLevel_target;
    float mLevel_current;
    float mLevel_diff;
    float mLevel_ramp;


    //****************************** Controls IDs *****************************//

    enum CtrlID: unsigned char
    {
#ifdef REMOTE61                     // novation ReMOTE61
        COMB_LEVEL          = 0x15,
        SVFILTER_LEVEL      = 0x16,
        EFFECTS_LEVEL       = 0x17,
        REVERB_LEVEL        = 0x18,

        DRIVE               = 0x19,
        FOLD                = 0x1A,
        ASYM                = 0x1B,

        LEVEL               = 0x1C,
        KEY_TRACKING        = 0x26
#else
        COMB_LEVEL          ,
        SVFILTER_LEVEL      ,
        EFFECTS_LEVEL       ,
        REVERB_LEVEL        ,

        DRIVE               ,
        FOLD                ,
        ASYM                ,

        LEVEL               ,
        KEY_TRACKING
#endif
    };
};
