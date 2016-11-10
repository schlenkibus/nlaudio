/******************************************************************************/
/** @file		outputmixer.h
    @date		2016-10-20
    @version	0.1
    @author		Anton Schmied[2016-10-20]
    @brief		An implementation of the Outputmixer Class
                as used in the C15 and implemented in Reaktor
*******************************************************************************/

#pragma once

#include "sharedoutputs.h"

#include "nltoolbox.h"
#include "smoother.h"
#include "onepolefilters.h"

class Outputmixer
{

public:
    Outputmixer();                  // Default Constructor

//    Outputmixer();                // Parametrized Constructor

    ~Outputmixer(){}                // Destructor

    float mSample_L, mSample_R;       // Resulting Samples for left and right channel

    void applyOutputmixer();

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
    float mKeyPanArray[NUM_VOICES];

    OnePoleFilters mLeftHighpass;
    OnePoleFilters mRightHighpass;

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

    void calcKeyPan();

    enum CtrlID: unsigned char      // enum for controm IDs novation ReMOTE61
    {
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
    };

};
