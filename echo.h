/******************************************************************************/
/** @file		cabinet.h
    @date		2016-07-01
    @version	0.2
    @author		Anton Schmied[2016-03-18]
    @brief		An implementation of the Echo Effect
                as used in the C15 and implemented in Reaktor
*******************************************************************************/

#pragma once

#include "toolbox.h"
#include "smoother.h"
#include "onepolefilters.h"
#include <array>

class Echo                  // Echo Class
{
public:

    Echo ();                            // Default Constructor

    Echo(int _sampleRate,               // Parameterized Constructor
         float _delayTime,
         float _stereo,
         float _feedbackAmnt,
         float _crossFeedbackAmnt,
         float _hiCut,
         float _mix);

    ~Echo(){}                           // Class Destructor

    float applyEcho(float _currSample, unsigned int _chInd);

    void setMix(float _mix);
    void setHiCut(float _hiCut);
    void setDelayTime(float _delayTime);
    void setStereoAmount(float _stereoAmnt);
    inline void setChannelDelayTime();

    void setFeedbackAmount(float _feedback);
    void setCrossFeedbackAmount(float _crossFeedback);

    void setEchoParams(float _ctrlVal, unsigned char _ctrlTag);


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

    Smoother mDrySmoother;                      // dry amount smoother
    Smoother mWetSmoother;                      // wet smount smoother
    Smoother mLocalFeedbackSmoother;            // local feedback smoother
    Smoother mCrossFeedbackSmoother;            // cross feedback smoother

    struct EchoChannel{                         // Struct for channel dependant modules and variables

        float mChannelStateVar;                     // channel state variable

        float mDelayTime;                           // channel delay time

        std::array<float,131072> mSampleBuffer;     // sample buffer for writing and reading the samples of each channel
        unsigned int mSampleBufferIndx;             // sample buffer index counter

        OnePoleFilters mLowpass;                    // lowpass filter
        OnePoleFilters mHighpass;                   // highpass filter at 50Hz
        NlToolbox::Filters::Lowpass2Hz mLowpass2Hz;        // 2Hz lowpass filter for smoothing the delay time if changed

    }leftChannel, rightChannel;


    enum CtrlId: unsigned char                  // Enum class for control IDs (Korg Nano Kontrol I)
    {
        HICUT             = 0x12,
        MIX               = 0x04,
        DELAYTIME         = 0x05,
        STEREOAMNT        = 0x06,
        FEEDBACKAMNT      = 0x08,
        CROSSFEEDBACKAMNT = 0x09
    };

    float delay(float _inputSample, float _delayTime, unsigned int _chInd);
#if 0
    float delay(float _inputSample, float _delayTime, std::array<float,131072> &_sampleBuffer, unsigned int &_sampleBufferIndx);
#endif
    void calcFeedback();
};
