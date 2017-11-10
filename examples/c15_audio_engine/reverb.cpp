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

    mSize =  (2.f - fabs(0.5f)) * 0.5f;

    mFeedWetness = mSize * (0.6f - fabs(mSize) * -0.4f);
    mFeedWetness *= -3.32f;
    mFeedWetness += 4.32f;

    mDepthSize = 311.f + mSize * -200.f;

    mBalance = (mSize * (1.3f - 0.3f * fabs(mSize))) * 0.9f;
    mBalance_half = 1.f - mBalance * mBalance;
    mBalance_full = mBalance * (2.f - mBalance);

    mSize = mSize * (0.5f - fabs(mSize) * -0.5f);
    mAbAmnt = mSize * 0.334f + 0.666f;
    mFBAmnt = mSize * 0.667f + 0.333f;

    mFeedColor = 1.f;
    mFeed = mFeedColor * mFeedWetness;

    mDepthChorus = 0.0625f;
    mDepth = mDepthSize * mDepthChorus;

    mLPOmega = 0.838423f;
    mLPCoeff_1 = 1.f / (mLPOmega + 1.f);
    mLPCoeff_2 = mLPOmega - 1.f;

    mHPOmega = 0.0031101f;
    mHPCoeff_1 = 1.f / (mHPOmega + 1.f);
    mHPCoeff_2 = mHPOmega - 1.f;

    mLPStateVar_L = 0.f;
    mLPStateVar_R = 0.f;
    mHPStateVar_L = 0.f;
    mHPStateVar_R = 0.f;

    mDry = 1.f;
    mWet = 0.f;

    mPreDelayTime_L = 138;
    mPreDelayTime_R = 164;

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

    mSmootherMask = 0x0000;

    mBalance_ramp = 1.f;
    mSize_ramp = 1.f;
    mLpFltr_ramp = 1.f;
    mPreDelayTime_ramp = 1.f;
    mDepth_ramp = 1.f;
    mFeed_ramp = 1.f;
    mMix_ramp = 1.f;
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

    if (mDepth > 0.f)
    {
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
    }
    else
    {
        mLFOStateVar_1 = 0.f;
        mLFOStateVar_2 = 0.f;

        modCoeff_1a = 0.f;
        modCoeff_2a = 0.f;
        modCoeff_1b = 0.f;
        modCoeff_2b = 0.f;
    }


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

    wetSample_L = mAsymBuffer_L[ind_t0] + delaySamples_fract * (mAsymBuffer_L[ind_tm1] - mAsymBuffer_L[ind_t0]);


    wetSample_L = wetSample_L + mDelayStateVar_L9 * mFBAmnt;

    //*************************** Loop Filter L ****************************//
    wetSample_L = (wetSample_L - mLPStateVar_L * mLPCoeff_2) * mLPCoeff_1;          // LP IIR
    holdSample = mLPStateVar_L;
    mLPStateVar_L = wetSample_L;

    wetSample_L = (wetSample_L + holdSample) * mLPOmega;                            // LP FIR

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
    ind_tm1 += 1;

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

    wetSample_R = mAsymBuffer_R[ind_t0] + delaySamples_fract * (mAsymBuffer_R[ind_tm1] - mAsymBuffer_R[ind_t0]);

    wetSample_R = wetSample_R + mDelayStateVar_R9 * mFBAmnt;


    //*************************** Loop Filter R ****************************//

    wetSample_R = (wetSample_R - mLPStateVar_R * mLPCoeff_2) * mLPCoeff_1;          // LP IIR
    holdSample = mLPStateVar_R;
    mLPStateVar_R = wetSample_R;

    wetSample_R = (wetSample_R + holdSample) * mLPOmega;                            // LP FIR

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
    ind_tm1 += 1;

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

    mSampleBufferIndx = (mSampleBufferIndx + 1) & BUFFERSIZE_M1;

    //**************************** Delay Mixer *****************************//
    wetSample_L = wetSample_L * mBalance_full + wetSample_L2 * mBalance_half;
    wetSample_R = wetSample_R * mBalance_full + wetSample_R2 * mBalance_half;


    //**********************************************************************//
    //*************************** Output Mixer *****************************//

    mReverbOut_L = _EchosSample_L * mDry + wetSample_L * mWet;
    mReverbOut_R = _EchosSample_R * mDry + wetSample_R * mWet;

#else

    float wetSample_L = 0.f;
    float wetSample_R = 0.f;

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

            _ctrlVal = (2.f - fabs(_ctrlVal)) * _ctrlVal;

            // Size for Feed
            mFeedWetness = _ctrlVal * (0.6f - fabs(_ctrlVal) * -0.4f);
            mFeedWetness *= -3.32f;
            mFeedWetness += 4.32f;
            initFeedSmoother();

            // Size for Depth
            mDepthSize = 311.f + _ctrlVal * -200.f;
            initDepthSmoother();

            // Size for Balance
            // Initialize Smoother ID: 1 Balance
            mBalance_target = (_ctrlVal * (1.3f - 0.3f * fabs(_ctrlVal))) * 0.9f;
            mBalance_base = mBalance;
            mBalance_diff = mBalance_target - mBalance_base;

            mSmootherMask |= 0x0001;
            mBalance_ramp = 0.f;

            // Size for Absorb and Feedback Amounts
            // Initialize Smoother ID: 2 Size
            mSize_target = _ctrlVal * (0.5f - fabs(_ctrlVal) * -0.5f);
            mSize_base = mSize;
            mSize_diff = mSize_target - mSize_base;

            mSmootherMask |= 0x0002;
            mSize_ramp = 0.f;
            break;

        case CtrlID::COLOR:
            _ctrlVal = _ctrlVal / 127.f;
            printf("Reverb - Color: %f\n", _ctrlVal);

            // Color for Feed
            mFeedColor = _ctrlVal;

            if (mFeedColor < 0.66f)               // Norm Latch
            {
                mFeedColor = 0.66f;
            }
            else if (mFeedColor > 1.f)
            {
                mFeedColor = 1.f;
            }

            mFeedColor -= 0.66f;
            mFeedColor *= 2.94118f;

            mFeedColor = mFeedColor * mFeedColor;

            mFeedColor *= 0.46f;                  // Val Crossfade
            mFeedColor += 1.f;
            initFeedSmoother();

            // Color for Loop Filter
            _ctrlVal = _ctrlVal + _ctrlVal;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal -= 1.f;
                mLPOmega_target = NlToolbox::Conversion::pitch2freq(_ctrlVal * -7.f + 137.f);
                mHPOmega_target = NlToolbox::Conversion::pitch2freq(_ctrlVal * 56.f + 29.f);
            }
            else
            {
                mLPOmega_target = NlToolbox::Conversion::pitch2freq(_ctrlVal * 71.f + 66.f);
                mHPOmega_target = NlToolbox::Conversion::pitch2freq(29.f);
            }

            mLPOmega_target = NlToolbox::Math::tan(mLPOmega_target * WARPCONST_PI);
            mHPOmega_target = NlToolbox::Math::tan(mHPOmega_target * WARPCONST_PI);

            mLPOmega_base = mLPOmega;
            mHPOmega_base = mHPOmega;

            mLPOmega_diff = mLPOmega_target - mLPOmega_base;
            mHPOmega_diff = mHPOmega_target - mHPOmega_base;

            mSmootherMask |= 0x0004;
            mLpFltr_ramp = 0.f;
            break;

        case CtrlID::CHORUS:
            _ctrlVal = _ctrlVal / 127.f;
            printf("Reverb - Chorus: %f\n", _ctrlVal);

            mDepthChorus = _ctrlVal * _ctrlVal;
            initDepthSmoother();
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

            // Initialize Smoother ID 7: Dry and Wet
            mDry_target = 1.f - _ctrlVal;
            mDry_target = (2.f - mDry_target) * mDry_target;
            mDry_base = mDry;
            mDry_diff = mDry_target - mDry_base;

            mWet_target = (2.f - _ctrlVal) * _ctrlVal;
            mWet_base = mWet;
            mWet_diff = mWet_target - mWet_base;

            mSmootherMask |= 0x0040;            // switch seventh bit to 1
            mMix_ramp = 0.f;
            break;
    }
}



/*****************************************************************************/
/** @brief    applies the smoothers of the cabinet module, depending if the
 *            corresponding bit of the mask is set to 1
******************************************************************************/

inline void Reverb::applySmoother()
{
    //********************** ID 1: Balance Smoother *************************//
    if (mBalance_ramp < 1.f)
    {
        mBalance_ramp += REVERB_SMOOTHER_INC;

        if (mBalance_ramp > 1.f)
        {
            mBalance = mBalance_target;
            mSmootherMask &= 0xFFFE;            // switch first bit to 0
        }
        else
        {
            mBalance = mBalance_base + mBalance_diff * mBalance_ramp;
        }

        mBalance_half = 1.f - mBalance * mBalance;
        mBalance_full = mBalance * (2.f - mBalance);
    }


    //********************** ID 2: Balance Smoother *************************//
    if (mSize_ramp < 1.f)
    {
        mSize_ramp += REVERB_SMOOTHER_INC;

        if (mSize_ramp > 1.f)
        {
            mSize = mSize_target;
            mSmootherMask &= 0xFFFD;            // switch second bit to 0
        }
        else
        {
            mSize = mSize_base + mSize_diff * mSize_ramp;
        }

        mAbAmnt = mSize * 0.334f + 0.666f;
        mFBAmnt = mSize * 0.667f + 0.333f;
    }


    //******************* ID 3: Loop Filter Smoother ************************//
    if (mLpFltr_ramp < 1.f)
    {
        mLpFltr_ramp += REVERB_SMOOTHER_INC;

        if (mLpFltr_ramp > 1.f)
        {
            mLPOmega = mLPOmega_target;
            mHPOmega = mHPOmega_target;
            mSmootherMask &= 0xFFFB;            // switch third bit to 0
        }
        else
        {
            mLPOmega = mLPOmega_base + mLPOmega_diff * mLpFltr_ramp;
            mHPOmega = mHPOmega_base + mHPOmega_diff * mLpFltr_ramp;
        }

        mLPCoeff_1 = 1.f / (mLPOmega + 1.f);
        mLPCoeff_2 = mLPOmega - 1.f;

        mHPCoeff_1 = 1.f / (mHPOmega + 1.f);
        mHPCoeff_2 = mHPOmega - 1.f;
    }


    //******************** ID 4: Pre-Delay Smoother *************************//
    if (mPreDelayTime_ramp < 1.f)
    {
            mPreDelayTime_ramp += REVERB_SMOOTHER_INC;

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


    //********************** ID 5: Depth Smoother ***************************//
    if (mDepth_ramp < 1.f)
    {
        mDepth_ramp += REVERB_SMOOTHER_INC;

        if (mDepth_ramp > 1.f)
        {
            mDepth = mDepth_target;
            mSmootherMask &= 0xFFEF;       // switch fifth bit to 0
        }
        else
        {
            mDepth = mDepth_base + mDepth_diff * mDepth_ramp;
        }
    }


    //*********************** ID 6: Feed Smoother ***************************//
    if (mFeed_ramp < 1.f)
    {
        mFeed_ramp += SMOOTHER_INC;

        if (mFeed_ramp > 1.f)
        {
            mFeed = mFeed_target;
            mSmootherMask &= 0xFFDF;       // switch sixth bit to 0
        }
        else
        {
            mFeed = mFeed_base + mFeed_diff * mFeed_ramp;
        }
    }


    //************************ ID 7: Mix Smoother ***************************//
    if (mMix_ramp < 1.f)
    {
        mMix_ramp += SMOOTHER_INC;

        if (mMix_ramp > 1.f)
        {
            mDry = mDry_target;
            mWet = mWet_target;
            mSmootherMask &= 0xFFBF;       // switch seventh bit to 0
        }
        else
        {
            mDry = mDry_base + mDry_diff * mMix_ramp;
            mWet = mWet_base + mWet_diff * mMix_ramp;
        }
    }
}



/*****************************************************************************/
/** @brief
******************************************************************************/

void Reverb::initFeedSmoother()
{
    // Initialize Smoother ID 6: Feed
    mFeed_target = mFeedColor * mFeedWetness;
    mFeed_base = mFeed;
    mFeed_diff = mFeed_target - mFeed_base;

    mSmootherMask |= 0x0020;        // switch sixth bit to 1
    mFeed_ramp = 0.f;
}



/*****************************************************************************/
/** @brief
******************************************************************************/

void Reverb::initDepthSmoother()
{
    // Initialize Smoother ID 5: Depth
    mDepth_target = mDepthSize * mDepthChorus;
    mDepth_base = mDepth;
    mDepth_diff = mDepth_target - mDepth_base;

    mSmootherMask |= 0x00E0;
    mDepth_ramp = 0.f;
}
