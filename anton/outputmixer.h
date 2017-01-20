/******************************************************************************/
/** @file		outputmixer.h
    @date		2016-10-20
    @version	0.1
    @author		Anton Schmied[2016-10-20]
    @brief		An implementation of the Outputmixer Class
                as used in the C15 and implemented in Reaktor
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "onepolefilters.h"
#ifdef SMOOTHEROBJ
#include "smoother.h"
#endif

class Outputmixer
{

public:
    Outputmixer();                  // Default Constructor

//    Outputmixer();                /// Parametrized Constructor - fehlt!

    ~Outputmixer(){}                // Destructor

    float mSample_L, mSample_R;       // Resulting Samples for left and right channel

    void applyMixer(float _sampleA, float _sampleB, float _sampleComb, float _sampleSVFilter);
    inline void applySmoothers();

    void calcKeyPan();

    void setOutputmixerParams(unsigned char _ctrlID, float _ctrlVal);

    void setALevel(float _level);
    void setAPan(float _pan);
    void setBLevel(float _level);
    void setBPan(float _pan);
    void setCombLevel(float _level);
    void setCombPan(float _pan);
    void setSVFilterLevel(float _level);
    void setSVFilterPan(float _pan);

    void setDrive(float _drive);
    void setFold(float _fold);
    void setAsym(float _asym);

    void setMainLevel(float _level);
    void setKeyPan(float _keypan);
    void setKeyPitch(uint32_t _voiceNumber, float _keyPitch);

    ///Temporär für Envelope und Velocity
    void setVelocity(uint32_t _voiceNumber, float _vel);
    void setEnvRamp(float _relTime);

private:
    float mALevel;
    float mAPan;
    float mBLevel;
    float mBPan;

    float mCombLevel;
    float mCombPan;

    float mSVFilterLevel;
    float mSVFilterPan;

    float mDrive;
    float mFold;
    float mAsym;

    float mMainLevel;
    float mKeypan;

    ///Temporäres Envelope und Velocity!!!!
    float mVelocity[NUM_VOICES];
    float mEnvRamp[NUM_VOICES];
    float mEnvInc;

    float mPitchPanArray[NUM_VOICES];
    float mKeyPitch[NUM_VOICES];

    OnePoleFilters mLeftHighpass;
    OnePoleFilters mRightHighpass;

#ifdef SMOOTHEROBJ
    Smoother mALevelSmoother;
    Smoother mAPanSmoother;
    Smoother mBLevelSmoother;
    Smoother mBPanSmoother;
    Smoother mCombLevelSmoother;
    Smoother mCombPanSmoother;
    Smoother mSVFilterLevelSmoother;
    Smoother mSVFilterPanSmoother;

    Smoother mDriveSmoother;
    Smoother mFoldSmoother;
    Smoother mAsymSmoother;

    Smoother mMainLevelSmoother;
    Smoother mKeypanSmoother;
#else
    uint32_t mOMSmootherMask;           // Smoother Mask
    float mInc;                         // Smoother Increment

    // Mask ID: 0
    float mALevel_base;
    float mALevel_target;
    float mALevel_diff;
    float mALevel_ramp;

    // Mask ID: 1
    float mAPan_base;
    float mAPan_target;
    float mAPan_diff;
    float mAPan_ramp;

    // Mask ID: 2
    float mBLevel_base;
    float mBLevel_target;
    float mBLevel_diff;
    float mBLevel_ramp;

    // Mask ID: 3
    float mBPan_base;
    float mBPan_target;
    float mBPan_diff;
    float mBPan_ramp;

    // Mask ID: 4
    float mCombLevel_base;
    float mCombLevel_target;
    float mCombLevel_diff;
    float mCombLevel_ramp;

    // Mask ID: 5
    float mCombPan_base;
    float mCombPan_target;
    float mCombPan_diff;
    float mCombPan_ramp;

    // Mask ID: 6
    float mSVFilterLevel_base;
    float mSVFilterLevel_target;
    float mSVFilterLevel_diff;
    float mSVFilterLevel_ramp;

    // Mask ID: 7
    float mSVFilterPan_base;
    float mSVFilterPan_target;
    float mSVFilterPan_diff;
    float mSVFilterPan_ramp;

    // Mask ID: 8
    float mDrive_base;
    float mDrive_target;
    float mDrive_diff;
    float mDrive_ramp;

    // Mask ID: 9
    float mFold_base;
    float mFold_target;
    float mFold_diff;
    float mFold_ramp;

    // Mask ID: 10
    float mAsym_base;
    float mAsym_target;
    float mAsym_diff;
    float mAsym_ramp;

    // Mask ID: 11
    float mMainLevel_base;
    float mMainLevel_target;
    float mMainLevel_diff;
    float mMainLevel_ramp;

    // Mask ID: 12
    float mKeypan_base;
    float mKeypan_target;
    float mKeypan_diff;
    float mKeypan_ramp;
#endif

    enum CtrlID: unsigned char      // enum for controm IDs
    {
#ifdef REMOTE61                     // novation ReMOTE61
        A_LEVEL             = 0x15,
        A_PAN               = 0x1F,
        B_LEVEL             = 0x16,
        B_PAN               = 0x20,
        COMB_LEVEL          = 0x17,
        COMB_PAN            = 0x21,
        SVFILTER_LEVEL      = 0x18,
        SVFILTER_PAN        = 0x22,

        DRIVE               = 0x19,
        FOLD                = 0x1A,
        ASYM                = 0x1B,

        MAIN_LEVEL          = 0x1C,
        KEYPAN              = 0x26,

        /// Temporär
        ENV_ATTACK          = 0x2F,
        ENV_RELEASE         = 0x30
#endif
    };
};
