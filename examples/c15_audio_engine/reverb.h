/******************************************************************************/
/** @file		reverb.h
    @date		2017-06-27
    @version	0.1
    @author		Anton Schmied[2017-06-27]
    @brief		An implementation of the Reverb Class
                as used in the C15 and implemented in Reaktor

    @note       all smoothers and the LFO is running at full
                samplingrate!!!
                Loop Filter -> Lpf & Hpf Coeficients will be calculated
                with half the SR
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include <array>

//****************************** Buffer Arrays *******************************//
#define REVERB_BUFFERSIZE 16384
#define REVERB_BUFFERSIZE_M1 16383
#define REVERB_BUFFERSIZE_M2 16382

//************************** Fixed Delay Samples ****************************//
#define DELAYSAMPLES_1 281
#define DELAYSAMPLES_2 1122
#define DELAYSAMPLES_3 862
#define DELAYSAMPLES_4 466
#define DELAYSAMPLES_5 718
#define DELAYSAMPLES_6 1030
#define DELAYSAMPLES_7 886
#define DELAYSAMPLES_8 1216
#define DELAYSAMPLES_9 379
#define DELAYSAMPLES_10 1102
#define DELAYSAMPLES_11 928
#define DELAYSAMPLES_12 490
#define DELAYSAMPLES_13 682
#define DELAYSAMPLES_14 1018
#define DELAYSAMPLES_15 858
#define DELAYSAMPLES_16 1366

#define DELAYSAMPLES_L 2916
#define DELAYSAMPLES_R 2676

//*************************** Fixed Gain Amounts *****************************//
#define GAIN_1 0.617748f
#define GAIN_2 0.630809f
#define GAIN_3 0.64093f
#define GAIN_4 0.653011f


class Reverb
{
public:
    Reverb();                       // Default Constructor

    Reverb(float _size,             // Paramerized Constructor
           float _color,
           float _chorus,
           float _preDelayTime,
           float _mix);

    ~Reverb(){}                     // Destructor

    float mReverbOut_L, mReverbOut_R;
    float mFeedbackOut;

    void applyReverb(float _EchosSample_L, float _EchosSample_R, float _ReverbLevel);
    void setReverbParams(unsigned char _ctrlID, float _ctrlVal);

    float mFlushFade;
    void resetBuffer();

private:
    //*************************** Control Variables **************************//
    float mSize;
    float mAbAmnt;
    float mFBAmnt;

    float mBalance;
    float mBalance_half;
    float mBalance_full;

    float mFeedWetness;
    float mFeedColor;
    float mFeed;

    float mHPOmega, mLPOmega;
    float mLPCoeff_1, mLPCoeff_2;
    float mHPCoeff_1, mHPCoeff_2;
    float mLPStateVar_L, mLPStateVar_R;
    float mHPStateVar_L, mHPStateVar_R;

    float mDepthSize;
    float mDepthChorus;
    float mDepth;

    float mPreDelayTime_L, mPreDelayTime_R;

    float mDry;
    float mWet;

    float mLFOStateVar_1, mLFOStateVar_2;
    float mLFOWarpedFreq_1, mLFOWarpedFreq_2;

    //***************************** Delay Buffers ****************************//
    uint32_t mSampleBufferIndx;

    std::array<float, REVERB_BUFFERSIZE> mAsymBuffer_L;
    std::array<float, REVERB_BUFFERSIZE> mAsymBuffer_R;

    std::array<float, REVERB_BUFFERSIZE> mDelayBuffer_L1;
    std::array<float, REVERB_BUFFERSIZE> mDelayBuffer_L2;
    std::array<float, REVERB_BUFFERSIZE> mDelayBuffer_L3;
    std::array<float, REVERB_BUFFERSIZE> mDelayBuffer_L4;
    std::array<float, REVERB_BUFFERSIZE> mDelayBuffer_L5;
    std::array<float, REVERB_BUFFERSIZE> mDelayBuffer_L6;
    std::array<float, REVERB_BUFFERSIZE> mDelayBuffer_L7;
    std::array<float, REVERB_BUFFERSIZE> mDelayBuffer_L8;
    std::array<float, REVERB_BUFFERSIZE> mDelayBuffer_L9;

    std::array<float, REVERB_BUFFERSIZE> mDelayBuffer_R1;
    std::array<float, REVERB_BUFFERSIZE> mDelayBuffer_R2;
    std::array<float, REVERB_BUFFERSIZE> mDelayBuffer_R3;
    std::array<float, REVERB_BUFFERSIZE> mDelayBuffer_R4;
    std::array<float, REVERB_BUFFERSIZE> mDelayBuffer_R5;
    std::array<float, REVERB_BUFFERSIZE> mDelayBuffer_R6;
    std::array<float, REVERB_BUFFERSIZE> mDelayBuffer_R7;
    std::array<float, REVERB_BUFFERSIZE> mDelayBuffer_R8;
    std::array<float, REVERB_BUFFERSIZE> mDelayBuffer_R9;

    float mDelayStateVar_L1;
    float mDelayStateVar_L2;
    float mDelayStateVar_L3;
    float mDelayStateVar_L4;
    float mDelayStateVar_L5;
    float mDelayStateVar_L6;
    float mDelayStateVar_L7;
    float mDelayStateVar_L8;
    float mDelayStateVar_L9;

    float mDelayStateVar_R1;
    float mDelayStateVar_R2;
    float mDelayStateVar_R3;
    float mDelayStateVar_R4;
    float mDelayStateVar_R5;
    float mDelayStateVar_R6;
    float mDelayStateVar_R7;
    float mDelayStateVar_R8;
    float mDelayStateVar_R9;

    //************************** Smoothing Variables *************************//
    // Smoother Mask    ID 1: Balance
    //                  ID 2: Size
    //                  ID 3: LoopFilter
    //                  ID 4: PreDelayTime
    //                  ID 5: Depth
    //                  ID 6: Feed
    //                  ID 7: Mix
    //************************************************************************//
    inline void applySmoother();
    uint32_t mSmootherMask;

    // Mask ID: 1
    float mBalance_ramp;
    float mBalance_base;
    float mBalance_target;
    float mBalance_diff;

    // Mask ID: 2
    float mSize_ramp;
    float mSize_base;
    float mSize_target;
    float mSize_diff;

    // Mask ID: 3
    float mLpFltr_ramp;

    float mLPOmega_base;
    float mLPOmega_target;
    float mLPOmega_diff;

    float mHPOmega_base;
    float mHPOmega_target;
    float mHPOmega_diff;

    // Mask ID: 4
    float mPreDelayTime_ramp;

    float mPreDelayTime_L_base;
    float mPreDelayTime_L_target;
    float mPreDelayTime_L_diff;

    float mPreDelayTime_R_base;
    float mPreDelayTime_R_target;
    float mPreDelayTime_R_diff;

    // Mask ID: 5
    float mDepth_ramp;
    float mDepth_base;
    float mDepth_target;
    float mDepth_diff;

    // Mask ID: 6
    float mFeed_ramp;
    float mFeed_base;
    float mFeed_target;
    float mFeed_diff;

    // Mask ID: 7
    float mMix_ramp;

    float mDry_base;
    float mDry_target;
    float mDry_diff;

    float mWet_base;
    float mWet_target;
    float mWet_diff;


    //****************************** Controls IDs *****************************//
    enum CtrlID: unsigned char
    {
#ifdef REMOTE61                         // ReMote 61
        SIZE                = 0x15,
        COLOR               = 0x16,
        CHORUS              = 0x17,
        PRE_DELAY           = 0x18,
        MIX                 = 0x1C
#endif
#ifndef REMOTE61
        SIZE                = ,
        COLOR               = ,
        CHORUS              = ,
        PRE_DELAY           = ,
        MIX                 =
#endif
    };


    //**************************** Helper Functions ***************************//
    void initFeedSmoother();
    void initDepthSmoother();
};
