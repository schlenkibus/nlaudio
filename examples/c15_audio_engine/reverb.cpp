/******************************************************************************/
/** @file		reverb.cpp
    @date		2017-06-27
    @version	0.1
    @author		Anton Schmied[2017-06-27]
    @brief		Reverb Class member and method definitions
*******************************************************************************/

#include "reverb.h"

Reverb::Reverb()
{
    mReverbOut_L = 0.f;
    mReverbOut_R = 0.f;

    mFeedbackOut = 0.f;

    mSampleBufferIndx = 0;

    mAsymBuffer_L = {0.f};
    mAsymBuffer_R = {0.f};

    mDelayBuffer_L1 = {0.f};
    mDelayBuffer_L2 = {0.f};
    mDelayBuffer_L3 = {0.f};
    mDelayBuffer_L4 = {0.f};
    mDelayBuffer_L5 = {0.f};
    mDelayBuffer_L6 = {0.f};
    mDelayBuffer_L7 = {0.f};
    mDelayBuffer_L8 = {0.f};
    mDelayBuffer_L9 = {0.f};

    mDelayBuffer_R1 = {0.f};
    mDelayBuffer_R2 = {0.f};
    mDelayBuffer_R3 = {0.f};
    mDelayBuffer_R4 = {0.f};
    mDelayBuffer_R5 = {0.f};
    mDelayBuffer_R6 = {0.f};
    mDelayBuffer_R7 = {0.f};
    mDelayBuffer_R8 = {0.f};
    mDelayBuffer_R9 = {0.f};

    mDelayStateVar_L1 = 0.f;
    mDelayStateVar_L2 = 0.f;
    mDelayStateVar_L3 = 0.f;
    mDelayStateVar_L4 = 0.f;
    mDelayStateVar_L5 = 0.f;
    mDelayStateVar_L6 = 0.f;
    mDelayStateVar_L7 = 0.f;
    mDelayStateVar_L8 = 0.f;
    mDelayStateVar_L9 = 0.f;

    mDelayStateVar_R1 = 0.f;
    mDelayStateVar_R2 = 0.f;
    mDelayStateVar_R3 = 0.f;
    mDelayStateVar_R4 = 0.f;
    mDelayStateVar_R5 = 0.f;
    mDelayStateVar_R6 = 0.f;
    mDelayStateVar_R7 = 0.f;
    mDelayStateVar_R8 = 0.f;
    mDelayStateVar_R9 = 0.f;

    mLFOStateVar_1 = 0.f;
    mLFOStateVar_2 = 0.f;
    mLFOWarpedFreq_1 = 0.86306 * (2 / SAMPLERATE);
    mLFOWarpedFreq_2 = 0.6666 * (2 / SAMPLERATE);

    mLPStateVar_L = 0.f;
    mLPStateVar_R = 0.f;
    mHPStateVar_L = 0.f;
    mHPStateVar_R = 0.f;

//    float mLPCoeff_1, mLPCoeff_2;
//    float mHPCoeff_1, mHPCoeff_2;


    mSmootherMask = 0x0000;
    mPrSmCounter = 0;
    mDepthSmCounter = 0;

    mWet_ramp = 1.f;
    mDry_ramp = 1.f;
    mFeed_ramp = 1.f;
    mPreDelayTime_ramp =1.f;
    mDepth_ramp = 1.f;
}


/*****************************************************************************/
/** @brief    processes the incoming samples of both channels
 *  @param    raw left Sample, raw right Sample, Reverb Level from
 *            the feedback mixer
******************************************************************************/

void Reverb::applyReverb(float _EchosSample_L, float _EchosSample_R, float _ReverbLevel)
{
    //***************************** Smoothing ******************************//
    if (mSmootherMask)
    {
        applySmoother();
    }

#if 1
    //**************** Temp. Variables for both Channels *******************//
    float modCoeff_1a, modCoeff_2a, modCoeff_1b, modCoeff_2b;           // Modulation Coefficients
    int32_t ind_tm1, ind_t0, ind_tp1, ind_tp2;                          // Interpolation Points

    float holdSample;                                                   // Holds the state of currently processed sample
    float delaySamples_int, delaySamples_fract;                         // Interger and fraction parts of delaytimes

    float wetSample_L, wetSample_R;                                     // Fully processed Samples
    float wetSample_L2, wetSample_R2;                                   // Processed Samples after Delays L4 and R12

    //************************* Reverb Modulation **************************//
    //// Check Controlrate -> Reaktor does it with half of the SR

    float phase = mLFOStateVar_1 + mLFOWarpedFreq_1;        // phase 1
    phase = phase - round(phase);
    mLFOStateVar_1 = phase;

    phase = (8.f - fabs(phase) * 16.f) * phase;             // par

    phase += 1.f;
    modCoeff_1a = phase * mDepth;
    modCoeff_2a = (1.f - phase) * mDepth;

    phase = mLFOStateVar_2 + mLFOWarpedFreq_2;              // phase 2
    phase = phase - round(phase);
    mLFOStateVar_2 = phase;

    phase = (8.f - fabs(phase) * 16.f) * phase;             // par

    phase += 1.f;
    modCoeff_1b = phase * mDepth;
    modCoeff_2b = (1.f - phase) * mDepth;


    //**********************************************************************//
    //*************************** Left Channel *****************************//
    //**********************************************************************//

    wetSample_L = _EchosSample_L * mFeed;

    //***************************** Asym 2 L *******************************//
    mAsymBuffer_L[mSampleBufferIndx] = wetSample_L;         // write

    if (mPreDelayTime_L > BUFFERSIZE_M1)
    {
        mPreDelayTime_L = BUFFERSIZE_M1;
    }
    else if (mPreDelayTime_L < 0.f)
    {
        mPreDelayTime_L = 0.f;
    }

    delaySamples_int = round(mPreDelayTime_L - 0.5f);
    delaySamples_fract = mPreDelayTime_L - delaySamples_int;

    /// LATCH MISSING (might not need it ... )

    ind_t0 = mSampleBufferIndx - delaySamples_int;
    ind_tm1 = ind_t0 - 1.f;

    ind_t0 &= BUFFERSIZE_M1;
    ind_tm1 &= BUFFERSIZE_M1;

    wetSample_L = mAsymBuffer_L[ind_t0] + delaySamples_fract * (mAsymBuffer_L[ind_t0] - mAsymBuffer_L[ind_tm1]);


    wetSample_L = wetSample_L + mDelayStateVar_L9 * mFBAmnt;

    //*************************** Loop Filter L ****************************//
    wetSample_L = (wetSample_L - mLPStateVar_L * mLPCoeff_2) * mLPCoeff_1;          // LP IIR
    holdSample = mLPStateVar_L;
    mLPStateVar_L = wetSample_L;

    wetSample_L = (wetSample_L + holdSample) * mLpOmega;                            // LP FIR

    wetSample_L = (wetSample_L - mHPStateVar_L * mHPCoeff_2) * mHPCoeff_1;          // HP IIR
    holdSample = mHPStateVar_L;
    mHPStateVar_L = wetSample_L;

    wetSample_L = wetSample_L - holdSample;                                         // HP FIR


    //***************************** Del 4p L1 ******************************//
    holdSample = mDelayStateVar_L1 * mAbAmnt;
    wetSample_L = wetSample_L + (holdSample * GAIN_1);

    mDelayBuffer_L1[mSampleBufferIndx] = wetSample_L;           // Write

    delaySamples_fract = DELAYSAMPLES_1 + modCoeff_2a;

    if (delaySamples_fract > BUFFERSIZE_M2)              // Clip
    {
        delaySamples_fract = BUFFERSIZE_M2;
    }
    else if (delaySamples_fract < 1.f)
    {
        delaySamples_fract = 1.f;
    }

    delaySamples_int = round(delaySamples_fract - 0.5f);
    delaySamples_fract = delaySamples_int - delaySamples_fract;

    ind_t0 = mSampleBufferIndx - delaySamples_int;
    ind_tp1 = ind_t0 + -1;
    ind_tp2 = ind_t0 + -2;

    if (delaySamples_int < 0.f)
    {
        delaySamples_int = 1.f;
    }

    ind_tm1 = mSampleBufferIndx - delaySamples_int;

    ind_tm1 &= BUFFERSIZE_M1;
    ind_t0  &= BUFFERSIZE_M1;
    ind_tp1 &= BUFFERSIZE_M1;
    ind_tp2 &= BUFFERSIZE_M1;

    mDelayStateVar_L1 = NlToolbox::Math::interpolRT(delaySamples_fract,             // 4 Point Interpolation
                                                    mDelayBuffer_L1[ind_tm1],
                                                    mDelayBuffer_L1[ind_t0],
                                                    mDelayBuffer_L1[ind_tp1],
                                                    mDelayBuffer_L1[ind_tp2]);

    wetSample_L = wetSample_L * -GAIN_1 + holdSample;


    //***************************** Del 1p L2 ******************************//
    holdSample = mDelayStateVar_L2 * mAbAmnt;
    wetSample_L = wetSample_L + (holdSample * GAIN_2);

    mDelayBuffer_L2[mSampleBufferIndx] = wetSample_L;           // Write

    ind_t0  = mSampleBufferIndx - DELAYSAMPLES_2;
    ind_t0 &= BUFFERSIZE_M1;

    mDelayStateVar_L2 = mDelayBuffer_L2[ind_t0];

    wetSample_L = wetSample_L *-GAIN_2 + holdSample;


    //***************************** Del 1p L3 ******************************//
    holdSample = mDelayStateVar_L3 * mAbAmnt;
    wetSample_L = wetSample_L + (holdSample * GAIN_3);

    mDelayBuffer_L3[mSampleBufferIndx] = wetSample_L;           // Write

    ind_t0  = mSampleBufferIndx - DELAYSAMPLES_3;
    ind_t0 &= BUFFERSIZE_M1;

    mDelayStateVar_L3 = mDelayBuffer_L3[ind_t0];

    wetSample_L = wetSample_L *-GAIN_3 + holdSample;


    //***************************** Del 1p L4 ******************************//
    holdSample = mDelayStateVar_L4 * mAbAmnt;
    wetSample_L = wetSample_L + (holdSample * GAIN_4);

    mDelayBuffer_L3[mSampleBufferIndx] = wetSample_L;           // Write

    ind_t0  = mSampleBufferIndx - DELAYSAMPLES_4;
    ind_t0 &= BUFFERSIZE_M1;

    mDelayStateVar_L4 = mDelayBuffer_L4[ind_t0];

    wetSample_L = wetSample_L *-GAIN_4 + holdSample;

    wetSample_L2 = wetSample_L;

    //***************************** Del 1p L5 ******************************//
    holdSample = mDelayStateVar_L5 * mAbAmnt;
    wetSample_L = wetSample_L + (holdSample * GAIN_4);

    mDelayBuffer_L4[mSampleBufferIndx] = wetSample_L;           // Write

    ind_t0  = mSampleBufferIndx - DELAYSAMPLES_5;
    ind_t0 &= BUFFERSIZE_M1;

    mDelayStateVar_L5 = mDelayBuffer_L5[ind_t0];

    wetSample_L = wetSample_L *-GAIN_4 + holdSample;


    //***************************** Del 1p L6 ******************************//
    holdSample = mDelayStateVar_L6 * mAbAmnt;
    wetSample_L = wetSample_L + (holdSample * GAIN_4);

    mDelayBuffer_L6[mSampleBufferIndx] = wetSample_L;           // Write

    ind_t0  = mSampleBufferIndx - DELAYSAMPLES_6;
    ind_t0 &= BUFFERSIZE_M1;

    mDelayStateVar_L6 = mDelayBuffer_L6[ind_t0];

    wetSample_L = wetSample_L *-GAIN_4 + holdSample;


    //***************************** Del 1p L7 ******************************//
    holdSample = mDelayStateVar_L7 * mAbAmnt;
    wetSample_L = wetSample_L + (holdSample * GAIN_4);

    mDelayBuffer_L7[mSampleBufferIndx] = wetSample_L;           // Write

    ind_t0  = mSampleBufferIndx - DELAYSAMPLES_7;
    ind_t0 &= BUFFERSIZE_M1;

    mDelayStateVar_L7 = mDelayBuffer_L7[ind_t0];

    wetSample_L = wetSample_L *-GAIN_4 + holdSample;


    //***************************** Del 1p L8 ******************************//
    holdSample = mDelayStateVar_L8 * mAbAmnt;
    wetSample_L = wetSample_L + (holdSample * GAIN_4);

    mDelayBuffer_L8[mSampleBufferIndx] = wetSample_L;           // Write

    ind_t0  = mSampleBufferIndx - DELAYSAMPLES_8;
    ind_t0 &= BUFFERSIZE_M1;

    mDelayStateVar_L8 = mDelayBuffer_L8[ind_t0];

    wetSample_L = wetSample_L *-GAIN_4 + holdSample;


    //***************************** Del 4p L9 ******************************//
    mDelayBuffer_L9[mSampleBufferIndx] = wetSample_L;

    delaySamples_fract = DELAYSAMPLES_L + modCoeff_1a;

    if (delaySamples_fract > BUFFERSIZE_M2)              // Clip
    {
        delaySamples_fract = BUFFERSIZE_M2;
    }
    else if (delaySamples_fract < 0.f)
    {
        delaySamples_fract = 0.f;
    }

    delaySamples_int = round(delaySamples_fract - 0.5f);
    delaySamples_fract = delaySamples_int - delaySamples_fract;

    ind_t0 = mSampleBufferIndx - delaySamples_int;
    ind_tp1 = ind_t0 + -1;
    ind_tp2 = ind_t0 + -2;

    if (delaySamples_int < 1.f)                                 // IClip Lo
    {
        delaySamples_int = 1.f;
    }

    ind_tm1 = mSampleBufferIndx - delaySamples_int;

    ind_tm1 &= BUFFERSIZE_M1;
    ind_t0  &= BUFFERSIZE_M1;
    ind_tp1 &= BUFFERSIZE_M1;
    ind_tp2 &= BUFFERSIZE_M1;

    mDelayStateVar_L9 = NlToolbox::Math::interpolRT(delaySamples_fract,             // 4 Point Interpolation
                                                    mDelayBuffer_L9[ind_tm1],
                                                    mDelayBuffer_L9[ind_t0],
                                                    mDelayBuffer_L9[ind_tp1],
                                                    mDelayBuffer_L9[ind_tp2]);


    //**********************************************************************//
    //************************** Right Channel *****************************//
    //**********************************************************************//
    wetSample_R = _EchosSample_R * mFeed;

    //***************************** Asym 2 R *******************************//
    mAsymBuffer_R[mSampleBufferIndx] = wetSample_R;

    if (mPreDelayTime_R > BUFFERSIZE_M1)
    {
        mPreDelayTime_R = BUFFERSIZE_M1;
    }
    else if (mPreDelayTime_R < 0.f)
    {
        mPreDelayTime_R = 0.f;
    }

    delaySamples_int = round(mPreDelayTime_R - 0.5f);
    delaySamples_fract = mPreDelayTime_R - delaySamples_int;

    /// LATCH MISSING (might not need it ...)

    ind_t0 = mSampleBufferIndx - delaySamples_int;
    ind_tm1 = ind_t0 - 1.f;

    ind_t0 &= BUFFERSIZE_M1;
    ind_tm1 &= BUFFERSIZE_M1;

    wetSample_R = mAsymBuffer_R[ind_t0] + delaySamples_fract * (mAsymBuffer_R[ind_t0] - mAsymBuffer_R[ind_tm1]);

    wetSample_R = wetSample_R + mDelayStateVar_R9 * mFBAmnt;


    //*************************** Loop Filter R ****************************//

    wetSample_R = (wetSample_R - mLPStateVar_R * mLPCoeff_2) * mLPCoeff_1;          // LP IIR
    holdSample = mLPStateVar_R;
    mLPStateVar_R = wetSample_R;

    wetSample_R = (wetSample_R + holdSample) * mLpOmega;                            // LP FIR

    wetSample_R = (wetSample_R - mHPStateVar_R * mHPCoeff_2) * mHPCoeff_1;          // HP IIR
    holdSample = mHPStateVar_R;
    mHPStateVar_R = wetSample_R;

    wetSample_R = wetSample_R - holdSample;                                         // HP FIR


    //***************************** Del 4p R1 ******************************//
    holdSample = mDelayStateVar_R1 * mAbAmnt;
    wetSample_R = wetSample_R + (holdSample * GAIN_1);

    mDelayBuffer_R1[mSampleBufferIndx] = wetSample_R;           // Write

    delaySamples_fract = DELAYSAMPLES_9 + modCoeff_2b;

    if (delaySamples_fract > BUFFERSIZE_M2)              // Clip
    {
        delaySamples_fract = BUFFERSIZE_M2;
    }
    else if (delaySamples_fract < 1.f)
    {
        delaySamples_fract = 1.f;
    }

    delaySamples_int = round(delaySamples_fract - 0.5f);
    delaySamples_fract = delaySamples_int - delaySamples_fract;

    ind_t0 = mSampleBufferIndx - delaySamples_int;
    ind_tp1 = ind_t0 + -1;
    ind_tp2 = ind_t0 + -2;

    if (delaySamples_int < 0.f)
    {
        delaySamples_int = 1.f;
    }

    ind_tm1 = mSampleBufferIndx - delaySamples_int;

    ind_tm1 &= BUFFERSIZE_M1;
    ind_t0  &= BUFFERSIZE_M1;
    ind_tp1 &= BUFFERSIZE_M1;
    ind_tp2 &= BUFFERSIZE_M1;

    mDelayStateVar_R1 = NlToolbox::Math::interpolRT(delaySamples_fract,             // 4 Point Interpolation
                                                    mDelayBuffer_R1[ind_tm1],
                                                    mDelayBuffer_R1[ind_t0],
                                                    mDelayBuffer_R1[ind_tp1],
                                                    mDelayBuffer_R1[ind_tp2]);

    wetSample_R = wetSample_R * -GAIN_1 + holdSample;

    //***************************** Del 1p R2 ******************************//
    holdSample = mDelayStateVar_R2 * mAbAmnt;
    wetSample_R = wetSample_R + (holdSample * GAIN_2);

    mDelayBuffer_R2[mSampleBufferIndx] = wetSample_R;           // Write

    ind_t0  = mSampleBufferIndx - DELAYSAMPLES_10;
    ind_t0 &= BUFFERSIZE_M1;

    mDelayStateVar_R2 = mDelayBuffer_R2[ind_t0];

    wetSample_R = wetSample_R * -GAIN_2 + holdSample;


    //***************************** Del 1p R3 ******************************//
    holdSample = mDelayStateVar_R3 * mAbAmnt;
    wetSample_R = wetSample_R + (holdSample * GAIN_3);

    mDelayBuffer_R3[mSampleBufferIndx] = wetSample_R;           // Write

    ind_t0  = mSampleBufferIndx - DELAYSAMPLES_11;
    ind_t0 &= BUFFERSIZE_M1;

    mDelayStateVar_R3 = mDelayBuffer_R3[ind_t0];

    wetSample_R = wetSample_R * -GAIN_3 + holdSample;


    //***************************** Del 1p R4 ******************************//
    holdSample = mDelayStateVar_R4 * mAbAmnt;
    wetSample_R = wetSample_R + (holdSample * GAIN_4);

    mDelayBuffer_R4[mSampleBufferIndx] = wetSample_R;           // Write

    ind_t0  = mSampleBufferIndx - DELAYSAMPLES_12;
    ind_t0 &= BUFFERSIZE_M1;

    mDelayStateVar_R4 = mDelayBuffer_R4[ind_t0];

    wetSample_R = wetSample_R * -GAIN_4 + holdSample;

    wetSample_R2 = wetSample_R;

    //***************************** Del 1p R5 ******************************//
    holdSample = mDelayStateVar_R5 * mAbAmnt;
    wetSample_R = wetSample_R + (holdSample * GAIN_4);

    mDelayBuffer_R4[mSampleBufferIndx] = wetSample_R;           // Write

    ind_t0  = mSampleBufferIndx - DELAYSAMPLES_13;
    ind_t0 &= BUFFERSIZE_M1;

    mDelayStateVar_R5 = mDelayBuffer_R5[ind_t0];

    wetSample_R = wetSample_R *-GAIN_4 + holdSample;


    //***************************** Del 1p R6 ******************************//
    holdSample = mDelayStateVar_R6 * mAbAmnt;
    wetSample_R = wetSample_R + (holdSample * GAIN_4);

    mDelayBuffer_R6[mSampleBufferIndx] = wetSample_R;           // Write

    ind_t0  = mSampleBufferIndx - DELAYSAMPLES_14;
    ind_t0 &= BUFFERSIZE_M1;

    mDelayStateVar_R6 = mDelayBuffer_R6[ind_t0];

    wetSample_R = wetSample_R *-GAIN_4 + holdSample;


    //***************************** Del 1p R7 ******************************//
    holdSample = mDelayStateVar_R7 * mAbAmnt;
    wetSample_R = wetSample_R + (holdSample * GAIN_4);

    mDelayBuffer_R7[mSampleBufferIndx] = wetSample_R;           // Write

    ind_t0  = mSampleBufferIndx - DELAYSAMPLES_15;
    ind_t0 &= BUFFERSIZE_M1;

    mDelayStateVar_R7 = mDelayBuffer_R7[ind_t0];

    wetSample_R = wetSample_R *-GAIN_4 + holdSample;


    //***************************** Del 1p R8 ******************************//
    holdSample = mDelayStateVar_R8 * mAbAmnt;
    wetSample_R = wetSample_R + (holdSample * GAIN_4);

    mDelayBuffer_R8[mSampleBufferIndx] = wetSample_R;           // Write

    ind_t0  = mSampleBufferIndx - DELAYSAMPLES_16;
    ind_t0 &= BUFFERSIZE_M1;

    mDelayStateVar_R8 = mDelayBuffer_R8[ind_t0];

    wetSample_R = wetSample_R *-GAIN_4 + holdSample;


    //***************************** Del 4p R9 ******************************//
    mDelayBuffer_R9[mSampleBufferIndx] = wetSample_R;

    delaySamples_fract = DELAYSAMPLES_R + modCoeff_1b;

    if (delaySamples_fract > BUFFERSIZE_M2)              // Clip
    {
        delaySamples_fract = BUFFERSIZE_M2;
    }
    else if (delaySamples_fract < 0.f)
    {
        delaySamples_fract = 0.f;
    }

    delaySamples_int = round(delaySamples_fract - 0.5f);
    delaySamples_fract = delaySamples_int - delaySamples_fract;

    ind_t0 = mSampleBufferIndx - delaySamples_int;
    ind_tp1 = ind_t0 + -1;
    ind_tp2 = ind_t0 + -2;

    if (delaySamples_int < 1.f)                                 // IClip Lo
    {
        delaySamples_int = 1.f;
    }

    ind_tm1 = mSampleBufferIndx - delaySamples_int;

    ind_tm1 &= BUFFERSIZE_M1;
    ind_t0  &= BUFFERSIZE_M1;
    ind_tp1 &= BUFFERSIZE_M1;
    ind_tp2 &= BUFFERSIZE_M1;

    mDelayStateVar_R9 = NlToolbox::Math::interpolRT(delaySamples_fract,             // 4 Point Interpolation
                                                    mDelayBuffer_R9[ind_tm1],
                                                    mDelayBuffer_R9[ind_t0],
                                                    mDelayBuffer_R9[ind_tp1],
                                                    mDelayBuffer_R9[ind_tp2]);


    //**************************** Delay Mixer *****************************//
    /// This should not be calculated with every sample, yoar!
    float  balance = mBalance * (2.f - mBalance);
    float balance_1m = 1.f - mBalance;
    balance_1m = balance_1m * (2.f  - balance_1m);

    wetSample_L = wetSample_L * balance + wetSample_L2 * balance_1m;
    wetSample_R = wetSample_R * balance + wetSample_R2 * balance_1m;


    //**********************************************************************//
    //*************************** Output Mixer *****************************//

    mReverbOut_L = _EchosSample_L * mDry + wetSample_L * mWet;
    mReverbOut_R = _EchosSample_R * mDry + wetSample_R * mWet;

#else

    mReverbOut_L = _EchosSample_L;
    mReverbOut_R = _EchosSample_R;

#endif

    //************************** Feedback Mixer ****************************//

    mFeedbackOut = ((_EchosSample_L + _EchosSample_R) * (1.f - _ReverbLevel))
            + ((wetSample_L + wetSample_R) * _ReverbLevel);
}


/*****************************************************************************/
/** @brief    interface method which converts and scales the incoming midi
 *            values and passes these to the respective methods
 *  @param    midi control value [0 ... 127]
 *  @param    midi control ID -> enum CtrlID (reverb.h)
******************************************************************************/

void Reverb::setReverbParams(unsigned char _ctrlID, float _ctrlVal)
{
    switch(_ctrlID)
    {
        case CtrlID::SIZE:
            _ctrlVal = _ctrlVal / 127.f;
            printf("Reverb - Size: %f\n", _ctrlVal);

            mSize = (2.f - fabs(_ctrlVal)) * _ctrlVal;
            calcFeedAndBalance();
            break;

        case CtrlID::COLOR:
            _ctrlVal = _ctrlVal / 127.f;
            printf("Reverb - Color: %f\n", _ctrlVal);

            mColor = _ctrlVal;
            calcFeedAndBalance();
            break;

        case CtrlID::CHORUS:
            _ctrlVal = _ctrlVal / 127.f;
            printf("Reverb - Chorus: %f\n", _ctrlVal);

            mChorus = _ctrlVal;
            break;

        case CtrlID::PRE_DELAY:
            _ctrlVal = _ctrlVal / 127.f;
            printf("Reverb - Pre Delay: %f\n", _ctrlVal);

            _ctrlVal = _ctrlVal * _ctrlVal * _ctrlVal;
            _ctrlVal *= 200.f;

            _ctrlVal = _ctrlVal * (SAMPLERATE / 1000.f);

            // Initialize Smoother ID 4: Predelay Time
            mPreDelayTime_L_target = round(_ctrlVal);
            mPreDelayTime_R_target = round(_ctrlVal * 1.18933);

            mPreDelayTime_L_base = mPreDelayTime_L;
            mPreDelayTime_L_diff = mPreDelayTime_L_target - mPreDelayTime_L_base;

            mPreDelayTime_R_base = mPreDelayTime_R;
            mPreDelayTime_R_diff = mPreDelayTime_R_target - mPreDelayTime_R_base;

            mSmootherMask |= 0x0008;            // Switch fourth bit to 1
            mPreDelayTime_ramp = 0.f;
            break;

        case CtrlID::MIX:
            _ctrlVal = _ctrlVal / 127.f;
            printf("Reverb - Mix: %f\n", _ctrlVal);

            _ctrlVal = _ctrlVal * _ctrlVal;

            // Initialize Smoother ID 1: Dry
            mDry_target = 1.f - _ctrlVal;
            mDry_target = (2.f - mDry_target) * mDry_target;
            mDry_base = mDry;
            mDry_diff = mDry_target - mDry_base;

            mSmootherMask |= 0x0001;            // switch first bit to 1
            mDry_ramp = 0.f;

            // Initialize Smoother ID 2: Wet
            mWet_target = (2.f - _ctrlVal) * _ctrlVal;
            mWet_base = mWet;
            mWet_diff = mWet_target - mWet_base;

            mSmootherMask |= 0x0002;            // switch second bit to 1
            mWet_ramp = 0.f;
            break;
    }
}



/*****************************************************************************/
/** @brief    applies the smoothers of the cabinet module, depending if the
 *            corresponding bit of the mask is set to 1
******************************************************************************/

inline void Reverb::applySmoother()
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

    //*********************** ID 3: Feed Smoother ***************************//
    if (mFeed_ramp < 1.f)
    {
        mFeed_ramp += SMOOTHER_INC;

        if (mFeed_ramp > 1.f)
        {
            mFeed = mFeed_target;
            mSmootherMask &= 0xFFFB;       // switch third bit to 0
        }
        else
        {
            mFeed = mFeed_base + mFeed_diff * mFeed_ramp;
        }
    }

    //******************** ID 4: Pre-Delay Smoother *************************//
    if (mPreDelayTime_ramp < 1.f)
    {
        mPrSmCounter &= 1;
        if (mPrSmCounter == 0)
        {
            mPreDelayTime_ramp += PDELAY_SMOOTHER_INC;

            if (mPreDelayTime_ramp > 1.f)
            {
                mPreDelayTime_L = mPreDelayTime_L_target;
                mPreDelayTime_R = mPreDelayTime_R_target;
                mSmootherMask &= 0xFFF7;        // switch fourth bit to 0
            }
            else
            {
                mPreDelayTime_L = mPreDelayTime_L_base + mPreDelayTime_L_diff * mPreDelayTime_ramp;
                mPreDelayTime_R = mPreDelayTime_R_base + mPreDelayTime_R_diff * mPreDelayTime_ramp;
            }
        }
        mPrSmCounter += 1;
    }

    //********************** ID 5: Depth Smoother ***************************//
    if (mDepth_ramp < 1.f)
    {
        mDepthSmCounter &= 1;
        if (mDepthSmCounter == 0)
        {
            mDepth_ramp += PDELAY_SMOOTHER_INC;

            if (mDepth_ramp > 1.f)
            {
                mDepth = mDepth_target;
                mSmootherMask &= 0xFFEF;
            }
            else
            {
                mDepth = mDepth_base + mDepth_diff * mDepth_ramp;
            }
        }
        mDepthSmCounter += 1;
    }
}



/*****************************************************************************/
/** @brief
******************************************************************************/

void Reverb::calcFeedAndBalance()
{
    float feed = mColor;

    if (feed < 0.66f)               // Norm Latch
    {
        feed = 0.66f;
    }
    else if (feed > 1.f)
    {
        feed = 1.f;
    }

    feed -= 0.66f;
    feed *= 2.94118f;

    feed = feed * feed;

    feed *= 0.46f;                  // Val Crossfade
    feed += 1.f;

    float wetFeed;
    wetFeed = mSize * (0.6f - fabs(mSize) * -0.4f);
    wetFeed *= -3.32f;
    wetFeed += 4.32f;

    // Initialize Smoother ID 3: Feed
    mFeed_target = feed * wetFeed;
    mFeed_base = mFeed;
    mFeed_diff = mFeed_target - mFeed_base;

    mSmootherMask |= 0x0004;        // switch third bit to 1
    mFeed_ramp = 0.f;

    mBalance = mSize * (1.3f - fabs(mSize) * 0.3f);
    mBalance *= 0.9f;

    float size = mSize * -200.f + 311.f;

    mDepth = size * mChorus * mChorus;
    // Initialize Smoother ID 4: Depth
//    mDepth_target = size * mChorus * mChorus;
//    mDepth_base = mDepth;
//    mDepth_diff = mDepth_target - mDepth_base;
}

/*****************************************************************************/
/** @brief
******************************************************************************/

void Reverb::calcLpAndHpCutFreq()
{
    float value = mColor + mColor;

    if (value > 1.f)
    {
        value -= 1.f;
        mLpOmega = NlToolbox::Conversion::pitch2freq(value * -7.f + 137.f);
        mHpOmega = NlToolbox::Conversion::pitch2freq(value * 56.f + 29.f);

    }
    else
    {
        mLpOmega = NlToolbox::Conversion::pitch2freq(value * 71.f + 66.f);
        mHpOmega = NlToolbox::Conversion::pitch2freq(29.f);
    }

    mLpOmega = NlToolbox::Math::tan(mLpOmega * WARPCONST_PI);
    mHpOmega = NlToolbox::Math::tan(mHpOmega * WARPCONST_PI);

    mLPCoeff_1 = 1.f / (mLpOmega + 1.f);
    mLPCoeff_2 = mLpOmega - 1.f;

    mHPCoeff_1 = 1.f / (mHpOmega + 1.f);
    mHPCoeff_2 = mHpOmega - 1.f;

}
