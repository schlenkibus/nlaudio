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
    float osc_freq = _signal[OSC_A_FRQ];

    //**************************** Modulation A ******************************//
    float tmpVar = m_oscA_selfmix * _signal[OSC_A_PMSEA];
    tmpVar = tmpVar + 0.f * 1.f;                    /// m_oscB_crossmix * _signal[OSCA_PM_B]
    tmpVar = tmpVar + _feedbackSample * 1.f;        /// * _signal[OSCA_PM_F]


    //**************************** Oscillator A ******************************//
    tmpVar = m_chirpFilter_A.applyFilter(tmpVar);
    tmpVar += m_oscA_phase;

    tmpVar += (-0.25f);                                     // Wrap
    tmpVar = tmpVar - int(tmpVar + 0.5f);                   // tmpVar -= round(tmpVar);

    if (fabs(m_oscA_phase_stateVar - tmpVar) > 0.5f)        // Check edge
    {
        m_OscA_randVal_int = m_OscA_randVal_int * 1103515245 + 12345;
        m_OscA_randVal_float = static_cast<float>(m_OscA_randVal_int) * 4.5657e-10f;
    }

    m_oscA_phaseInc = ((m_OscA_randVal_float * _signal[OSC_A_FLU] * osc_freq) + osc_freq) * m_sample_interval;           /// multiply by 1/samplerate

    m_oscA_phase_stateVar = tmpVar;

    m_oscA_phase += m_oscA_phaseInc;
    m_oscA_phase = m_oscA_phase - int(m_oscA_phase + 0.5f);  // m_oscA_phase -= round(m_oscA_phase);

    tmpVar += tmpVar;                                   // oscSinP3
    tmpVar = fabs(tmpVar);
    tmpVar = 0.5f - tmpVar;

    float squareTmpVar = tmpVar * tmpVar;
    float oscSampleA = tmpVar * ((2.26548f * squareTmpVar - 5.13274f) * squareTmpVar + 3.14159f);


    //**************************** Modulation B ******************************//
    tmpVar = m_oscB_selfmix * 1.f;                  /// _signal[OSCB_PM_B];
    tmpVar = tmpVar + 0.f * 1.f;                    /// m_oscA_crossmix * _signal[OSCB_PM_A];
    tmpVar = tmpVar + _feedbackSample * 1.f;        /// * _signal[OSCB_PM_F]


    //**************************** Oscillator B ******************************//
    tmpVar = m_chirpFilter_B.applyFilter(tmpVar);
    tmpVar += m_oscB_phase;

    tmpVar += (0.25f);
    tmpVar = tmpVar - int(tmpVar + 0.5f);                   // tmpVar -= round(tmpVar);

    squareTmpVar = tmpVar * tmpVar;
    float oscSampleB = tmpVar * ((2.26548f * squareTmpVar - 5.13274f) * squareTmpVar + 3.14159f);


    //******************************* Shaper A *******************************//
    tmpVar = 0.f;           /// _signal[SHPA_DRI] * 0.18f;

    float shaperSampleA = oscSampleA * tmpVar;
    tmpVar = shaperSampleA;

    shaperSampleA = NlToolbox::Math::sinP3(shaperSampleA);
    shaperSampleA = NlToolbox::Others::threeRanges(shaperSampleA, tmpVar, 0.f);     /// _signal[SHPA_FOLD]

    squareTmpVar = shaperSampleA * shaperSampleA + (-0.5f);

    shaperSampleA = NlToolbox::Others::parAsym(shaperSampleA, squareTmpVar, 0.f);   /// _signal[SHPA_ASYM]

    //******************************* Shaper B *******************************//
    tmpVar = 0.f;           /// _signal[SHPB_DRI] * 0.18f;

    float shaperSampleB = oscSampleB * tmpVar;
    tmpVar = shaperSampleB;

    shaperSampleB = NlToolbox::Math::sinP3(shaperSampleB);
    shaperSampleB = NlToolbox::Others::threeRanges(shaperSampleB, tmpVar, 0.f);     /// _signal[SHPB_FOLD]

    squareTmpVar = shaperSampleB * shaperSampleB + (-0.5f);

    shaperSampleB = NlToolbox::Others::parAsym(shaperSampleB, squareTmpVar, 0.f);   /// _signal[SHPB_ASYM]

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
