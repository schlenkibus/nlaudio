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

    void setCombFilterParams(unsigned char _ctrlID, float _ctrlVal);

    void setLowpassFreq();

private:
    float mSampleRate;
    float mPitch;               // cutrrent pitch of the voice
    float mEnv;                 // current vallue of the Envelope C

    float mLowpassPitch;
    float mLowpassKeyTrk;
    float mLowpassEnvC;


    /// SMOOTHING
    uint32_t mCFSmootherMask;
    float inc;

    float mDecayTime;

    std::array<float,8192> mSampleBuffer;
    uint32_t mSampleBufferIndex;

    /// 1-Pole Lowpass
    CombFilterLowpass mCBLowpass;

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

            _cutFrequency =* (3.14159f / mSampleRate);

            _cutFrequency = NlToolbox::Math::tan(_cutFrequency);

            mA1 = (1.f - _cutFrequency) / (1.f + _cutFrequency);
         }


        float applyFilter(float _sample)                    // apply the Lowpass to the incoming sample
        {
            _sample =* (1.f - mA1);
            _sample =+ (mA1 * mStateVar);

            _sample =+ DNC_CONST;

            mStateVar = _sample;
            return _sample;
        }

    };

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
