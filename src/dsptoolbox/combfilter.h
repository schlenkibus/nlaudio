/******************************************************************************/
/** @file		combfilter.h
    @date		2016-12-26
    @version	0.1
    @author		Anton Schmied[2016-12-26]
    @brief		An implementation of the Comb Filter
                as used in the C15 and implemented in Reaktor
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include <array>


class CombFilter                // Commbfilter Class
{
public:

    CombFilter();               // Default Constructor

//    CombFilter();               // Parametrized Constructor

    ~CombFilter(){}             // Class Destructor

    float mCombFilterOut;       // public processed output

    void applyCombFilter(float _sampleA, float _sampleB);
    void applySmoothers();

    void setCombFilterParams(unsigned char _ctrlID, float _ctrlVal);

    void setMainFreq(float _keyPitch);
    void setABSelectAmnt(float _ctrlVal);
    void setPitchEdit(float _ctrlVal);
    void setPitchKeytrackingAmnt(float _ctrlVal);
    void setPitchEnvCAmnt(float _ctrlVal);

    void setPhasemodAmnt(float _ctrlVal);
    void setABPhasemodAmnt(float _ctrlVal);

    void setLowpassFreq();
    void setMainFreq();

private:
    float mSampleRate;
    float mMainFreq;            // Frequency after Pitch Edit
    float mEnv;                 // current vallue of the Envelope C

    float mABSelectAmnt;        // Select Amount between Sample A and B

    float mLowpassPitch;
    float mLowpassKeyTrk;
    float mLowpassEnvC;

    float mPhasemodAmount;      /// Phase Modulation Amount

    float mABPhasemodAmnt;      /// Phase Modulation amount for th incoming samples
    float mABPhasemod;          /// Resulting mix between the two incoming samples

    uint32_t mCFSmootherMask;   // Smoother Mask
    float mInc;                 // Smoother Increment

    // Mask ID: 0
    float mABSelectAmnt_base;
    float mABSelectAmnt_target;
    float mABSelectAmnt_diff;
    float mABSelectAmnt_ramp;

    // Mask ID: 1
    float mABPhasemodAmnt_base;
    float mABPhasemodAmnt_target;
    float mABPhasemodAmnt_diff;
    float mABPhasemodAmnt_ramp;

    //////////////////// Delay Module ////////////////////////
    float mDecayTime;

    std::array<float,8192> mSampleBuffer;
    uint32_t mSampleBufferIndex;

    float mDelaySamples;
    float mDelaySamples_int;
    float mDelaySamples_fract;

    int32_t mInd_tm1;                       // indices for intorpolation
    int32_t mInd_t0;
    int32_t mInd_tp1;
    int32_t mInd_tp2;
    //////////////////////////////////////////////////////////

    /// 1-Pole Lowpass
//    CombFilterLowpass mCBLowpass;

    struct CombFilterLowpass
    {
        CombFilterLowpass()                                               // Default Constructor
            : mSampleRate(48000.f)
            , mStateVar(0.f)
        {
            mClipConstMax = mSampleRate / 4;
            mClipConstMin = mSampleRate / 24576.f;
        }

        CombFilterLowpass(uint32_t _sampleRate, float _cutFrequency)      // Parameterized Constructor
            : mSampleRate(static_cast<float>(_sampleRate))
            , mStateVar(0.f)
        {
            setCoefficient(_cutFrequency);

            mClipConstMax = mSampleRate / 4;
            mClipConstMin = mSampleRate / 24576.f;
        }


        float mSampleRate;
        float mStateVar;
        float mA1;

        float mClipConstMax;
        float mClipConstMin;


        void setCoefficient(float _cutFrequency)        // set HiCut Coefficient
        {
            if(_cutFrequency > mClipConstMax)           // Clip check Max
            {
                _cutFrequency = mClipConstMax;
            }

            if(_cutFrequency < mClipConstMin)           // Clip check Min
            {
                _cutFrequency = mClipConstMin;
            }

            _cutFrequency = _cutFrequency* (3.14159f / mSampleRate);

            _cutFrequency = NlToolbox::Math::tan(_cutFrequency);

            mA1 = (1.f - _cutFrequency) / (1.f + _cutFrequency);
         }


        float applyFilter(float _sample)                    // apply the Lowpass to the incoming sample
        {
            _sample =_sample * (1.f - mA1);
            _sample =+ (mA1 * mStateVar);

            _sample =+ DNC_CONST;

            mStateVar = _sample;
            return _sample;
        }

    }mCBLowpass;

    enum CtrlId: unsigned char          // Enum class for control IDs
    {
#ifdef REMOTE61                         // ReMote 61
        ABCROSSFADE         ,

        PITCHEDIT           ,
        PITCH_KEYTRACKING   ,
        PITCH_ENVCAMOUNT    ,

        DECAYTIME           ,
        DECAY_KEYTRACKING   ,
        DECAY_GATE          ,

        ALLPASSFREQ         ,
        ALLPASSRES          ,
        ALLPASS_KEYTRACKING ,
        ALLPASS_ENVCAMOUNT  ,

        LOWPASSFREQ         ,
        LOWPASS_KEYTRACKING ,
        LOWPASS_ENVCAMOUNT  ,

        PHASEMODAMOUNT      ,
        AB_PHASEMOD
#endif
    };
};
