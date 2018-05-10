/******************************************************************************/
/** @file           ae_combfilter.h
    @date           2018-05-10
    @version        1.0
    @author         Anton Schmied
    @brief          Combfilter
    @todo
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "dsp_defines_signallabels.h"

struct ae_combfilter
{
    ae_combfilter();        // Default Contructor

    float m_sampleComb;     // Generated Sample
    float m_decayStateVar;

    void init(float _samplerate, uint32_t _vn);
    void applyCombfilter(float _sampleA, float _sampleB, float *_signal);


    //**************************** Highpass Filter ****************************//
    struct OnePoleHighpassFilter
    {
        float m_inStateVar, m_outStateVar;
        float m_b0, m_b1, m_a1;

        void initFilter(float _samplerate, float _cutFrequency)
        {
            m_inStateVar = 0.f;
            m_outStateVar = 0.f;

            _cutFrequency = _cutFrequency * (3.14159f / _samplerate);
            _cutFrequency = NlToolbox::Math::tan(_cutFrequency);

            m_a1 = (1.f - _cutFrequency) / (1.f + _cutFrequency);
            m_b0 = 1.f / (1.f + _cutFrequency);
            m_b1 = (1.f / (1.f + _cutFrequency)) * -1.f;
        }

        inline float applyFilter(float _sample)
        {
            float tmpVar;

            tmpVar  = m_b0 * _sample;
            tmpVar += m_b1 * m_inStateVar;
            tmpVar += m_a1 * m_outStateVar;

            m_inStateVar  = _sample + DNC_CONST;
            m_outStateVar = tmpVar + DNC_CONST;

            return tmpVar;
        }
    }m_highpass;
};
