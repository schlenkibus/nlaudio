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

void ae_soundgenerator::resetPhase(float *_signal)
{
   m_oscA_phase = _signal[OSC_A_PHS];           /// Osc A Phase Label missing
                                        /// Osc B Phase reset missing
}



/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_soundgenerator::generateSound(float _feedbackSample, float *_signal)
{
    //**************************** Get Signals *******************************//
    float oscA_freq = _signal[OSC_A_FRQ];
    float oscA_fluct = _signal[OSC_A_FLU];
    float oscA_pmSelf = _signal[OSC_A_PMSEA];

    //**************************** Modulation A ******************************//
    float tmpVar = m_oscA_selfmix * oscA_pmSelf;
    tmpVar = tmpVar + 0.f * 1.f;                    /// m_oscB_crossmix * oscB_pmCross
    tmpVar = tmpVar + _feedbackSample * 1.f;        /// * oscA_pmFeedback


    //**************************** Oscillator A ******************************//
    tmpVar = m_chirpFilter_A.applyFilter(tmpVar);
    tmpVar += m_oscA_phase;

    tmpVar += (-0.25f);
    tmpVar -= round(tmpVar);                                // Wrap

    if (fabs(m_oscA_phase_stateVar - tmpVar) > 0.5f)        // Check edge
    {
        m_OscA_randVal_int = m_OscA_randVal_int * 1103515245 + 12345;
        m_OscA_randVal_float = static_cast<float>(m_OscA_randVal_int) * 4.5657e-10f;
    }

    m_oscA_phaseInc = ((m_OscA_randVal_float * oscA_fluct * oscA_freq) + oscA_freq) * m_sample_interval;           /// multiply by 1/samplerate

    m_oscA_phase_stateVar = tmpVar;

    m_oscA_phase += m_oscA_phaseInc;
    m_oscA_phase -= round(m_oscA_phase);

    tmpVar += tmpVar;                                   // oscSinP3
    tmpVar = fabs(tmpVar);
    tmpVar = 0.5f - tmpVar;

    float squareTmpVar = tmpVar * tmpVar;
    float oscSampleA = tmpVar * ((2.26548f * squareTmpVar - 5.13274f) * squareTmpVar + 3.14159f);


    //**************************** Modulation B ******************************//

    //**************************** Oscillator B ******************************//

    //******************************* Shaper A *******************************//

    //******************************* Shaper B *******************************//

    //****************************** Crossfades ******************************//
    m_oscA_selfmix = oscSampleA;
    m_sampleA = oscSampleA;
    m_sampleB = 0.f;

    //************************** Envelope Influence **************************//
    m_sampleA *= _signal[ENV_A_SIG];
    m_sampleB *= 0.f;                        // _signal[ENV_B_SIG];

    //**************************** Feedback Mix ******************************//

    //************************** Ring Modulation *****************************//
}
