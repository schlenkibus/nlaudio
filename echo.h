/******************************************************************************/
/** @file		echo.h
    @date		2016-07-01
    @version	0.2
    @author		Anton Schmied[2016-03-18]
    @brief		An implementation of the Echo Effect
                as used in the C15 and implemented in Reaktor
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "smoother.h"
#include "onepolefilters.h"
#include <array>

class Echo                  // Echo Class
{
public:

    Echo ();                            // Default Constructor

    Echo(uint32_t _sampleRate,               // Parameterized Constructor
         float _delayTime,
         float _stereo,
         float _feedbackAmnt,
         float _crossFeedbackAmnt,
         float _hiCut,
         float _mix);

    ~Echo(){}                           // Class Destructor

    float mDelayOutL;
    float mDelayOutR;

    float applyEcho(float _currSample, uint32_t _chInd);
    void applyEcho(float _rawLeftSample, float _rawRightSample);
    inline void applyEchoSmoother();

    void setMix(float _mix);
    void setHiCut(float _hiCut);
    void setDelayTime(float _delayTime);
    void setStereoAmount(float _stereoAmnt);
    inline void setChannelDelayTime();

    void setFeedbackAmount(float _feedback);
    void setCrossFeedbackAmount(float _crossFeedback);

    void setEchoParams(unsigned char _ctrlId, float _ctrlVal);

private:
    float mSampleRate;              // samplerate
    float mFeedbackAmnt;            // feedback amount - external
    float mCrossFeedbackAmnt;       // cross feedback amount - external

    float mStereoAmnt;				// stereo amount
    float mDelayTime;				// delay time

    float mWet;						// wet amount
    float mDry;						// dry amount
    float mLocalFeedback;           // local feedback amount - internal
    float mCrossFeedback;			// cross feedback amount - internal

#ifdef SMOOTHEROBJ
    Smoother mDrySmoother;                      // dry amount smoother
    Smoother mWetSmoother;                      // wet smount smoother
    Smoother mLocalFeedbackSmoother;            // local feedback smoother
    Smoother mCrossFeedbackSmoother;            // cross feedback smoother
#else
    uint32_t mESmootherMask;                    // Smoother Mask (0: dry, 1: wet, 2: local feedback, 3: cross feedback)
    float mInc;                                 // Smoothing Increment

    float mDry_base;                            // Dry Smoothing values
    float mDry_target;
    float mDry_diff;
    float mDry_ramp;

    float mWet_base;                            // Wet Smoothing values
    float mWet_target;
    float mWet_diff;
    float mWet_ramp;

    float mLFeedback_base;                      // Local Feedback Smoothing values
    float mLFeedback_target;
    float mLFeedback_diff;
    float mLFeedback_ramp;

    float mCFeedback_base;                      // Cross Feedback Smoothing values
    float mCFeedback_target;
    float mCFeedback_diff;
    float mCFeedback_ramp;
#endif

    struct EchoChannel{                         // Struct for channel dependant modules and variables

        float mChannelStateVar;                     // channel state variable

        float mDelayTime;                           // channel delay time

        std::array<float,131072> mSampleBuffer;     // sample buffer for writing and reading the samples of each channel
        uint32_t mSampleBufferIndx;                 // sample buffer index counter

        OnePoleFilters mLowpass;                    // lowpass filter
        OnePoleFilters mHighpass;                   // highpass filter at 50Hz
        NlToolbox::Filters::Lowpass2Hz mLowpass2Hz;        // 2Hz lowpass filter for smoothing the delay time if changed

    }leftChannel, rightChannel;


    enum CtrlId: unsigned char          // Enum class for control IDs
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
        DELAYTIME         = 0x29,
        STEREOAMNT        = 0x2A,
        FEEDBACKAMNT      = 0x2B,
        CROSSFEEDBACKAMNT = 0x2C,
        HICUT             = 0x2D,
        MIX               = 0x30
#endif
    };

    float delay(float _inputSample, float _delayTime, uint32_t _chInd);
#if 0
    float delay(float _inputSample, float _delayTime, std::array<float,131072> &_sampleBuffer, uint32_t &_sampleBufferIndx);
#endif
    void calcFeedback();
};
