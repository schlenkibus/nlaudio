/******************************************************************************/
/** @file		cabinet.cpp
    @date		2016-07-01
    @version	0.2
    @author		Anton Schmied[2016-03-18]
    @brief		Echo Class member and method definitions
*******************************************************************************/

#include "echo.h"



/******************************************************************************/
/** Echo Default Constructor
 * @brief    initialization of the modules local variabels with default values
 *           SampleRate:            48 kHz
 *           Delay Time:            0.375 ms
 *           Stereo Amount:         0.0
 *           Feedback Amount:       0.5
 *           Crossfeedback Amount:  0.0
 *           HiCut Frequency:       4700 Hz     Lowpass Filters
 *           Mix:                   0.f
*******************************************************************************/

Echo::Echo()
    : mSampleRate(48000.f)
    , mFeedbackAmnt(0.5f)
    , mCrossFeedbackAmnt(0.f)
#ifdef SMOOTHEROBJ
    , mDrySmoother(48000.f, 0.032f)
    , mWetSmoother(48000.f, 0.032f)
    , mLocalFeedbackSmoother(48000.f, 0.032f)
    , mCrossFeedbackSmoother(48000.f, 0.032f)
#else
    , mESmootherMask(0x0000)
    , mInc(5.f / (48000.f * 0.032f))
#endif
{
    leftChannel.mDelayTime = 0.f;
    leftChannel.mChannelStateVar = 0.f;
    leftChannel.mSampleBuffer = {0.f};
    leftChannel.mSampleBufferIndx = 0;
    leftChannel.mLowpass = OnePoleFilters(48000.f, 4700.f, 0.f, OnePoleFilterType::LOWPASS);
    leftChannel.mHighpass = OnePoleFilters(48000.f, 50.f, 0.f, OnePoleFilterType::HIGHPASS);
    leftChannel.mLowpass2Hz = NlToolbox::Filters::Lowpass2Hz(48000.f);

    rightChannel = leftChannel;

    setDelayTime(0.375);
    setStereoAmount(0.f);
    calcFeedback();
    setMix(0.f);
}



/******************************************************************************/
/** Echo Parameterized Constructor
 * @brief    initialization of the modules local variabels with custom
 *           parameters
*******************************************************************************/

Echo::Echo(uint32_t _sampleRate,
           float _delayTime,
           float _stereoAmnt,
           float _feedbackAmnt,
           float _crossFeedbackAmnt,
           float _hiCut,
           float _mix)
    : mSampleRate(static_cast<float>(_sampleRate))
    , mFeedbackAmnt(_feedbackAmnt)
    , mCrossFeedbackAmnt(_crossFeedbackAmnt)
#ifdef SMOOTHEROBJ
    , mDrySmoother(_sampleRate, 0.032f)
    , mWetSmoother(_sampleRate, 0.032f)
    , mLocalFeedbackSmoother(_sampleRate, 0.032f)
    , mCrossFeedbackSmoother(_sampleRate, 0.032f)
#else
    , mESmootherMask(0x0000)
    , mInc(5.f / (48000.f * 0.032f))
#endif
{
    leftChannel.mDelayTime = 0.f;
    leftChannel.mChannelStateVar = 0.f;
    leftChannel.mSampleBuffer = {0.f};
    leftChannel.mSampleBufferIndx = 0;
    leftChannel.mLowpass = OnePoleFilters(_sampleRate, _hiCut, 0.f, OnePoleFilterType::LOWPASS);
    leftChannel.mHighpass = OnePoleFilters(_sampleRate, 50.f, 0.f, OnePoleFilterType::HIGHPASS);
    leftChannel.mLowpass2Hz = NlToolbox::Filters::Lowpass2Hz(_sampleRate);

    rightChannel = leftChannel;

    setDelayTime(_delayTime);
    setStereoAmount(_stereoAmnt);
    calcFeedback();
    setMix(_mix);
}



/*****************************************************************************/
/** @brief    sets mix amount, calculate dry and wet amounts
 *  @param    mix amount [0 .. 1]
******************************************************************************/

void Echo::setMix(float _mix)
{
#ifdef SMOOTHEROBJ
    float mix_square = _mix * _mix;
    mWet = (mix_square + mix_square) - mix_square * mix_square;
    mDry = (1.f - mix_square + 1.f - mix_square) - (1.f - mix_square) * (1.f - mix_square);      //dry = 1.f - 4.f * mix_square + mix_square * mix_square;

    mDrySmoother.initSmoother(mDry);
    mWetSmoother.initSmoother(mWet);
#else
    float mix_square = _mix * _mix;

    // 0: dry amount
    mDry_target = (1.f - mix_square + 1.f - mix_square) - (1.f - mix_square) * (1.f - mix_square);
    mDry_base = mDry;
    mDry_diff = mDry_target - mDry_base;

    mESmootherMask |= 0x0001;            // switch first bit to 1
    mDry_ramp = 0.f;

    // 1: wet amount
    mWet_target = (mix_square + mix_square) - mix_square * mix_square;
    mWet_base = mWet;
    mWet_diff = mWet_target - mWet_base;

    mESmootherMask |= 0x0002;            // switch second bit to 1
    mWet_ramp = 0.f;

#endif
}



/*****************************************************************************/
/** @brief    sets hiCut for the lowpass fiter
 *  @param    hiCut frequnecy in Hz
******************************************************************************/

void Echo::setHiCut(float _hiCut)
{
    leftChannel.mLowpass.setCutFreq(_hiCut);
    rightChannel.mLowpass.setCutFreq(_hiCut);
}



/*****************************************************************************/
/** @brief    sets Delay Time
 *  @param    delay time in ms
******************************************************************************/

void Echo::setDelayTime(float _delayTime)
{
    mDelayTime = _delayTime;;
    setChannelDelayTime();
}



/*****************************************************************************/
/** @brief    sets Stereo Amount
 *  @param    stereo amount [-1 .. 1]
******************************************************************************/

void Echo::setStereoAmount(float _stereoAmnt)
{
    mStereoAmnt = _stereoAmnt;
    setChannelDelayTime();
}



/*****************************************************************************/
/** @brief    sets Delay Time for L/R-Channels depending on Stereo Amount
******************************************************************************/

inline void Echo::setChannelDelayTime()
{
    leftChannel.mDelayTime = (1.f + mStereoAmnt * 0.0101f) * mDelayTime;
    rightChannel.mDelayTime = (1.f - mStereoAmnt * 0.0101f) * mDelayTime;
}



/*****************************************************************************/
/** @brief    sets Feedback Amount
 *  @param    feedback [0 .. 1]
******************************************************************************/

void Echo::setFeedbackAmount(float _feedbackAmnt)
{
    mFeedbackAmnt = _feedbackAmnt;
    calcFeedback();
}



/*****************************************************************************/
/** @brief    sets Cross Feedback Amount
 *  @param    cross feedback [0 .. 1]
******************************************************************************/

void Echo::setCrossFeedbackAmount(float _crossFeedbackAmnt)
{
    mCrossFeedbackAmnt = _crossFeedbackAmnt;
    calcFeedback();
}



/*****************************************************************************/
/** @brief    interface method which converts and scales the incoming midi
 *            values and passes these to the respective methods
 *  @param    midi control value [0 ... 127]
 *  @param    midi control ID -> enum CtrlID (echo.h)
******************************************************************************/

void Echo::setEchoParams(unsigned char _ctrlTag, float _ctrlVal)
{
    switch (_ctrlTag)
    {
        case CtrlId::HICUT:
            _ctrlVal = (_ctrlVal * 80.f) / 127.f + 60.f;          // Pitch Values for better testing
            printf("HiCut: %f\n", _ctrlVal);

            _ctrlVal = pow(2.f, (_ctrlVal - 69.f) / 12) * 440.f;

            setHiCut(_ctrlVal);
            break;

        case CtrlId::MIX:
            _ctrlVal = _ctrlVal / 127.f;
            printf("Mix: %f\n", _ctrlVal);

            setMix(_ctrlVal);
            break;

        case CtrlId::DELAYTIME:
            _ctrlVal = _ctrlVal / 127.f;
            _ctrlVal = _ctrlVal * _ctrlVal * 1.5f;
            printf("Time: %f\n", _ctrlVal * 1000.f);

            setDelayTime(_ctrlVal);
            break;

        case CtrlId::STEREOAMNT:
             _ctrlVal = ((_ctrlVal / 63.f) - 1.f);

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            _ctrlVal = _ctrlVal * 33.f;
            printf("Stereo: %f\n", _ctrlVal);

            setStereoAmount(_ctrlVal);
            break;

        case CtrlId::FEEDBACKAMNT:
            _ctrlVal = _ctrlVal / 127.f;
            printf("Feedback: %f\n", _ctrlVal);

            setFeedbackAmount(_ctrlVal);
            break;

        case CtrlId::CROSSFEEDBACKAMNT:
            _ctrlVal = _ctrlVal / 127.f;
            printf("Cross Feedback: %f\n", _ctrlVal);

            setCrossFeedbackAmount(_ctrlVal);
            break;
    }
}


/*****************************************************************************/
/** @brief    processes the incoming samples of both channels
 *  @param    raw left Sample, raw right Sample
******************************************************************************/

void Echo::applyEcho(float _rawLeftSample, float _rawRightSample)
{
#ifdef SMOOTHEROBJ
    // apply Smoothers
    mDry = mDrySmoother.smooth();
    mWet = mWetSmoother.smooth();
    mLocalFeedback = mLocalFeedbackSmoother.smooth();
    mCrossFeedback = mCrossFeedbackSmoother.smooth();
#else
    if (mESmootherMask)
    {
        applyEchoSmoother();
    }
#endif

    // Left Channel
    float processedLeftSample = _rawLeftSample + (leftChannel.mChannelStateVar * mLocalFeedback) + (rightChannel.mChannelStateVar * mCrossFeedback);

    float leftDelayTime = leftChannel.mLowpass2Hz.applyFilter(leftChannel.mDelayTime);

    processedLeftSample = delay(processedLeftSample, leftDelayTime, 0);							// apply delay

    processedLeftSample = leftChannel.mLowpass.applyFilter(processedLeftSample);                 // apply 1-pole lowpass

    leftChannel.mChannelStateVar = leftChannel.mHighpass.applyFilter(processedLeftSample) + DNC_CONST;         // apply 1-pole highpass

    //Right Channel
    float processedRightSample = _rawRightSample + (rightChannel.mChannelStateVar * mLocalFeedback) + (leftChannel.mChannelStateVar * mCrossFeedback);

    float rightDelayTime = rightChannel.mLowpass2Hz.applyFilter(rightChannel.mDelayTime);

    processedRightSample = delay(processedRightSample, rightDelayTime, 1);							// apply delay

    processedRightSample = rightChannel.mLowpass.applyFilter(processedRightSample);                 // apply 1-pole lowpass

    rightChannel.mChannelStateVar = rightChannel.mHighpass.applyFilter(processedRightSample) + DNC_CONST;         // apply 1-pole highpass

    // Crossfade
    mDelayOutL = NlToolbox::Crossfades::crossFade(_rawLeftSample, processedLeftSample, mDry, mWet);
    mDelayOutR = NlToolbox::Crossfades::crossFade(_rawRightSample, processedRightSample, mDry, mWet);
}



/*****************************************************************************/
/** @brief    processes the incoming sample depending on the channel
 *  @param    raw Sample
 *  @param    channel index, 0 - Left, 1 - Right
 *  @return   processed sample
******************************************************************************/

float Echo::applyEcho(float _currSample, uint32_t _chInd)
{
#ifdef SMOOTHEROBJ
    // apply Smoothers
    mDry = mDrySmoother.smooth();
    mWet = mWetSmoother.smooth();
    mLocalFeedback = mLocalFeedbackSmoother.smooth();
    mCrossFeedback = mCrossFeedbackSmoother.smooth();
#else
    if (mESmootherMask)
    {
        applyEchoSmoother();
    }
#endif

    float output = 0.f;

    if (_chInd == 0)                            //Channel L
    {
        output = _currSample + (leftChannel.mChannelStateVar * mLocalFeedback) + (rightChannel.mChannelStateVar * mCrossFeedback);

        float delayTime = leftChannel.mLowpass2Hz.applyFilter(leftChannel.mDelayTime);
#if 0
        output = delay(output, delayTime, leftChannel.mSampleBuffer, leftChannel.mSampleBufferIndx);
#endif
        output = delay(output, delayTime, _chInd);							// apply delay

        output = leftChannel.mLowpass.applyFilter(output);                 // apply 1-pole lowpass

        leftChannel.mChannelStateVar = leftChannel.mHighpass.applyFilter(output);         // apply 1-pole highpass

    }
    else if (_chInd == 1)                       //Channel R
    {
        output = _currSample + (rightChannel.mChannelStateVar * mLocalFeedback) + (leftChannel.mChannelStateVar * mCrossFeedback);

        float delayTime = rightChannel.mLowpass2Hz.applyFilter(rightChannel.mDelayTime);
#if 0
        output = delay(output, delayTime, rightChannel.mSampleBuffer, rightChannel.mSampleBufferIndx);
#endif
        output = delay(output, delayTime, _chInd);							// apply delay

        output = rightChannel.mLowpass.applyFilter(output);                 // apply 1-pole lowpass

        rightChannel.mChannelStateVar = rightChannel.mHighpass.applyFilter(output);         // apply 1-pole highpass
    }

    output = NlToolbox::Crossfades::crossFade(_currSample, output, mDry, mWet);

    return output;
}


#ifndef SMOOTHEROBJ
/*****************************************************************************/
/** @brief    applies the smoothers of the cabinet module, depending if the
 *            corresponding bit of the mask is set to 1
******************************************************************************/

inline void Echo::applyEchoSmoother()
{
    // 0: Dry Smoother
    if (mDry_ramp < 1.0)
    {
        mDry_ramp += mInc;

        if (mDry_ramp > 1.0)
        {
            mDry = mDry_target;
            mESmootherMask &= 0xFFFE;       // switch first bit to 0
        }
        else
        {
            mDry = mDry_base + mDry_diff * mDry_ramp;
        }
    }

    // 1: Wet Smoother
    if (mWet_ramp < 1.0)
    {
        mWet_ramp += mInc;

        if (mWet_ramp > 1.0)
        {
            mWet = mWet_target;
            mESmootherMask &= 0xFFFD;       // switch second bit to 0
        }
        else
        {
            mWet = mWet_base + mWet_diff * mWet_ramp;
        }
    }

    // 2: Local Feedback Smoother
    if (mLFeedback_ramp < 1.0)
    {
        mLFeedback_ramp += mInc;

        if (mLFeedback_ramp > 1.0)
        {
            mLocalFeedback = mLFeedback_target;
            mESmootherMask &= 0xFFFB;        // switch third bit to 0
        }
        else
        {
            mLocalFeedback = mLFeedback_base + mLFeedback_diff * mLFeedback_ramp;
        }
    }

    // 3: Cross Feedback Smoother
    if (mCFeedback_ramp < 1.0)
    {
        mCFeedback_ramp += mInc;

        if (mCFeedback_ramp > 1.0)
        {
            mCrossFeedback = mCFeedback_target;
            mESmootherMask &= 0xFFF7;        // switch fourth bit to 0
        }
        else
        {
            mCrossFeedback = mCFeedback_base + mCFeedback_diff * mCFeedback_ramp;
        }
    }
}
#endif


#if 0
/*****************************************************************************/
/** @brief    main delay function, writes to delay buffer, reads from delay buffer,
 *  		  interpolates 4 neighbouring values, all dependant on the channel index
 *  @param    raw sample
 *  @param 	  delay time, depending on the channel
 *  @param    pointer to the sampleBuffer
 *  @param    pointer to the sampleBuffer index
 *  @return	  processed sample
******************************************************************************/

float Echo::delay(float _inputSample, float _delayTime, std::array<float,131072> &_sampleBuffer, uint32_t &_sampleBufferIndx)
{
    //  DelayTime wird übergeebn, somit muss der 2Hz Lowpass vorher schon durchrechnen!
    float delaySamples = _delayTime * mSampleRate;

    float delaySamples_int = round(delaySamples - 0.5f);
    float delaySamples_fract = delaySamples - delaySamples_int;

    _sampleBuffer[_sampleBufferIndx] = _inputSample;                              //Write

    uint32_t ind_tm1 = delaySamples_int - 1;
    if (ind_tm1 < 0)
    {
        ind_tm1 = 0;
    }
    int32_t ind_t0  = delaySamples_int;
    int32_t ind_tp1 = delaySamples_int + 1;
    int32_t ind_tp2 = delaySamples_int + 2;

    ind_tm1 = _sampleBufferIndx - ind_tm1;
    ind_t0  = _sampleBufferIndx - ind_t0;
    ind_tp1 = _sampleBufferIndx - ind_tp1;
    ind_tp2 = _sampleBufferIndx - ind_tp2;

    ind_tm1 &= (_sampleBuffer.size() - 1);                         //Wrap with a mask sampleBuffer.size()-1
    ind_t0  &= (_sampleBuffer.size() - 1);
    ind_tp1 &= (_sampleBuffer.size() - 1);
    ind_tp2 &= (_sampleBuffer.size() - 1);

#if 0
    if (ind_tm1 < 0.f)  {ind_tm1 += _sampleBuffer.size();}         //Wrap as found in Reaktor
    if (ind_t0  < 0.f)  {ind_t0  += _sampleBuffer.size();}
    if (ind_tp1 < 0.f)  {ind_tp1 += _sampleBuffer.size();}
    if (ind_tp2 < 0.f)  {ind_tp2 += _sampleBuffer.size();}
#endif

    float outputSample = NlToolbox::Math::interpolRT(delaySamples_fract,
                                                     _sampleBuffer[ind_tm1],
                                                     _sampleBuffer[ind_t0],
                                                     _sampleBuffer[ind_tp1],
                                                     _sampleBuffer[ind_tp2]);

    ++_sampleBufferIndx;
    if (_sampleBufferIndx >= _sampleBuffer.size())
    {
        _sampleBufferIndx = 0;
    }

    return outputSample;

}
#endif

/*****************************************************************************/
/** @brief    main delay function, writes to delay buffer, reads from delay buffer,
 *  		  interpolates 4 neighbouring values, all dependant on the channel index
 *  @param    raw sample
 *  @param 	  delay time, depending on the channel
 *  @param    channel index
 *  @return	  processed sample
******************************************************************************/

float Echo::delay(float _inputSample, float _delayTime, uint32_t _chInd)
{
    std::array<float,131072> &sampleBuffer = (_chInd == 0)
            ? leftChannel.mSampleBuffer
            : rightChannel.mSampleBuffer;

    uint32_t &sampleBufferIndx = (_chInd == 0)
            ? leftChannel.mSampleBufferIndx
            : rightChannel.mSampleBufferIndx;

    float delaySamples = _delayTime * mSampleRate;

    float delaySamples_int = round(delaySamples - 0.5f);
    float delaySamples_fract = delaySamples - delaySamples_int;

    sampleBuffer[sampleBufferIndx] = _inputSample;                              //Write

    int32_t ind_tm1 = delaySamples_int - 1;
    if (ind_tm1 < 0)
    {
        ind_tm1 = 0;
    }
    int32_t ind_t0  = delaySamples_int;
    int32_t ind_tp1 = delaySamples_int + 1;
    int32_t ind_tp2 = delaySamples_int + 2;

    ind_tm1 = sampleBufferIndx - ind_tm1;
    ind_t0  = sampleBufferIndx - ind_t0;
    ind_tp1 = sampleBufferIndx - ind_tp1;
    ind_tp2 = sampleBufferIndx - ind_tp2;

    ind_tm1 &= (sampleBuffer.size() - 1);                         //Wrap with a mask sampleBuffer.size()-1
    ind_t0  &= (sampleBuffer.size() - 1);
    ind_tp1 &= (sampleBuffer.size() - 1);
    ind_tp2 &= (sampleBuffer.size() - 1);

#if 0
    if (ind_tm1 < 0.f)  {ind_tm1 += sampleBuffer.size();}         //Wrap as found in Reaktor
    if (ind_t0  < 0.f)  {ind_t0  += sampleBuffer.size();}
    if (ind_tp1 < 0.f)  {ind_tp1 += sampleBuffer.size();}
    if (ind_tp2 < 0.f)  {ind_tp2 += sampleBuffer.size();}
#endif

    float outputSample = NlToolbox::Math::interpolRT(delaySamples_fract,
                                                     sampleBuffer[ind_tm1],
                                                     sampleBuffer[ind_t0],
                                                     sampleBuffer[ind_tp1],
                                                     sampleBuffer[ind_tp2]);

    ++sampleBufferIndx;
    if (sampleBufferIndx >= sampleBuffer.size())
    {
        sampleBufferIndx = 0;
    }

    return outputSample;
}



/*****************************************************************************/
/** @brief    calculates (cross)feedback amount and initializes smoothers
******************************************************************************/

void Echo::calcFeedback()
{
#ifdef SMOOTHEROBJ
    mLocalFeedback = mFeedbackAmnt * (1.f - mCrossFeedbackAmnt);
    mCrossFeedback = mFeedbackAmnt * mCrossFeedbackAmnt;

    mLocalFeedbackSmoother.initSmoother(mLocalFeedback);
    mCrossFeedbackSmoother.initSmoother(mCrossFeedback);
#else
    // 2: Local Feedback
    mLFeedback_target = mFeedbackAmnt * (1.f - mCrossFeedbackAmnt);

    mLFeedback_base = mLocalFeedback;
    mLFeedback_diff = mLFeedback_target - mLFeedback_base;

    mESmootherMask |= 0x0004;                // set third bit to 1
    mLFeedback_ramp = 0.0;

    // 3: Cross Feedback
    mCFeedback_target = mFeedbackAmnt * mCrossFeedbackAmnt;

    mCFeedback_base = mCrossFeedback;
    mCFeedback_diff = mCFeedback_target - mCFeedback_base;

    mESmootherMask |= 0x0008;                // set fourth bit to 1
    mCFeedback_ramp = 0.0;

#endif
}

