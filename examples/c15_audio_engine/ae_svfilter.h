/******************************************************************************/
/** @file           ae_svfilter.h
    @date           2018-07-05
    @version        1.0
    @author         Anton Schmied
    @brief          Combfilter
    @todo
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "dsp_defines_signallabels.h"

struct ae_svfilter
{
    ae_svfilter();      // Default Constructor

    float m_sampleSVF;
    float m_warpConst_2PI;

    void init(float _samplerate, uint32_t _vn);
    void applySVFilter(float _sampleA, float _sampleB, float _sampleComb, float *_signal);
    void setSVFilter(float *_signal, float _samplerate);

    float m_first_attenuation, m_second_attenuation;

    //**************************** State Variables ****************************//
    float m_first_fir_stateVar, m_second_fir_stateVar;
    float m_first_int1_stateVar, m_first_int2_stateVar;
    float m_second_int1_stateVar, m_second_int2_stateVar;

    float m_first_sv_sample;
    float m_first_sat_stateVar, m_second_sat_stateVar;
};
