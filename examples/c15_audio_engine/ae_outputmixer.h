/******************************************************************************/
/** @file           ae_outputmixer.h
    @date           2018-05-09
    @version        1.0
    @author         Anton Schmied
    @brief          Outputmixer
    @todo
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "dsp_defines_signallabels.h"
#include "onepolefilters.h"

struct ae_outputmixer
{
    ae_outputmixer();       // Default Constructor

    float m_sampleL, m_sampleR;

    float m_stateVarL[20] = {};
    float m_stateVarR[20] = {};

    uint32_t m_numberOfVoices;
    uint32_t m_voiceCounter;

    void init(float _samplerate, uint32_t _numberOfVoices);
    void mixAndShape(float _sampleA, float _sampleB, float _sampleComb, float _sampleSVFilter, float *_signal);
    void filterAndLevel(float *_signal);


    //*************************** Highpass Filters ****************************//
    struct OnePoleHighpassFilter
    {
        float m_warp_const;
        float m_omegaTan;
        float m_inStateVar, m_outStateVar;
        float m_b0, m_b1, m_a1;

        void initFilter(float _samplerate, float _cutFrequency)
        {
            m_warp_const = 3.14159f / _samplerate;
            m_inStateVar = 0.f;
            m_outStateVar = 0.f;
            setCoeffs(_cutFrequency);
        }

        void setCoeffs(float _cutFrequency)
        {
            _cutFrequency *= m_warp_const;
            m_omegaTan = NlToolbox::Math::tan(_cutFrequency);

            m_a1 = (1.f - m_omegaTan) / (1.f + m_omegaTan);
            m_b0 = 1.f / (1.f + m_omegaTan);
            m_b1 = (1.f / (1.f + m_omegaTan)) * -1.f;
        }

        float applyFilter(float _sample)
        {
            float tmpVar;

            tmpVar  = m_b0 * _sample;
            tmpVar += m_b1 * m_inStateVar;
            tmpVar += m_a1 * m_outStateVar;

            m_inStateVar  = _sample + DNC_CONST;
            m_outStateVar = tmpVar + DNC_CONST;

            return tmpVar;
        }
    }m_highpass_L, m_highpass_R;
};
