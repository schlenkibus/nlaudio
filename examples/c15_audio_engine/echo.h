/******************************************************************************/
/** @file		echo.h
    @date		2017-05-11
    @version	1.0
    @author		Anton Schmied[2016-03-18]
    @brief		An implementation of the Echo Effect
                as used in the C15 and implemented in Reaktor
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "nlglobaldefines.h"
#include "onepolefilters.h"
#include <array>

//******************************* Buffer Arrays ******************************//
#define ECHO_BUFFERSIZE 131072
#define ECHO_BUFFERSIZE_M1 131071

class Echo
{
public:
    Echo ();                            // Default Constructor

    Echo(float _delayTime,              // Parameterized Constructor
         float _stereo,
         float _feedbackAmnt,
         float _crossFeedbackAmnt,
         float _hiCut,
         float _mix);

    ~Echo();                            // Class Destructor

    float mEchoOut_L;
    float mEchoOut_R;

    void applyEcho(float _rawSample_L, float _rawSample_R);
    void setEchoParams(unsigned char _ctrlId, float _ctrlVal);
    inline void resetBuffer();

private:
    //*************************** Control Variabels **************************//
    float mFeedbackAmnt;            // feedback amount - external
    float mCrossFeedbackAmnt;       // cross feedback amount - external
    float mLocalFeedback;           // local feedback amount - internal
    float mCrossFeedback;			// cross feedback amount - internal

    float mStereoAmnt;				// stereo amount
    float mDelayTime;				// delay time
    float mDelayTime_L;             // channel delay time
    float mDelayTime_R;

    float mDry;						// dry amount, dependant on mix amount
    float mWet;						// wet amount, dependant on mix amount

    //******************** Channel Variables and Filters *********************//
    float mChannelStateVar_L;                       // channel state variable
    float mChannelStateVar_R;

    uint32_t mSampleBufferIndx;                     // sample buffer index
    std::array<float, ECHO_BUFFERSIZE> mSampleBuffer_L;      // sample buffer for writing and reading the samples
    std::array<float, ECHO_BUFFERSIZE> mSampleBuffer_R;

    OnePoleFilters* pLowpass_L;                     // lowpass filter
    OnePoleFilters* pLowpass_R;

    OnePoleFilters* pHighpass_L;                    // highpass filter at 50Hz
    OnePoleFilters* pHighpass_R;

    NlToolbox::Filters::Lowpass2Hz* pLowpass2Hz_L;  // 2Hz lowpass filter for smoothing the delay time
    NlToolbox::Filters::Lowpass2Hz* pLowpass2Hz_R;

    //************************** Smoothing Variables *************************//
    // Smoother Mask    ID 1: Dry
    //                  ID 2: Wet
    //                  ID 3: Local Feedback
    //                  ID 4: Cross Feedback
    //************************************************************************//
    inline void applyEchoSmoother();

    uint32_t mSmootherMask;

    // Mask ID: 1
    float mDry_base;
    float mDry_target;
    float mDry_diff;
    float mDry_ramp;

    // Mask ID: 2
    float mWet_base;
    float mWet_target;
    float mWet_diff;
    float mWet_ramp;

    // Mask ID: 3
    float mLFeedback_base;
    float mLFeedback_target;
    float mLFeedback_diff;
    float mLFeedback_ramp;

    // Mask ID: 4
    float mCFeedback_base;
    float mCFeedback_target;
    float mCFeedback_diff;
    float mCFeedback_ramp;

    //****************************** Controls IDs ****************************//
    enum CtrlID: unsigned char
    {
#ifdef NANOKONTROL_I                    // Korg Nano Kontrol I
        HICUT             = 0x12,
        MIX               = 0x04,
        DELAYTIME         = 0x05,
        STEREOAMNT        = 0x06,
        FEEDBACKAMNT      = 0x08,
        CROSSFEEDBACKAMNT = 0x09
#endif
#ifdef REMOTE61                         // ReMote 61
        FLUSH_TRG         = 0x01,
        DELAYTIME         = 0x29,
        STEREOAMNT        = 0x2A,
        FEEDBACKAMNT      = 0x2B,
        CROSSFEEDBACKAMNT = 0x2C,
        HICUT             = 0x2D,
        MIX               = 0x30
#endif
    };


    //**************************** Helper Functions ***************************//
    void initFeedbackSmoother();
    inline void calcChannelDelayTime();
};
