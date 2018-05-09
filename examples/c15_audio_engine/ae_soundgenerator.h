/******************************************************************************/
/** @file           ae_soundgenerator.h
    @date           2018-03-22
    @version        1.0
    @author         Anton Schmied
    @brief          Soundgenerator which contains both Oscillators and Shapers
                    receiving the rendered parameter signals from the
                    param engine
    @todo
*******************************************************************************/

#pragma once

#include <cmath>
#include "nltoolbox.h"
#include "dsp_defines_signallabels.h"

struct ae_soundgenerator
{
    ae_soundgenerator();            // Default Constructor

    float m_sampleA, m_sampleB;       // Generated Samples

    void init(float _samplerate, uint32_t _vn);
    void generateSound(float _feedbackSample, float *_signal);
    void resetPhase(float _phaseA, float _phaseB);

    //************************** Shared Variables *****************************//
    float m_sample_interval;

    //*********************** Oscillator A Variables **************************//
    float m_oscA_selfmix;
    float m_oscA_crossmix;

    float m_oscA_phase;
    float m_oscA_phase_stateVar;
    float m_oscA_phaseInc;

    int32_t m_OscA_randVal_int;
    float m_OscA_randVal_float;


    //*********************** Oscillator B Variables **************************//
    float m_oscB_selfmix;
    float m_oscB_crossmix;

    float m_oscB_phase;
    float m_oscB_phase_stateVar;
    float m_oscB_phaseInc;

    int32_t m_OscB_randVal_int;
    float m_OscB_randVal_float;

    //***************************** Chirp Filter ******************************//
    struct ChirpFilter
    {
        float m_warp_const;
        float m_omega;
        float m_stateVar;
        float m_a0, m_a1;

        void initFilter(float _samplerate, float _chirpFrequency)
        {
            m_warp_const = 3.14159f / _samplerate;
            m_stateVar = 0.f;
            setCoeffs(_chirpFrequency);
        }

        void setCoeffs(float _chirpFrequency)
        {
            m_omega = _chirpFrequency * m_warp_const;
            m_omega = NlToolbox::Math::tan(m_omega);

            m_a0 = 1.f / (m_omega + 1.f);
            m_a1 = m_omega - 1.f;
        }

        float applyFilter(float _sample)
        {
            _sample = _sample - (m_a1 * m_stateVar);        // IIR
            _sample *= m_a0;

            float tmpVar = _sample;

            _sample = (_sample + m_stateVar) * m_omega;     // FIR
            m_stateVar = tmpVar + 1.e-18f;

            return _sample;
        }
    }m_chirpFilter_A, m_chirpFilter_B;
};
