/******************************************************************************/
/** @file		echo.cpp
    @date		2017-05-11
    @version	1.0
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
{
    mFlushFade = 1.f;

    //******************************* Outputs ********************************//
    mEchoOut_L = 0.f;
    mEchoOut_R = 0.f;

    //*************************** Feedback Amnts *****************************//
    mFeedbackAmnt = 0.5f;
    mCrossFeedbackAmnt = 0.f;
    mLocalFeedback = mFeedbackAmnt * (1.f - mCrossFeedbackAmnt);
    mCrossFeedback = mFeedbackAmnt * mCrossFeedbackAmnt;

    //***************************** Delay Times ******************************//
    mStereoAmnt = 0.f;
    mDelayTime = 0.375f;
    calcChannelDelayTime();

    //********************************* Mix **********************************//
    mDry = 1.f;
    mWet = 0.f;

    //******************************* Buffers ********************************//
    mChannelStateVar_L = 0.f;
    mChannelStateVar_R = 0.f;

    mSampleBufferIndx = 0;
    mSampleBuffer_L = {0.f};
    mSampleBuffer_R = {0.f};

    //******************************* Filters ********************************//
    pLowpass_L = new OnePoleFilters(4700.f, 0.f, OnePoleFilterType::LOWPASS);
    pLowpass_R = new OnePoleFilters(4700.f, 0.f, OnePoleFilterType::LOWPASS);
    pHighpass_L = new OnePoleFilters(50.f, 0.f, OnePoleFilterType::HIGHPASS);
    pHighpass_R = new OnePoleFilters(50.f, 0.f, OnePoleFilterType::HIGHPASS);
    pLowpass2Hz_L = new NlToolbox::Filters::Lowpass2Hz(SAMPLERATE);
    pLowpass2Hz_R = new NlToolbox::Filters::Lowpass2Hz(SAMPLERATE);

    //***************************** Smoothing ********************************//
    mSmootherMask = 0x0000;
    mWet_ramp = 1.f;
    mDry_ramp = 1.f;
    mLFeedback_ramp = 1.f;
    mCFeedback_ramp = 1.f;
}



/******************************************************************************/
/** Echo Parameterized Constructor
 * @brief    initialization of the modules local variabels with custom
 *           parameters
*******************************************************************************/

Echo::Echo(float _delayTime,
           float _stereoAmnt,
           float _feedbackAmnt,
           float _crossFeedbackAmnt,
           float _hiCut,
           float _mix)
{
    mFlushFade = 1.f;

    //******************************* Outputs ********************************//
    mEchoOut_L = 0.f;
    mEchoOut_R = 0.f;

    //*************************** Feedback Amnts *****************************//
    mFeedbackAmnt = _feedbackAmnt;
    mCrossFeedbackAmnt = _crossFeedbackAmnt;
    mLocalFeedback = mFeedbackAmnt * (1.f - mCrossFeedbackAmnt);
    mCrossFeedback = mFeedbackAmnt * mCrossFeedbackAmnt;

    //***************************** Delay Times ******************************//
    mStereoAmnt = _stereoAmnt * 33.f;
    mDelayTime = _delayTime;
    calcChannelDelayTime();

    //********************************* Mix **********************************//
    mDry = (2.f - _mix * _mix  - _mix * _mix) - (1.f - _mix * _mix) * (1.f - _mix * _mix);
    mWet = (_mix * _mix + _mix * _mix) - (_mix * _mix * _mix * _mix);

    //******************************* Buffers ********************************//
    mChannelStateVar_L = 0.f;
    mChannelStateVar_R = 0.f;

    mSampleBufferIndx = 0;
    mSampleBuffer_L = {0.f};
    mSampleBuffer_R = {0.f};

    //******************************* Filters ********************************//
    pLowpass_L = new OnePoleFilters(_hiCut, 0.f, OnePoleFilterType::LOWPASS);
    pLowpass_R = new OnePoleFilters(_hiCut, 0.f, OnePoleFilterType::LOWPASS);
    pHighpass_L = new OnePoleFilters(50.f, 0.f, OnePoleFilterType::HIGHPASS);
    pHighpass_R = new OnePoleFilters(50.f, 0.f, OnePoleFilterType::HIGHPASS);
    pLowpass2Hz_L = new NlToolbox::Filters::Lowpass2Hz(SAMPLERATE);
    pLowpass2Hz_R = new NlToolbox::Filters::Lowpass2Hz(SAMPLERATE);

    //***************************** Smoothing ********************************//
    mSmootherMask = 0x0000;
    mWet_ramp = 1.f;
    mDry_ramp = 1.f;
    mLFeedback_ramp = 1.f;
    mCFeedback_ramp = 1.f;
}



/******************************************************************************/
/** Echo Destructor
 * @brief
*******************************************************************************/

Echo::~Echo()
{
    delete pLowpass_L;
    delete pLowpass_R;
    delete pHighpass_L;
    delete pHighpass_R;
    delete pLowpass2Hz_L;
    delete pLowpass2Hz_R;
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
        case CtrlID::HICUT:
            _ctrlVal = (_ctrlVal * 80.f) / 127.f + 60.f;
#ifdef PRINT_PARAMVALUES
            printf("Echo - HiCut: %f\n", _ctrlVal);
#endif
            pLowpass_L->setCutFreq(NlToolbox::Conversion::pitch2freq(_ctrlVal));
            pLowpass_R->setCutFreq(NlToolbox::Conversion::pitch2freq(_ctrlVal));
            break;

        case CtrlID::MIX:
            _ctrlVal = _ctrlVal / 127.f;
#ifdef PRINT_PARAMVALUES
            printf("Echo - Mix: %f\n", _ctrlVal);
#endif
            _ctrlVal *= _ctrlVal;               // square of the value

            // Initialize Smoother ID 1: Dry
            mDry_target = (2.f - _ctrlVal - _ctrlVal) - (1.f - _ctrlVal) * (1.f - _ctrlVal);
            mDry_base = mDry;
            mDry_diff = mDry_target - mDry_base;

            mSmootherMask |= 0x0001;
            mDry_ramp = 0.f;

            // Initialize Smoother ID 2: Wet
            mWet_target = (_ctrlVal + _ctrlVal) - _ctrlVal * _ctrlVal;
            mWet_base = mWet;
            mWet_diff = mWet_target - mWet_base;

            mSmootherMask |= 0x0002;            // switch second bit to 1
            mWet_ramp = 0.f;
            break;

        case CtrlID::DELAYTIME:
            _ctrlVal = _ctrlVal / 127.f;
            _ctrlVal = _ctrlVal * _ctrlVal * 1.5f;
#ifdef PRINT_PARAMVALUES
            printf("Echo - Time: %f\n", _ctrlVal * 1000.f);
#endif
            mDelayTime = _ctrlVal;
            calcChannelDelayTime();
            break;

        case CtrlID::STEREOAMNT:
             _ctrlVal = ((_ctrlVal / 63.f) - 1.f);

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            _ctrlVal = _ctrlVal * 33.f;
#ifdef PRINT_PARAMVALUES
            printf("Echo - Stereo: %f\n", _ctrlVal);
#endif
            mStereoAmnt = _ctrlVal;
            calcChannelDelayTime();
            break;

        case CtrlID::FEEDBACKAMNT:
            _ctrlVal = _ctrlVal / 127.f;
#ifdef PRINT_PARAMVALUES
            printf("Echo - Feedback: %f\n", _ctrlVal);
#endif
            mFeedbackAmnt = _ctrlVal;
            initFeedbackSmoother();
            break;

        case CtrlID::CROSSFEEDBACKAMNT:
            _ctrlVal = _ctrlVal / 127.f;
#ifdef PRINT_PARAMVALUES
            printf("Echo - Cross Feedback: %f\n", _ctrlVal);
#endif
            mCrossFeedbackAmnt = _ctrlVal;
            initFeedbackSmoother();
            break;

        case CtrlID::FLUSH_TRG:

            if (_ctrlVal == 1.f)
            {
#ifdef PRINT_PARAMVALUES
                printf("Echo - Flush Buffer\n");
#endif
                resetBuffer();
            }
            break;
    }
}



/*****************************************************************************/
/** @brief    processes the incoming samples of both channels
 *  @param    raw left Sample, raw right Sample
******************************************************************************/

void Echo::applyEcho(float _rawSample_L, float _rawSample_R)
{
    //*********************************** Smoothing *************************************//
    if (mSmootherMask)
    {
        applyEchoSmoother();
    }


    //********************************** Left Channel ***********************************//
    float processedSample = _rawSample_L + (mChannelStateVar_L * mLocalFeedback) + (mChannelStateVar_R * mCrossFeedback);

    float delayTime = pLowpass2Hz_L->applyFilter(mDelayTime_L);                  // 2Hz Lowpass for delay time smoothing

    processedSample *= mFlushFade;
    mSampleBuffer_L[mSampleBufferIndx] = processedSample;                    // delay - write sample to Buffer

    float delaySamples = delayTime * SAMPLERATE;

    float delaySamples_int = round(delaySamples - 0.5f);
    float delaySamples_fract = delaySamples - delaySamples_int;

    int32_t ind_tm1 = delaySamples_int - 1;
    int32_t ind_t0  = delaySamples_int;
    int32_t ind_tp1 = delaySamples_int + 1;
    int32_t ind_tp2 = delaySamples_int + 2;

    ind_tm1 = mSampleBufferIndx - ind_tm1;
    ind_t0  = mSampleBufferIndx - ind_t0;
    ind_tp1 = mSampleBufferIndx - ind_tp1;
    ind_tp2 = mSampleBufferIndx - ind_tp2;

    ind_tm1 &= ECHO_BUFFERSIZE_M1;                                               // Wrap with a mask sampleBuffer.size()-1
    ind_t0  &= ECHO_BUFFERSIZE_M1;
    ind_tp1 &= ECHO_BUFFERSIZE_M1;
    ind_tp2 &= ECHO_BUFFERSIZE_M1;


    processedSample = NlToolbox::Math::interpolRT(delaySamples_fract,           // Interpolation
                                                  mSampleBuffer_L[ind_tm1],
                                                  mSampleBuffer_L[ind_t0],
                                                  mSampleBuffer_L[ind_tp1],
                                                  mSampleBuffer_L[ind_tp2]);

    processedSample *= mFlushFade;
    processedSample = pLowpass_L->applyFilter(processedSample);               // 1-pole lowpass

    mChannelStateVar_L = pHighpass_L->applyFilter(processedSample) + DNC_CONST;                // 1-pole highpass

    mEchoOut_L = NlToolbox::Crossfades::crossFade(_rawSample_L, processedSample, mDry, mWet);      // Crossfade


    //********************************* Right Channel ***********************************//
    processedSample = _rawSample_R + (mChannelStateVar_R * mLocalFeedback) + (mChannelStateVar_L * mCrossFeedback);

    delayTime = pLowpass2Hz_R->applyFilter(mDelayTime_R);                    // 2Hz Lowpass for delay time smoothing

    processedSample *= mFlushFade;
    mSampleBuffer_R[mSampleBufferIndx] = processedSample;                       // delay - Write sample to Buffer

    delaySamples = delayTime * SAMPLERATE;

    delaySamples_int = round(delaySamples - 0.5f);
    delaySamples_fract = delaySamples - delaySamples_int;

    ind_tm1 = delaySamples_int - 1;
    ind_t0  = delaySamples_int;
    ind_tp1 = delaySamples_int + 1;
    ind_tp2 = delaySamples_int + 2;

    ind_tm1 = mSampleBufferIndx - ind_tm1;
    ind_t0  = mSampleBufferIndx - ind_t0;
    ind_tp1 = mSampleBufferIndx - ind_tp1;
    ind_tp2 = mSampleBufferIndx - ind_tp2;

    ind_tm1 &= ECHO_BUFFERSIZE_M1;                                                   // Wrap with a mask sampleBuffer.size()-1
    ind_t0  &= ECHO_BUFFERSIZE_M1;
    ind_tp1 &= ECHO_BUFFERSIZE_M1;
    ind_tp2 &= ECHO_BUFFERSIZE_M1;

    processedSample = NlToolbox::Math::interpolRT(delaySamples_fract,               // Interpolation
                                                  mSampleBuffer_R[ind_tm1],
                                                  mSampleBuffer_R[ind_t0],
                                                  mSampleBuffer_R[ind_tp1],
                                                  mSampleBuffer_R[ind_tp2]);

    processedSample *= mFlushFade;
    processedSample = pLowpass_R->applyFilter(processedSample);                  // 1 pole lowpass

    mChannelStateVar_R = pHighpass_R->applyFilter(processedSample) + DNC_CONST;              // 1-pole highpass

    mEchoOut_R = NlToolbox::Crossfades::crossFade(_rawSample_R, processedSample, mDry, mWet);    // Crossfade

    mSampleBufferIndx = (mSampleBufferIndx + 1) & ECHO_BUFFERSIZE_M1;      // increase SampleBufferindx and check index boundaries
}



/*****************************************************************************/
/** @brief    applies the smoothers of the cabinet module, depending if the
 *            corresponding bit of the mask is set to 1
******************************************************************************/

inline void Echo::applyEchoSmoother()
{
    //************************ ID 1: Dry Smoother ***************************//
    if (mDry_ramp < 1.f)
    {
        mDry_ramp += SMOOTHER_INC;

        if (mDry_ramp > 1.f)
        {
            mDry = mDry_target;
            mSmootherMask &= 0xFFFE;       // switch first bit to 0
        }
        else
        {
            mDry = mDry_base + mDry_diff * mDry_ramp;
        }
    }


    //************************ ID 2: Wet Smoother ***************************//
    if (mWet_ramp < 1.f)
    {
        mWet_ramp += SMOOTHER_INC;

        if (mWet_ramp > 1.f)
        {
            mWet = mWet_target;
            mSmootherMask &= 0xFFFD;       // switch second bit to 0
        }
        else
        {
            mWet = mWet_base + mWet_diff * mWet_ramp;
        }
    }


    //******************* ID 3: Local Feedback Smoother *********************//
    if (mLFeedback_ramp < 1.f)
    {
        mLFeedback_ramp += SMOOTHER_INC;

        if (mLFeedback_ramp > 1.f)
        {
            mLocalFeedback = mLFeedback_target;
            mSmootherMask &= 0xFFFB;        // switch third bit to 0
        }
        else
        {
            mLocalFeedback = mLFeedback_base + mLFeedback_diff * mLFeedback_ramp;
        }
    }


    //******************* ID 4: Cross Feedback Smoother *********************//
    if (mCFeedback_ramp < 1.f)
    {
        mCFeedback_ramp += SMOOTHER_INC;

        if (mCFeedback_ramp > 1.f)
        {
            mCrossFeedback = mCFeedback_target;
            mSmootherMask &= 0xFFF7;        // switch fourth bit to 0
        }
        else
        {
            mCrossFeedback = mCFeedback_base + mCFeedback_diff * mCFeedback_ramp;
        }
    }
}



/*****************************************************************************/
/** @brief    sets both Buffers to zero, when the preset changes for example
******************************************************************************/

void Echo::resetBuffer()
{
    mSampleBuffer_L.fill(0.f);
    mSampleBuffer_R.fill(0.f);
}



/*****************************************************************************/
/** @brief    sets Delay Time for L/R-Channels depending on Stereo Amount and
 *            globaly set Delay Time
******************************************************************************/

inline void Echo::calcChannelDelayTime()
{
    mDelayTime_L = (1.f + mStereoAmnt * 0.0101f) * mDelayTime;
    mDelayTime_R = (1.f - mStereoAmnt * 0.0101f) * mDelayTime;
}



/*****************************************************************************/
/** @brief    calculates (cross)feedback amount and initializes smoothers
******************************************************************************/

void Echo::initFeedbackSmoother()
{
    // Initialize Smoother ID 3: Local Feedback
    mLFeedback_target = mFeedbackAmnt * (1.f - mCrossFeedbackAmnt);

    mLFeedback_base = mLocalFeedback;
    mLFeedback_diff = mLFeedback_target - mLFeedback_base;

    mSmootherMask |= 0x0004;
    mLFeedback_ramp = 0.f;


    // Initialize Smoother ID 4: Cross Feedback
    mCFeedback_target = mFeedbackAmnt * mCrossFeedbackAmnt;

    mCFeedback_base = mCrossFeedback;
    mCFeedback_diff = mCFeedback_target - mCFeedback_base;

    mSmootherMask |= 0x0008;
    mCFeedback_ramp = 0.f;
}

