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

    m_numberOfVoices = _numberOfVoices;
    m_voiceCounter = _numberOfVoices - 1;

    m_stateVarL.assign(_numberOfVoices, 0.f);
    m_stateVarR.assign(_numberOfVoices, 0.f);

    m_highpass_L.initFilter(_samplerate, NlToolbox::Conversion::pitch2freq(8.f));
    m_highpass_R.initFilter(_samplerate, NlToolbox::Conversion::pitch2freq(8.f));
}



/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_outputmixer::mixAndShape(float _sampleA, float _sampleB, float _sampleComb, float _sampleSVFilter, float *_signal)
{
    m_voiceCounter = (m_voiceCounter + 1) % m_numberOfVoices;
    if (m_voiceCounter == 0)
    {
        m_sampleL = 0.f;
        m_sampleR = 0.f;
    }


    //******************************* Left Mix *******************************//
    float mainSample = 1.f * _sampleA + 1.f * _sampleB +                                /// _signal[OUT_A_L] and _signal[OUT_B_L]
                        1.f * _sampleComb + 1.f * _sampleSVFilter;                      /// _signal[OUT_COMB_L] and _signal[OUT_SVF_L]

    //************************* Left Sample Shaper ***************************//
    mainSample *= 0.25f;                                                                /// _signal[OUT_DRVE]
    float tmpVar = mainSample;

    mainSample = NlToolbox::Math::sinP3_warp(mainSample);
    mainSample = NlToolbox::Others::threeRanges(mainSample, tmpVar, 0.5f);              /// _signal[OUT_FLD]

    tmpVar = mainSample * mainSample;
    tmpVar = tmpVar * m_stateVarL[m_voiceCounter];
    m_stateVarL[m_voiceCounter] = tmpVar * 0.00427428f + m_stateVarL[m_voiceCounter] + DNC_CONST;       /// recalculate the factor, it'S dependant on samplerate

    mainSample = NlToolbox::Others::parAsym(mainSample, tmpVar, 0.f);                   /// _signal[OUT_ASM]

    m_sampleL += mainSample;

    //****************************** Right Mix *******************************//
    mainSample = 1.f * _sampleA + 1.f * _sampleB +                                      /// _signal[OUT_A_R] and _signal[OUT_B_R]
            1.f * _sampleComb + 1.f * _sampleSVFilter;                                  /// _signal[OUT_COMB_R] and _signal[OUT_SVF_R]

    //************************ Right Sample Shaper ***************************//
    mainSample *= 0.25f;                                                                /// _signal[OUT_DRVE]
    tmpVar = mainSample;

    mainSample = NlToolbox::Math::sinP3_warp(mainSample);
    mainSample = NlToolbox::Others::threeRanges(mainSample, tmpVar, 0.5f);              /// _signal[OUT_FLD]

    tmpVar = mainSample * mainSample;
    tmpVar = tmpVar * m_stateVarR[m_voiceCounter];
    m_stateVarR[m_voiceCounter] = tmpVar * 0.00427428f + m_stateVarR[m_voiceCounter] + DNC_CONST;  /// recalculate the factor, it'S dependant on samplerate

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
