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
    , mDrySmoother(48000.f, 0.032f)
    , mWetSmoother(48000.f, 0.032f)
    , mLocalFeedbackSmoother(48000.f, 0.032f)
    , mCrossFeedbackSmoother(48000.f, 0.032f)
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

Echo::Echo(int _sampleRate,
           float _delayTime,
           float _stereoAmnt,
           float _feedbackAmnt,
           float _crossFeedbackAmnt,
           float _hiCut,
           float _mix)
    : mSampleRate(static_cast<float>(_sampleRate))
    , mFeedbackAmnt(_feedbackAmnt)
    , mCrossFeedbackAmnt(_crossFeedbackAmnt)
    , mDrySmoother(_sampleRate, 0.032f)
    , mWetSmoother(_sampleRate, 0.032f)
    , mLocalFeedbackSmoother(_sampleRate, 0.032f)
    , mCrossFeedbackSmoother(_sampleRate, 0.032f)
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
    float mix_square = _mix * _mix;
    mWet = (mix_square + mix_square) - mix_square * mix_square;
    mDry = (1.f - mix_square + 1.f - mix_square) - (1.f - mix_square) * (1.f - mix_square);      //dry = 1.f - 4.f * mix_square + mix_square * mix_square;

    mDrySmoother.initSmoother(mDry);
    mWetSmoother.initSmoother(mWet);
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

void Echo::setEchoParams(float _ctrlVal, unsigned char _ctrlTag)
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
        _ctrlVal = (_ctrlVal - 63.5f) * (33.f / 63.5f);
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
/** @brief    processes the incoming sample depending on the channel
 *  @param    raw Sample
 *  @param    channel index, 0 - Left, 1 - Right
 *  @return   processed sample
******************************************************************************/

float Echo::applyEcho(float _currSample, unsigned int _chInd)
{
    mDry = mDrySmoother.smooth();                 //apply Smoothers
    mWet = mWetSmoother.smooth();
    mLocalFeedback = mLocalFeedbackSmoother.smooth();
    mCrossFeedback = mCrossFeedbackSmoother.smooth();

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


#if 0
float Echo::delay(float _inputSample, float _delayTime, std::array<float,131072> &_sampleBuffer, unsigned int &_sampleBufferIndx)
{
    float outputSample = 0.f;

    //  DelayTime wird übergeebn, somit muss der 2Hz Lowpass vorher schon durchrechnen!
    float delaySamples = _delayTime * mSampleRate;

    float delaySamples_int = round(delaySamples - 0.5f);
    float delaySamples_fract = delaySamples - delaySamples_int;

    _sampleBuffer[_sampleBufferIndx] = _inputSample;                              //Write

    int ind_tm1 = delaySamples_int - 1;
    if (ind_tm1 < 0)
    {
        ind_tm1 = 0;
    }
    int ind_t0  = delaySamples_int;
    int ind_tp1 = delaySamples_int + 1;
    int ind_tp2 = delaySamples_int + 2;

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

    outputSample = NlToolbox::Math::interpolRT(delaySamples_fract,
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

float Echo::delay(float _inputSample, float _delayTime, unsigned int _chInd)
{
    float outputSample = 0.f;

    std::array<float,131072> &sampleBuffer = (_chInd == 0)
            ? leftChannel.mSampleBuffer
            : rightChannel.mSampleBuffer;

    unsigned int &sampleBufferIndx = (_chInd == 0)
            ? leftChannel.mSampleBufferIndx
            : rightChannel.mSampleBufferIndx;

    float delaySamples = _delayTime * mSampleRate;

    float delaySamples_int = round(delaySamples - 0.5f);
    float delaySamples_fract = delaySamples - delaySamples_int;

    sampleBuffer[sampleBufferIndx] = _inputSample;                              //Write

    int ind_tm1 = delaySamples_int - 1;
    if (ind_tm1 < 0)
    {
        ind_tm1 = 0;
    }
    int ind_t0  = delaySamples_int;
    int ind_tp1 = delaySamples_int + 1;
    int ind_tp2 = delaySamples_int + 2;

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

    outputSample = NlToolbox::Math::interpolRT(delaySamples_fract,
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
    mLocalFeedback = mFeedbackAmnt * (1.f - mCrossFeedbackAmnt);
    mCrossFeedback = mFeedbackAmnt * mCrossFeedbackAmnt;

    mLocalFeedbackSmoother.initSmoother(mLocalFeedback);
    mCrossFeedbackSmoother.initSmoother(mCrossFeedback);
}

