/******************************************************************************/
/** @file           ae_combfilter.h
    @date           2018-05-10
    @version        1.0
    @author         Anton Schmied
    @brief          Combfilter
    @todo
*******************************************************************************/


#include "ae_combfilter.h"

/******************************************************************************/
/** @brief
*******************************************************************************/

ae_combfilter::ae_combfilter()
{

}



/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_combfilter::init(float _samplerate, uint32_t _vn)
{
    m_sampleComb = 0.f;
    m_decayStateVar = 0.f;

    m_highpass.initFilter(_samplerate, 60.f);
    m_highpass.initFilter(_samplerate, 60.f);

}



/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_combfilter::applyCombfilter(float _sampleA, float _sampleB, float *_signal)
{
    float tmpVar;

    //**************************** AB Sample Mix ****************************//
    tmpVar = _signal[0];                /// _signal[CMB_AB]
    m_sampleComb = _sampleA * (1.f - tmpVar) + _sampleB * tmpVar;
    m_sampleComb *= m_decayStateVar;


    //****************** AB Ssample Phase Mdulation Mix ********************//
    tmpVar = _signal[0];                /// _signal[CMB_PH_AB]
    float phaseMod = _sampleA * (1.f - tmpVar) + _sampleB * tmpVar;
    phaseMod *= _signal[0];             /// _signal[CMB_PH]


    //************************** 1-Pole Highpass ****************************//
    m_sampleComb = m_highpass.applyFilter(m_sampleComb);


    //*************************** 1-Pole Lowpass ****************************//



}
