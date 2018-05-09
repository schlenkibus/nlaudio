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
    tmpVar = tmpVar + 0.f * 1.f;                    /// m_oscB_crossmix * _signal[OSCA_PM_B]
    tmpVar = tmpVar + _feedbackSample * 1.f;        /// * _signal[OSCA_PM_F]


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

    m_oscA_phaseInc = ((m_OscA_randVal_float * _signal[OSC_A_FLUEC] * osc_freq) + osc_freq) * m_sample_interval;           /// multiply by 1/samplerate

    m_oscA_phase_stateVar = tmpVar;

    m_oscA_phase += m_oscA_phaseInc;
    m_oscA_phase -= NlToolbox::Conversion::float2int(m_oscA_phase);

    float oscSampleA = NlToolbox::Math::sinP3_noWarp(tmpVar);


    //**************************** Modulation B ******************************//
    tmpVar = m_oscB_selfmix * 1.f;                  /// _signal[OSCB_PM_B];
    tmpVar = tmpVar + 0.f * 1.f;                    /// m_oscA_crossmix * _signal[OSCB_PM_A];
    tmpVar = tmpVar + _feedbackSample * 1.f;        /// * _signal[OSCB_PM_F]


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

    osc_freq = 0.f;                                 /// _signal[OSC_B_FRQ]
    m_oscB_phaseInc = ((m_OscB_randVal_float * 0.f * osc_freq) + osc_freq) * m_sample_interval;     /// _signal[OSC_B_FLU]

    m_oscB_phase_stateVar = tmpVar;

    m_oscB_phase += m_oscB_phaseInc;
    m_oscB_phase -=  NlToolbox::Conversion::float2int(m_oscB_phase);

    float oscSampleB = NlToolbox::Math::sinP3_noWarp(tmpVar);


    //******************************* Shaper A *******************************//
    tmpVar = 0.f;                                                                   /// _signal[SHPA_DRI] * 0.18f;

    float shaperSampleA = oscSampleA * tmpVar;
    tmpVar = shaperSampleA;

    shaperSampleA = NlToolbox::Math::sinP3_warp(shaperSampleA);
    shaperSampleA = NlToolbox::Others::threeRanges(shaperSampleA, tmpVar, 0.f);     /// _signal[SHPA_FOLD]

    tmpVar = shaperSampleA * shaperSampleA + (-0.5f);

    shaperSampleA = NlToolbox::Others::parAsym(shaperSampleA, tmpVar, 0.f);         /// _signal[SHPA_ASYM]

    //******************************* Shaper B *******************************//
    tmpVar = 0.f;                                                                   /// _signal[SHPB_DRI] * 0.18f;

    float shaperSampleB = oscSampleB * tmpVar;
    tmpVar = shaperSampleB;

    shaperSampleB = NlToolbox::Math::sinP3_warp(shaperSampleB);
    shaperSampleB = NlToolbox::Others::threeRanges(shaperSampleB, tmpVar, 0.f);     /// _signal[SHPB_FOLD]

    tmpVar = shaperSampleB * shaperSampleB + (-0.5f);

    shaperSampleB = NlToolbox::Others::parAsym(shaperSampleB, tmpVar, 0.f);         /// _signal[SHPB_ASYM]

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
