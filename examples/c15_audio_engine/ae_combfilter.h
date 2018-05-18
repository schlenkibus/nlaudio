/******************************************************************************/
/** @file           ae_combfilter.h
    @date           2018-05-10
    @version        1.0
    @author         Anton Schmied
    @brief          Combfilter
    @todo
*******************************************************************************/

#pragma once

#include <array>
#include "nltoolbox.h"
#include "dsp_defines_signallabels.h"

#define COMB_BUFFER_SIZE    8192
#define COMB_BUFFER_SIZE_M1 8191
#define COMB_BUFFER_SIZE_M3 8189

struct ae_combfilter
{
    ae_combfilter();        // Default Contructor

    float m_samplerate;

    float m_sampleComb;     // Generated Sample
    float m_decayStateVar;

    float m_sampleInterval;
    float m_warpConst_PI;
    float m_warpConst_2PI;

    void init(float _samplerate, uint32_t _vn);
    void applyCombfilter(float _sampleA, float _sampleB, float *_signal);

    //**************************** Highpass Filter ****************************//
    void setHighpassCoeffs(float _frequency);

    float m_hpCoeff_b0, m_hpCoeff_b1, m_hpCoeff_a1;
    float m_hpInStateVar, m_hpOutStateVar;

    //***************************** Lowpass Filter ****************************//
    void setLowpassCoeffs(float _frequency);

    float m_lpCoeff;
    float m_lpStateVar;

    //***************************** Allpass Filter ****************************//
    void setAllpassCoeffs(float _frequency, float _resonance);

    float m_apCoeff_1, m_apCoeff_2;
    float m_apStateVar_1;        // Allpass State Variables
    float m_apStateVar_2;
    float m_apStateVar_3;
    float m_apStateVar_4;

    //****************************** Delay/ Decay *****************************//
    void setDelayTime(float _frequency);

    uint32_t m_delayBufferInd;
    std::array<float, COMB_BUFFER_SIZE> m_delayBuffer;

    float m_delaySamples;
    float m_delayFreqClip;
    float m_delayConst;
    float m_delayStateVar;

    void setDecayGain(float _frequency, float _decaytime);

    float m_decayGain;
};
