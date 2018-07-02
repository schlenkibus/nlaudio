/******************************************************************************/
/** @file           ae_outputmixer.cpp
    @date           2018-05-09
    @version        1.0
    @author         Anton Schmied
    @brief          Outputmixer
    @todo
*******************************************************************************/

#include "ae_outputmixer.h"

/******************************************************************************/
/** @brief
*******************************************************************************/

ae_outputmixer::ae_outputmixer()
{

}



/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_outputmixer::init(float _samplerate, uint32_t _numberOfVoices)
{
    m_sampleL = 0.f;
    m_sampleR = 0.f;

    m_warpedConst_30hz = (6.28319f / _samplerate) * 30.f;

    m_stateVarL.assign(_numberOfVoices, 0.f);
    m_stateVarR.assign(_numberOfVoices, 0.f);

    m_highpass_L.initFilter(_samplerate, NlToolbox::Conversion::pitch2freq(8.f));
    m_highpass_R.initFilter(_samplerate, NlToolbox::Conversion::pitch2freq(8.f));
}



/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_outputmixer::mixAndShape(float _sampleA, float _sampleB, float _sampleComb, float _sampleSVFilter, float *_signal, uint32_t _voiceID)
{
    //******************************* Left Mix *******************************//
    float mainSample = _signal[OUT_A_L] * _sampleA + _signal[OUT_B_L] * _sampleB +      /// _signal[OUT_A_L] and _signal[OUT_B_L]
                        _signal[OUT_CMB_L] * _sampleComb + 1.f * _sampleSVFilter;       /// _signal[OUT_CMB_L] and _signal[OUT_SVF_L]

    //************************* Left Sample Shaper ***************************//
    mainSample *= 0.25f;                                                                /// _signal[OUT_DRV]
    float tmpVar = mainSample;

    mainSample = NlToolbox::Math::sinP3_wrap(mainSample);
    mainSample = NlToolbox::Others::threeRanges(mainSample, tmpVar, 0.5f);              /// _signal[OUT_FLD]

    tmpVar = mainSample * mainSample;
    tmpVar = tmpVar - m_stateVarL[_voiceID];
    m_stateVarL[_voiceID] = tmpVar * m_warpedConst_30hz + m_stateVarL[_voiceID] + DNC_CONST;

    mainSample = NlToolbox::Others::parAsym(mainSample, tmpVar, 0.f);                   /// _signal[OUT_ASM]

    m_sampleL += mainSample;

    //****************************** Right Mix *******************************//
    mainSample = _signal[OUT_A_R] * _sampleA + _signal[OUT_B_R] * _sampleB +            /// _signal[OUT_A_R] and _signal[OUT_B_R]
            _signal[OUT_CMB_R] * _sampleComb + 1.f * _sampleSVFilter;                   /// _signal[OUT_CMB_R] and _signal[OUT_SVF_R]

    //************************ Right Sample Shaper ***************************//
    mainSample *= 0.25f;                                                                /// _signal[OUT_DRV]
    tmpVar = mainSample;

    mainSample = NlToolbox::Math::sinP3_wrap(mainSample);
    mainSample = NlToolbox::Others::threeRanges(mainSample, tmpVar, 0.5f);              /// _signal[OUT_FLD]

    tmpVar = mainSample * mainSample;
    tmpVar = tmpVar - m_stateVarR[_voiceID];
    m_stateVarR[_voiceID] = tmpVar * m_warpedConst_30hz + m_stateVarR[_voiceID] + DNC_CONST;

    mainSample = NlToolbox::Others::parAsym(mainSample, tmpVar, 0.f);                   /// _signal[OUT_ASM]

    m_sampleR += mainSample;
}



/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_outputmixer::filterAndLevel(float *_signal)
{
    m_sampleL = m_highpass_L.applyFilter(m_sampleL);
    m_sampleR = m_highpass_R.applyFilter(m_sampleR);

    m_sampleL *= 1.f;               /// _signal[OUT_MAIN_L]
    m_sampleR *= 1.f;               /// _signal[OUT_MAIN_R]
}
