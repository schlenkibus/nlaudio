/******************************************************************************/
/** @file		outputmixer.h
    @date		2016-05-30
    @version	1.0
    @author		Anton Schmied[2016-10-20]
    @brief		An implementation of the Outputmixer Class
                as used in the C15 and implemented in Reaktor

    @todo       Unison
                Check 1-Pole Filters
                the shaper state variables should be unique for each voice!!!
                NOT one for all!!!! -> fail!
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "onepolefilters.h"
#include <array>


class Outputmixer
{

public:
    Outputmixer();                  // Default Constructor

    Outputmixer(float _ALevel,      // Parametrized Constructor
                float _APan,
                float _BLevel,
                float _BPan,
                float _CombLevel,
                float _CombPan,
                float _SVFilterLevel,
                float _SVFilterPan,
                float _drive,
                float _fold,
                float _asym,
                float _mainLevel,
                float _keyPan);

    ~Outputmixer();                 // Destructor

    float mSample_L, mSample_R;     // Resulting Samples for left and right channel

    void applyOutputMixer(uint32_t _voiceNumber, float _sampleA, float _sampleB, float _sampleComb, float _sampleSVFilter);
    void setKeyPitch(uint32_t _voiceNumber, float _keyPitch);
    void setOutputmixerParams(unsigned char _ctrlID, float _ctrlVal);

private:
    //*************************** Control Variables **************************//
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

    float mShaperStateVar_R;
    float mShaperStateVar_L;

    //********************** Param Arrays for each Voice *********************//
    std::array<float, NUM_VOICES> mKeyPitch;
    std::array<float, NUM_VOICES> mAMix_R;
    std::array<float, NUM_VOICES> mAMix_L;
    std::array<float, NUM_VOICES> mBMix_R;
    std::array<float, NUM_VOICES> mBMix_L;
    std::array<float, NUM_VOICES> mCombMix_R;
    std::array<float, NUM_VOICES> mCombMix_L;
    std::array<float, NUM_VOICES> mSVFilterMix_R;
    std::array<float, NUM_VOICES> mSVFilterMix_L;

    //*************************** Filters ************************************//
    OnePoleFilters* pHighpass_L;
    OnePoleFilters* pHighpass_R;


    //************************** Smoothing Variables *************************//
    // Smoother Mask    ID 1: A Level
    //                  ID 2: A Pan
    //                  ID 3: B Level
    //                  ID 4: B Pan
    //                  ID 5: Comb Level
    //                  ID 6: Comb Pan
    //                  ID 7: SV Filter Level
    //                  ID 8: SV Filter Pan
    //                  ID 9: Drive
    //                  ID 10: Fold
    //                  ID 11: Asym
    //                  ID 12: Main Level
    //                  ID 13: Key Pan
    //************************************************************************//
    void applySmoothers();
    uint32_t mSmootherMask;

    // Mask ID: 1
    float mALevel_base;
    float mALevel_target;
    float mALevel_diff;
    float mALevel_ramp;

    // Mask ID: 2
    float mAPan_base;
    float mAPan_target;
    float mAPan_diff;
    float mAPan_ramp;

    // Mask ID: 3
    float mBLevel_base;
    float mBLevel_target;
    float mBLevel_diff;
    float mBLevel_ramp;

    // Mask ID: 4
    float mBPan_base;
    float mBPan_target;
    float mBPan_diff;
    float mBPan_ramp;

    // Mask ID: 5
    float mCombLevel_base;
    float mCombLevel_target;
    float mCombLevel_diff;
    float mCombLevel_ramp;

    // Mask ID: 6
    float mCombPan_base;
    float mCombPan_target;
    float mCombPan_diff;
    float mCombPan_ramp;

    // Mask ID: 7
    float mSVFilterLevel_base;
    float mSVFilterLevel_target;
    float mSVFilterLevel_diff;
    float mSVFilterLevel_ramp;

    // Mask ID: 8
    float mSVFilterPan_base;
    float mSVFilterPan_target;
    float mSVFilterPan_diff;
    float mSVFilterPan_ramp;

    // Mask ID: 9
    float mDrive_base;
    float mDrive_target;
    float mDrive_diff;
    float mDrive_ramp;

    // Mask ID: 10
    float mFold_base;
    float mFold_target;
    float mFold_diff;
    float mFold_ramp;

    // Mask ID: 11
    float mAsym_base;
    float mAsym_target;
    float mAsym_diff;
    float mAsym_ramp;

    // Mask ID: 12
    float mMainLevel_base;
    float mMainLevel_target;
    float mMainLevel_diff;
    float mMainLevel_ramp;

    // Mask ID: 13
    float mKeypan_base;
    float mKeypan_target;
    float mKeypan_diff;
    float mKeypan_ramp;


    //***************************** Controls IDs *****************************//
    enum CtrlID: unsigned char
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
        KEYPAN              = 0x26
#else
        A_LEVEL,
        A_PAN,
        B_LEVEL,
        B_PAN,
        COMB_LEVEL,
        COMB_PAN,
        SVFILTER_LEVEL,
        SVFILTER_PAN,

        DRIVE,
        FOLD,
        ASYM,

        MAIN_LEVEL,
        KEYPAN
#endif
    };
};
