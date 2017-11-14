/******************************************************************************/
/** @file		flanger.cpp
    @date		2017-06-08
    @version	0.1
    @author		Anton Schmied[2017-06-08]
    @brief		Flanger Class member and method definitions
*******************************************************************************/

#include "flanger.h"

/******************************************************************************/
/** Flanger Default Constructor
 * @brief    initialization of the modules local variabels with default values
 *           SampleRate:            48 kHz
 *           Rate:                  1.06
 *           Envelope:              0
 *           Phase:                 90
 *           Time Mod:              0
 *           Time:                  3.13
 *           Stereo:                0
 *           AP Mod:                0
 *           AP Tune:               140
 *           HI Cut:                120
 *           Feedback:              0
 *           Cross Feedback:        0
 *           Mix:                   0
*******************************************************************************/

Flanger::Flanger()
{
    //****************************** Outputs *******************************//
    mFlangerOut_L = 0.f;
    mFlangerOut_R = 0.f;

    //******************************** Mixes *******************************//
    mMixDry = 1.f;
    mMixWet = 0.f;

    mEnvDry = 1.f;
    mEnvWet = 0.f;

    //********************************** LFO *******************************//
    mLFRate = 1.05625f * SAMPLING_INTERVAL;
    mLFDecayWarpedRate = 0.99917f;
    mLFDepth = 0.f;
    mLFPhase = 0.25f;

    mLFStateVar = 0.f;
    mLFDecayStateVar = 0.f;

    //******************************** Allpass *****************************//
    mAPMod = 0.f;
    mAPTune = 140.f;

    //******************************* FeedBack *****************************//
    mFeedback = 1.07288e-6;
    mXFeedback = 0.f;
    mLocalFeedback = mFeedback * (1.f - fabs(mXFeedback));
    mCrossFeedback = mFeedback * mXFeedback;

    //************************ Time and Stereo Amnt ************************//
    mStereo = 0.f;
    mTime = 0.003125f;
    mFlangerTime_L = mTime * (1.f + mStereo);
    mFlangerTime_R = mTime * (1.f - mStereo);

    //****************************** Buffers *******************************//
    mSampleBufferIndx = 0;
    mChannelStateVar_L = 0.f;
    mChannelStateVar_R = 0.f;

    mSampleBuffer_L = {0.f};
    mSampleBuffer_R = {0.f};

    //****************************** Filters *******************************//
    pLowpass_L = new OnePoleFilters(SAMPLERATE, NlToolbox::Conversion::pitch2freq(120.f), 0.f, OnePoleFilterType::LOWPASS);
    pLowpass_R = new OnePoleFilters(SAMPLERATE, NlToolbox::Conversion::pitch2freq(120.f), 0.f, OnePoleFilterType::LOWPASS);
    pHighpass_L = new OnePoleFilters(SAMPLERATE, 50.f, 0.f, OnePoleFilterType::HIGHPASS);
    pHighpass_R = new OnePoleFilters(SAMPLERATE, 50.f, 0.f, OnePoleFilterType::HIGHPASS);
    pLowpass2Hz_L = new NlToolbox::Filters::Lowpass2Hz(SAMPLERATE);
    pLowpass2Hz_R = new NlToolbox::Filters::Lowpass2Hz(SAMPLERATE);
    pLowpass2Hz_Depth = new NlToolbox::Filters::Lowpass2Hz(SAMPLERATE);

    mAllpass_L.setCoeffs(NlToolbox::Conversion::pitch2freq(140.f));
    mAllpass_R.setCoeffs(NlToolbox::Conversion::pitch2freq(140.f));

    //***************************** Smoothing ******************************//
    mSmootherMask = 0x0000;
    mMixWet_ramp = 1.f;
    mEnvWet_ramp = 1.f;
    mLFeedback_ramp = 1.f;
    mCFeedback_ramp = 1.f;
    mLFPhase_ramp = 1.f;
}



/******************************************************************************/
/** Flanger Parameterized Constructor
 * @brief    initialization of the modules local variabels with custom
 *           parameters
*******************************************************************************/

Flanger::Flanger(float _rate,
                 float _env,
                 float _phase,
                 float _tMod,
                 float _time,
                 float _stereo,
                 float _apMod,
                 float _hiCut,
                 float _apTune,
                 float _feedback,
                 float _crossFeedback,
                 float _mix)
{
    //****************************** Outputs *******************************//
    mFlangerOut_L = 0.f;
    mFlangerOut_R = 0.f;

    //******************************** Mixes *******************************//
    mMixWet = _mix;
    mMixDry = 1.f - fabs(mMixWet);

    mEnvWet = _env;
    mEnvDry = 1.f - _env;

    //********************************** LFO *******************************//
    _rate = _rate * _rate * 10.f;
    mLFRate = _rate * SAMPLING_INTERVAL;
    mLFDecayWarpedRate = _rate * 0.55f;
    mLFDecayWarpedRate = mLFDecayWarpedRate * WARPCONST_2PI;
    if (mLFDecayWarpedRate > 1.9f)
    {
        mLFDecayWarpedRate = 1.9f;
    }
    mLFDecayWarpedRate = 1.f - mLFDecayWarpedRate;

    mLFDepth = _tMod * fabs(_tMod);
    mLFPhase = _phase / 360.f;

    mLFStateVar = 0.f;
    mLFDecayStateVar = 0.f;

    //******************************** Allpass *****************************//
    mAPMod = _apMod * 70.f;
    mAPTune = _apTune;

    //******************************* FeedBack *****************************//
    mFeedback = _feedback * 0.5f + 0.5f;
    if (mFeedback < 0.33f)
    {
        mFeedback = 1.515151515f * mFeedback - 1.f;
    }
    else if (mFeedback > 0.33f && mFeedback < 0.66f)
    {
        mFeedback = 3.03030303f * mFeedback - 1.5f;
    }
    else if (mFeedback > 0.66f && mFeedback < 1.f)
    {
        mFeedback = 1.515151515f * mFeedback - 0.5f;
    }

    mXFeedback = _crossFeedback;
    mLocalFeedback = mFeedback * (1.f - fabs(mXFeedback));
    mCrossFeedback = mFeedback * mXFeedback;


    //************************ Time and Stereo Amnt ************************//
    mStereo = _stereo * 0.01f;
    mTime = _time * _time * 50.f * 0.001f;
    mFlangerTime_L = mTime * (1.f + mStereo);
    mFlangerTime_R = mTime * (1.f - mStereo);

    //****************************** Buffers *******************************//
    mSampleBufferIndx = 0;
    mChannelStateVar_L = 0.f;
    mChannelStateVar_R = 0.f;

    mSampleBuffer_L = {0.f};
    mSampleBuffer_R = {0.f};

    //****************************** Filters *******************************//
    pLowpass_L = new OnePoleFilters(SAMPLERATE, NlToolbox::Conversion::pitch2freq(_hiCut), 0.f, OnePoleFilterType::LOWPASS);
    pLowpass_R = new OnePoleFilters(SAMPLERATE, NlToolbox::Conversion::pitch2freq(_hiCut), 0.f, OnePoleFilterType::LOWPASS);
    pHighpass_L = new OnePoleFilters(SAMPLERATE, 50.f, 0.f, OnePoleFilterType::HIGHPASS);
    pHighpass_R = new OnePoleFilters(SAMPLERATE, 50.f, 0.f, OnePoleFilterType::HIGHPASS);
    pLowpass2Hz_L = new NlToolbox::Filters::Lowpass2Hz(SAMPLERATE);
    pLowpass2Hz_R = new NlToolbox::Filters::Lowpass2Hz(SAMPLERATE);
    pLowpass2Hz_Depth = new NlToolbox::Filters::Lowpass2Hz(SAMPLERATE);

    mAllpass_L.setCoeffs(NlToolbox::Conversion::pitch2freq(_apTune));
    mAllpass_R.setCoeffs(NlToolbox::Conversion::pitch2freq(_apTune));

    //***************************** Smoothing ******************************//
    mSmootherMask = 0x0000;
    mMixWet_ramp = 1.f;
    mEnvWet_ramp = 1.f;
    mLFeedback_ramp = 1.f;
    mCFeedback_ramp = 1.f;
    mLFPhase_ramp = 1.f;

}



/******************************************************************************/
/** Flanger Destructor
 * @brief
*******************************************************************************/

Flanger::~Flanger()
{
    delete pLowpass_L;
    delete pLowpass_R;
    delete pHighpass_L;
    delete pHighpass_R;
    delete pLowpass2Hz_L;
    delete pLowpass2Hz_R;
    delete pLowpass2Hz_Depth;
}



/*****************************************************************************/
/** @brief    processes the incoming samples of both channels
 *  @param    raw left Sample, raw right Sample
******************************************************************************/

void Flanger::applyFlanger(float _rawSample_L, float _rawSample_R)
{
    //***************************** Smoothing *******************************//
    if (mSmootherMask)
    {
        applySmoother();
    }

    //******************************** LFO **********************************//
    float decayOut = mLFDecayStateVar;
    decayOut = -1.f + decayOut * 2.f;

    float lfoPhase = mLFRate + mLFStateVar;
    lfoPhase = lfoPhase - round(lfoPhase);                              // Wrap
    mLFStateVar = lfoPhase;

    float lfoOut_L = lfoPhase * (8.f - fabs(lfoPhase) * 16.f);          // Par

    float lfoOut_R = lfoPhase + mLFPhase;
    lfoOut_R = lfoOut_R - round(lfoOut_R);                              // Wrap
    lfoOut_R = lfoOut_R * (8.f - fabs(lfoOut_R) * 16.f);                // Par

    lfoOut_L = lfoOut_L * mEnvDry + decayOut * mEnvWet;
    lfoOut_R = lfoOut_R * mEnvDry + decayOut * mEnvWet;

    mLFDecayStateVar += DNC_CONST;
    mLFDecayStateVar *= mLFDecayWarpedRate;


    //*************************** AP Freq Calc ******************************//
    mAllpass_L.setCoeffs(NlToolbox::Conversion::pitch2freq(lfoOut_L * mAPMod + mAPTune));
    mAllpass_R.setCoeffs(NlToolbox::Conversion::pitch2freq(lfoOut_R * mAPMod + mAPTune));


    //**************************** Left Channel *****************************//
    float depth = pLowpass2Hz_Depth->applyFilter(mLFDepth);              // Depth for both channels

    float processedSample = _rawSample_L + (mChannelStateVar_L * mLocalFeedback) + (mChannelStateVar_R * mCrossFeedback);
    processedSample = pLowpass_L->applyFilter(processedSample);

    float delayTime = pLowpass2Hz_L->applyFilter(mFlangerTime_L);
    delayTime = delayTime + delayTime * depth * lfoOut_L;

    mSampleBuffer_L[mSampleBufferIndx] = processedSample;

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

    ind_tm1 &= FLANGER_BUFFERSIZE_M1;                                               // Wrap with a mask sampleBuffer.size()-1
    ind_t0  &= FLANGER_BUFFERSIZE_M1;
    ind_tp1 &= FLANGER_BUFFERSIZE_M1;
    ind_tp2 &= FLANGER_BUFFERSIZE_M1;

    processedSample = NlToolbox::Math::interpolRT(delaySamples_fract,           // Interpolation
                                                  mSampleBuffer_L[ind_tm1],
                                                  mSampleBuffer_L[ind_t0],
                                                  mSampleBuffer_L[ind_tp1],
                                                  mSampleBuffer_L[ind_tp2]);

    processedSample = mAllpass_L.applyAllpass(processedSample);

    mChannelStateVar_L = pHighpass_L->applyFilter(processedSample) + DNC_CONST;

    mFlangerOut_L = NlToolbox::Crossfades::crossFade(_rawSample_L, processedSample, mMixDry, mMixWet);


    //*************************** Right Channel *****************************//
    processedSample = _rawSample_R + (mChannelStateVar_R * mLocalFeedback) + (mChannelStateVar_L * mCrossFeedback);
    processedSample = pLowpass_R->applyFilter(processedSample);

    delayTime = pLowpass2Hz_R->applyFilter(mFlangerTime_R);
    delayTime = delayTime + delayTime * depth * lfoOut_R;

    mSampleBuffer_R[mSampleBufferIndx] = processedSample;

    delaySamples = delayTime * SAMPLERATE;

    delaySamples_int = round(delaySamples - 0.5);
    delaySamples_fract = delaySamples - delaySamples_int;

    ind_tm1 = delaySamples_int - 1;
    ind_t0  = delaySamples_int;
    ind_tp1 = delaySamples_int + 1;
    ind_tp2 = delaySamples_int + 2;

    ind_tm1 = mSampleBufferIndx - ind_tm1;
    ind_t0  = mSampleBufferIndx - ind_t0;
    ind_tp1 = mSampleBufferIndx - ind_tp1;
    ind_tp2 = mSampleBufferIndx - ind_tp2;

    ind_tm1 &= FLANGER_BUFFERSIZE_M1;                                               // Wrap with a mask sampleBuffer.size()-1
    ind_t0  &= FLANGER_BUFFERSIZE_M1;
    ind_tp1 &= FLANGER_BUFFERSIZE_M1;
    ind_tp2 &= FLANGER_BUFFERSIZE_M1;

    processedSample = NlToolbox::Math::interpolRT(delaySamples_fract,           // Interpolation
                                                  mSampleBuffer_R[ind_tm1],
                                                  mSampleBuffer_R[ind_t0],
                                                  mSampleBuffer_R[ind_tp1],
                                                  mSampleBuffer_R[ind_tp2]);

    processedSample = mAllpass_R.applyAllpass(processedSample);

    mChannelStateVar_R = pHighpass_R->applyFilter(processedSample) + DNC_CONST;

    mFlangerOut_R = NlToolbox::Crossfades::crossFade(_rawSample_R, processedSample, mMixDry, mMixWet);

    mSampleBufferIndx = (mSampleBufferIndx + 1) & FLANGER_BUFFERSIZE_M1;      // increase SampleBufferindx and check index boundaries
}



/*****************************************************************************/
/** @brief    interface method which converts and scales the incoming midi
 *            values and passes these to the respective methods
 *  @param    midi control value [0 ... 127]
 *  @param    midi control ID -> enum CtrlID (flanger.h)
******************************************************************************/

void Flanger::setFlangerParams(unsigned char _ctrlID, float _ctrlVal)
{
    switch (_ctrlID)
    {
        case CtrlID::RATE:
            _ctrlVal = (_ctrlVal / 127.f);
            _ctrlVal = _ctrlVal * _ctrlVal * 10.f;

            printf("Flanger - Rate: %f\n", _ctrlVal);

            mLFRate = _ctrlVal * SAMPLING_INTERVAL;

            mLFDecayWarpedRate = _ctrlVal * 0.55f;
            mLFDecayWarpedRate = mLFDecayWarpedRate * WARPCONST_2PI;
            if (mLFDecayWarpedRate > 1.9f)
            {
                mLFDecayWarpedRate = 1.9f;
            }
            mLFDecayWarpedRate = 1.f - mLFDecayWarpedRate;
            break;

        case CtrlID::PHASE:
            _ctrlVal = (_ctrlVal / 63.f) * 90.f;

            if (_ctrlVal > 180.f)
            {
                _ctrlVal = 180.f;
            }

            printf("Flanger - Phase: %f\n", _ctrlVal);

            // Initialize Smoother
            mLFPhase_target = _ctrlVal / 360.f;
            mLFPhase_base = mLFPhase;
            mLFPhase_diff = mLFPhase_target - mLFPhase_base;

            mSmootherMask |= 0x0010;
            mLFPhase_ramp = 0.f;
            break;

        case CtrlID::TIME_MODULTAION:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            _ctrlVal = _ctrlVal * fabs(_ctrlVal);
            printf("Flanger - T Mod: %f\n", _ctrlVal);

            mLFDepth = _ctrlVal;
            break;

        case CtrlID::ENVELOPE:
            _ctrlVal = (_ctrlVal / 127.f);

            printf("Flanger - Env: %f\n", _ctrlVal);

            // Initialize Smoother
            mEnvWet_target = _ctrlVal;
            mEnvWet_base = mEnvWet;
            mEnvWet_diff = mEnvWet_target - mEnvWet_base;

            mSmootherMask |= 0x0001;
            mEnvWet_ramp = 0.f;
            break;

        case CtrlID::TIME:
            _ctrlVal = _ctrlVal / 127.f;
            _ctrlVal = _ctrlVal * _ctrlVal * 50.f;

            printf("Flanger - Time: %f\n", _ctrlVal);

            mTime = _ctrlVal * 0.001f;

            mFlangerTime_L = mTime * (1.f + mStereo);
            mFlangerTime_R = mTime * (1.f - mStereo);
            break;

        case CtrlID::STEREO:
            _ctrlVal = ((_ctrlVal / 63.f) - 1.f) * 50.f;

            if (_ctrlVal > 50.f)
            {
                _ctrlVal = 50.f;
            }

            printf("Flanger - Stereo: %f\n", _ctrlVal);

            mStereo = _ctrlVal * 0.01f;

            mFlangerTime_L = mTime * (1.f + mStereo);
            mFlangerTime_R = mTime * (1.f - mStereo);
            break;

        case CtrlID::AP_MODULATOIN:
            _ctrlVal = ((_ctrlVal / 63.f) - 1.f);

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("Flanger - AP Mod: %f\n", _ctrlVal);

            mAPMod = _ctrlVal * 70.f;
            break;

        case CtrlID::AP_TUNE:
            _ctrlVal = (_ctrlVal / 127.f) * 140.f;

            printf("Flanger - AP Tune: %f\n", _ctrlVal);

            mAPTune = _ctrlVal;
            break;

        case CtrlID::HI_CUT:
            _ctrlVal = (_ctrlVal / 127.f) * 80.f + 60.f;

            printf("Flanger - HI Cut: %f\n", _ctrlVal);

            pLowpass_L->setCutFreq(NlToolbox::Conversion::pitch2freq(_ctrlVal));
            pLowpass_R->setCutFreq(NlToolbox::Conversion::pitch2freq(_ctrlVal));
            break;

        case CtrlID::FEEDBACK:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("Flanger - Feedback: %f\n", _ctrlVal);

            _ctrlVal = _ctrlVal * 0.5f + 0.5f;

            if (_ctrlVal < 0.33f)
            {
                _ctrlVal = 1.515151515f * _ctrlVal - 1.f;
            }
            else if (_ctrlVal > 0.33f && _ctrlVal < 0.66f)
            {
                _ctrlVal = 3.03030303f * _ctrlVal - 1.5f;
            }
            else if (_ctrlVal > 0.66f && _ctrlVal < 1.f)
            {
                _ctrlVal = 1.515151515f * _ctrlVal - 0.5f;
            }

            mFeedback = _ctrlVal;

            // Initialize Smoothers
            mLFeedback_target = mFeedback * (1.f - fabs(mXFeedback));
            mLFeedback_base = mLocalFeedback;
            mLFeedback_diff = mLFeedback_target - mLFeedback_base;

            mLFeedback_ramp = 0.f;
            mSmootherMask |= 0x0002;

            mCFeedback_target = mFeedback * mXFeedback;
            mCFeedback_base = mCrossFeedback;
            mCFeedback_diff = mCFeedback_target - mCFeedback_base;

            mCFeedback_ramp = 0.f;
            mSmootherMask |= 0x0004;
            break;

        case CtrlID::CROSS_FEEDBACK:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("Flanger - Cross Feedback: %f\n", _ctrlVal);

            mXFeedback = _ctrlVal;

            // Initialize Smoothers
            mLFeedback_target = mFeedback * (1.f - fabs(mXFeedback));
            mLFeedback_base = mLocalFeedback;
            mLFeedback_diff = mLFeedback_target - mLFeedback_base;

            mLFeedback_ramp = 0.f;
            mSmootherMask |= 0x0002;

            mCFeedback_target = mFeedback * mXFeedback;
            mCFeedback_base = mCrossFeedback;
            mCFeedback_diff = mCFeedback_target - mCFeedback_base;

            mCFeedback_ramp = 0.f;
            mSmootherMask |= 0x0004;
            break;

        case CtrlID::MIX:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("Flanger - Mix: %f\n", _ctrlVal);

            // Initialize Smoother
            mMixWet_target = _ctrlVal;
            mMixWet_base = mMixWet;
            mMixWet_diff = mMixWet_target - mMixWet_base;

            mSmootherMask |= 0x0008;
            mMixWet_ramp = 0.f;
            break;
    }
}



/*****************************************************************************/
/** @brief    applies the smoothers of the cabinet module, depending if the
 *            corresponding bit of the mask is set to 1
******************************************************************************/

void Flanger::applySmoother()
{
    //********************** ID 1: Env Wet Smoother *************************//
    if (mEnvWet_ramp < 1.0)
    {
        mEnvWet_ramp += SMOOTHER_INC;

        if (mEnvWet_ramp > 1.0)
        {
            mEnvWet = mEnvWet_target;
            mSmootherMask &= 0xFFFE;       // switch first bit to 0
        }
        else
        {
            mEnvWet = mEnvWet_base + mEnvWet_diff * mEnvWet_ramp;
        }

        mEnvDry = 1.f - mEnvWet;
    }

    //****************** ID 2: Local Feedback Smoother **********************//
    if (mLFeedback_ramp < 1.0)
    {
        mLFeedback_ramp += SMOOTHER_INC;

        if (mLFeedback_ramp > 1.0)
        {
            mLocalFeedback = mLFeedback_target;
            mSmootherMask &= 0xFFFD;       // switch second bit to 0
        }
        else
        {
            mLocalFeedback = mLFeedback_base + mLFeedback_diff * mLFeedback_ramp;
        }
    }

    //****************** ID 3: Cross Feedback Smoother **********************//
    if (mCFeedback_ramp < 1.0)
    {
        mCFeedback_ramp += SMOOTHER_INC;

        if (mCFeedback_ramp > 1.0)
        {
            mCrossFeedback = mCFeedback_target;
            mSmootherMask &= 0xFFFB;       // switch third bit to 0
        }
        else
        {
            mCrossFeedback = mCFeedback_base + mCFeedback_diff * mCFeedback_ramp;
        }
    }

    //*********************** ID 4: Mix Smoother ****************************//
    if (mMixWet_ramp < 1.0)
    {
        mMixWet_ramp += SMOOTHER_INC;

        if (mMixWet_ramp > 1.0)
        {
            mMixWet = mMixWet_target;
            mSmootherMask &= 0xFFF7;       // switch fourth bit to 0
        }
        else
        {
            mMixWet = mMixWet_base + mMixWet_diff * mMixWet_ramp;
        }

        mMixDry = 1.f - fabs(mMixWet);
    }

    //************************* ID 5: LFO Phase *****************************//
    if (mLFPhase_ramp < 1.f)
    {
        mLFPhase_ramp += SMOOTHER_INC;

        if (mLFPhase_ramp > 1.f)
        {
            mLFPhase = mLFPhase_target;
            mSmootherMask &= 0xFFEF;
        }
        else
        {
            mLFPhase = mLFPhase_base + mLFPhase_diff * mLFPhase_ramp;
        }
    }
}



/*****************************************************************************/
/** @brief    triggers the LFO with the incoming velocity
******************************************************************************/

void Flanger::triggerLFO(float _velocity)
{
    mLFDecayStateVar = _velocity;
}
