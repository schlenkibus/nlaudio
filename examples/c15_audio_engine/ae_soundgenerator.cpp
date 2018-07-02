/******************************************************************************/
/** @file           ae_soundgenerator.cpp
    @date           2018-03-22
    @version        1.0
    @author         Anton Schmied
    @brief          Soundgenerator which contains both Oscillators and Shapers
                    receiving the rendered parameter signals from the
                    param engine
    @todo
*******************************************************************************/

#include "ae_soundgenerator.h"

/******************************************************************************/
/** @brief
*******************************************************************************/

ae_soundgenerator::ae_soundgenerator()
{

}



/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_soundgenerator::init(float _samplerate, uint32_t _vn)
{
    m_sampleA = 0.f;
    m_sampleB = 0.f;

    m_sample_interval = 1.f / _samplerate;

    m_chirpFilter_A.initFilter(_samplerate, 7677.f);
    m_chirpFilter_B.initFilter(_samplerate, 7677.f);

    m_OscA_randVal_int = _vn + 1;
    m_OscB_randVal_int = _vn + 1 + 111;

}



/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_soundgenerator::resetPhase(float _phaseA, float _phaseB)
{
   m_oscA_phase = _phaseA;
   m_oscB_phase = _phaseB;
}



/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_soundgenerator::generateSound(float _feedbackSample, float *_signal)
{
    //**************************** Modulation A ******************************//
    float tmpVar = m_oscA_selfmix * _signal[OSC_A_PMSEA];
    tmpVar = tmpVar + m_oscB_crossmix * _signal[OSC_A_PMBEB];
    tmpVar = tmpVar + _feedbackSample * _signal[OSC_A_PMFEC];


    //**************************** Oscillator A ******************************//
    tmpVar = m_chirpFilter_A.applyFilter(tmpVar);
    tmpVar += m_oscA_phase;

    tmpVar += (-0.25f);                                         // Wrap
    tmpVar -= NlToolbox::Conversion::float2int(tmpVar);

    if (fabs(m_oscA_phase_stateVar - tmpVar) > 0.5f)            // Check edge
    {
        m_OscA_randVal_int = m_OscA_randVal_int * 1103515245 + 12345;
        m_OscA_randVal_float = static_cast<float>(m_OscA_randVal_int) * 4.5657e-10f;
    }

    float osc_freq = _signal[OSC_A_FRQ];
    m_oscA_phaseInc = ((m_OscA_randVal_float * _signal[OSC_A_FLUEC] * osc_freq) + osc_freq) * m_sample_interval;

    m_oscA_phase_stateVar = tmpVar;

    m_oscA_phase += m_oscA_phaseInc;
    m_oscA_phase -= NlToolbox::Conversion::float2int(m_oscA_phase);

    float oscSampleA = NlToolbox::Math::sinP3_noWrap(tmpVar);


    //**************************** Modulation B ******************************//
    tmpVar = m_oscB_selfmix * _signal[OSC_B_PMSEB];
    tmpVar = tmpVar + m_oscA_crossmix * _signal[OSC_B_PMAEA];
    tmpVar = tmpVar + _feedbackSample * _signal[OSC_B_PMFEC];


    //**************************** Oscillator B ******************************//
    tmpVar = m_chirpFilter_B.applyFilter(tmpVar);
    tmpVar += m_oscB_phase;

    tmpVar += (-0.25f);                                         // Warp
    tmpVar -= NlToolbox::Conversion::float2int(tmpVar);

    if (fabs(m_oscB_phase_stateVar - tmpVar) > 0.5f)            // Check edge
    {
        m_OscB_randVal_int = m_OscB_randVal_int * 1103515245 + 12345;
        m_OscB_randVal_float = static_cast<float>(m_OscB_randVal_int) * 4.5657e-10f;
    }

    osc_freq = _signal[OSC_B_FRQ];
    m_oscB_phaseInc = ((m_OscB_randVal_float * _signal[OSC_B_FLUEC] * osc_freq) + osc_freq) * m_sample_interval;

    m_oscB_phase_stateVar = tmpVar;

    m_oscB_phase += m_oscB_phaseInc;
    m_oscB_phase -=  NlToolbox::Conversion::float2int(m_oscB_phase);

    float oscSampleB = NlToolbox::Math::sinP3_noWrap(tmpVar);


    //******************************* Shaper A *******************************//
    float shaperSampleA = oscSampleA * _signal[SHP_A_DRVEA];
    tmpVar = shaperSampleA;

    shaperSampleA = NlToolbox::Math::sinP3_wrap(shaperSampleA);
    shaperSampleA = NlToolbox::Others::threeRanges(shaperSampleA, tmpVar, _signal[SHP_A_FLD]);

    tmpVar = shaperSampleA * shaperSampleA + (-0.5f);

    shaperSampleA = NlToolbox::Others::parAsym(shaperSampleA, tmpVar, _signal[SHP_A_ASM]);


    //******************************* Shaper B *******************************//
    float shaperSampleB = oscSampleB * _signal[SHP_B_DRVEB];
    tmpVar = shaperSampleB;

    shaperSampleB = NlToolbox::Math::sinP3_wrap(shaperSampleB);
    shaperSampleB = NlToolbox::Others::threeRanges(shaperSampleB, tmpVar, _signal[SHP_B_FLD]);

    tmpVar = shaperSampleB * shaperSampleB + (-0.5f);

    shaperSampleB = NlToolbox::Others::parAsym(shaperSampleB, tmpVar, _signal[SHP_B_ASM]);

    //****************************** Crossfades ******************************//
    m_oscA_selfmix  = NlToolbox::Crossfades::bipolarCrossFade(oscSampleA, shaperSampleA, _signal[OSC_A_PMSSH]);
    m_oscA_crossmix = NlToolbox::Crossfades::bipolarCrossFade(oscSampleA, shaperSampleA, _signal[OSC_B_PMASH]);

    m_oscB_selfmix  = NlToolbox::Crossfades::bipolarCrossFade(oscSampleB, shaperSampleB, _signal[OSC_B_PMSSH]);
    m_oscB_crossmix = NlToolbox::Crossfades::bipolarCrossFade(oscSampleB, shaperSampleB, _signal[OSC_A_PMBSH]);

    m_sampleA = NlToolbox::Crossfades::bipolarCrossFade(oscSampleA, shaperSampleA, _signal[SHP_A_MIX]);
    m_sampleB = NlToolbox::Crossfades::bipolarCrossFade(oscSampleB, shaperSampleB, _signal[SHP_B_MIX]);


    //************************** Envelope Influence **************************//
    m_sampleA *= _signal[ENV_A_SIG];
    m_sampleB *= _signal[ENV_B_SIG];


    //**************************** Feedback Mix ******************************//
    /// -> the crossfade for ENV_G_SIG, ENV_C_SIG and SHP_A_FBEC/SHP_B_FBEC will soon be part of processing
    tmpVar    = NlToolbox::Crossfades::unipolarCrossFade(_signal[ENV_G_SIG], _signal[ENV_C_SIG], _signal[SHP_A_FBEC]);
    tmpVar   *= _feedbackSample;
    m_sampleA = NlToolbox::Crossfades::unipolarCrossFade(m_sampleA, tmpVar, _signal[SHP_A_FBM]);

    tmpVar    = NlToolbox::Crossfades::unipolarCrossFade(_signal[ENV_G_SIG], _signal[ENV_C_SIG], _signal[SHP_B_FBEC]);
    tmpVar   *= _feedbackSample;
    m_sampleB = NlToolbox::Crossfades::unipolarCrossFade(m_sampleB, tmpVar, _signal[SHP_B_FBM]);


    //************************** Ring Modulation *****************************//
    tmpVar = m_sampleA * m_sampleB;

    m_sampleA = NlToolbox::Crossfades::unipolarCrossFade(m_sampleA, tmpVar, _signal[SHP_A_RM]);
    m_sampleB = NlToolbox::Crossfades::unipolarCrossFade(m_sampleB, tmpVar, _signal[SHP_B_RM]);
}
