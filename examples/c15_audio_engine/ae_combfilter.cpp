/******************************************************************************/
/** @file           ae_combfilter.cpp
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

    m_sampleInterval = 1.f / _samplerate;
    m_warpConst_PI  = 3.14159f / _samplerate;
    m_warpConst_2PI = 6.28319f / _samplerate;
    m_freqClip_24576 = _samplerate / 24576.f;
    m_freqClip_2 =  _samplerate / 2.125f;
    m_freqClip_4 = _samplerate / 4.f;

    //***************************** Highpass *********************************//
    m_hpInStateVar  = 0.f;
    m_hpOutStateVar = 0.f;

    //***************************** Lowpass **********************************//
    m_lpStateVar    = 0.f;

    //****************************** Allpass *********************************//
    m_apStateVar_1  = 0.f;
    m_apStateVar_2  = 0.f;
    m_apStateVar_3  = 0.f;
    m_apStateVar_4  = 0.f;

    //***************************** Delay ************************************//
    m_delayBufferInd = 0;
    m_delayBuffer = {};

    m_delayFreqClip = _samplerate / (COMB_BUFFER_SIZE - 2);
    m_delayConst = 0.693147f / (0.0025f * _samplerate);          // 25ms
    m_delaySamples = 0.f;
    m_delayStateVar = 0.f;
}




/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_combfilter::setDelaySmoother()
{
    m_delayStateVar = m_delaySamples;
}


/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_combfilter::applyCombfilter(float _sampleA, float _sampleB, float *_signal)
{
    float tmpVar;

    //**************************** AB Sample Mix ****************************//
    tmpVar = _signal[CMB_AB];                                                       // AB Mix is inverted, so crossfade mix is as well (currently)
    m_sampleComb  = _sampleB * (1.f - tmpVar) + _sampleA * tmpVar;

    //****************** AB Ssample Phase Mdulation Mix ********************//
    tmpVar = _signal[CMB_PMAB];
    float phaseMod = _sampleA * (1.f - tmpVar) + _sampleB * tmpVar;
    phaseMod *= _signal[CMB_PM];


    //************************** 1-Pole Highpass ****************************//
    tmpVar  = m_hpCoeff_b0 * m_sampleComb;
    tmpVar += (m_hpCoeff_b1 * m_hpInStateVar);
    tmpVar += (m_hpCoeff_a1 * m_hpOutStateVar);

    m_hpInStateVar  = m_sampleComb + DNC_CONST;
    m_hpOutStateVar = tmpVar + DNC_CONST;

    m_sampleComb = tmpVar;
    m_sampleComb += m_decayStateVar;

    //*************************** 1-Pole Lowpass ****************************//
    m_sampleComb *= (1.f - m_lpCoeff);
    m_sampleComb += (m_lpCoeff * m_lpStateVar);
    m_sampleComb += DNC_CONST;
    m_lpStateVar = m_sampleComb;


    //******************************* Allpass *******************************//
    tmpVar = m_sampleComb;

    m_sampleComb *= m_apCoeff_2;
    m_sampleComb += (m_apStateVar_1 * m_apCoeff_1);
    m_sampleComb += m_apStateVar_2;

    m_sampleComb -= (m_apStateVar_3 * m_apCoeff_1);
    m_sampleComb -= (m_apStateVar_4 * m_apCoeff_2);

    m_sampleComb += DNC_CONST;

    m_apStateVar_2 = m_apStateVar_1;
    m_apStateVar_1 = tmpVar;

    m_apStateVar_4 = m_apStateVar_3;
    m_apStateVar_3 = m_sampleComb;


    //****************************** Para D ********************************//
    if (fabs(m_sampleComb) > 0.501187f)
    {
        if (m_sampleComb > 0.f)
        {
            m_sampleComb -= 0.501187f;
            tmpVar = m_sampleComb;

            if (m_sampleComb > 2.98815f)
            {
                m_sampleComb = 2.98815f;
            }

            m_sampleComb *= (1.f - m_sampleComb * 0.167328f);

            m_sampleComb *= 0.7488f;
            tmpVar *= 0.2512f;

            m_sampleComb += (tmpVar + 0.501187f);
        }
        else
        {
            m_sampleComb += 0.501187f;
            tmpVar = m_sampleComb;

            if (m_sampleComb < -2.98815f)
            {
                m_sampleComb = -2.98815f;
            }

            m_sampleComb *= (1.f - fabs(m_sampleComb) * 0.167328f);

            m_sampleComb *= 0.7488f;
            tmpVar *= 0.2512f;

            m_sampleComb += (tmpVar - 0.501187f);
        }
    }


    //***************************** SmoothB ********************************//
    tmpVar  = m_delaySamples - m_delayStateVar;
    tmpVar *= m_delayConst;
    tmpVar += m_delayStateVar;

    m_delayStateVar = tmpVar;

    tmpVar *= _signal[CMB_FEC];
    tmpVar += (phaseMod * tmpVar);


    //******************************* Delay ********************************//
    float holdsample = m_sampleComb;            // for Bypass

    m_sampleComb *= m_flushFadePoint;
    m_delayBuffer[m_delayBufferInd] = m_sampleComb;

    /// hier kommt voicestealing hin!!

    tmpVar -= 1.f;
    if (tmpVar > COMB_BUFFER_SIZE_M3)
    {
        tmpVar = COMB_BUFFER_SIZE_M3;
    }
    else if (tmpVar < 1.f)
    {
        tmpVar = 1.f;
    }

    float delaySamples_int = NlToolbox::Conversion::float2int(tmpVar - 0.5f);               // integer and fraction speration
    float delaySamples_fract = tmpVar - delaySamples_int;

    int32_t ind_tm1 = delaySamples_int - 1;
    int32_t ind_t0  = delaySamples_int;
    int32_t ind_tp1 = delaySamples_int + 1;
    int32_t ind_tp2 = delaySamples_int + 2;

    ind_tm1 = m_delayBufferInd - ind_tm1;
    ind_t0  = m_delayBufferInd - ind_t0;
    ind_tp1 = m_delayBufferInd - ind_tp1;
    ind_tp2 = m_delayBufferInd - ind_tp2;

    ind_tm1 &= COMB_BUFFER_SIZE_M1;                             // Wrap with a mask sampleBuffer.size()-1
    ind_t0  &= COMB_BUFFER_SIZE_M1;
    ind_tp1 &= COMB_BUFFER_SIZE_M1;
    ind_tp2 &= COMB_BUFFER_SIZE_M1;

    m_sampleComb = NlToolbox::Math::interpolRT(delaySamples_fract,          // Interpolation
                                               m_delayBuffer[ind_tm1],
                                               m_delayBuffer[ind_t0],
                                               m_delayBuffer[ind_tp1],
                                               m_delayBuffer[ind_tp2]);

    m_sampleComb *= m_flushFadePoint;

    /// Envelope for voicestealingtmpVar


    m_delayBufferInd = (m_delayBufferInd + 1) & COMB_BUFFER_SIZE_M1;      // increase index and check boundaries

    tmpVar = _signal[CMB_BYP];                                            // Bypass
    m_sampleComb = tmpVar * holdsample + (1.f - tmpVar) * m_sampleComb;

    //****************************** Decay ********************************//
    m_decayStateVar = m_sampleComb * m_decayGain;
}



/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_combfilter::setCombfilter(float *_signal, float _samplerate)
{
    //********************** Highpass Coefficients *************************//
    float frequency = _signal[CMB_FRQ];
    frequency *= 0.125f;

    if (frequency < m_freqClip_24576)
    {
        frequency = m_freqClip_24576;
    }
    if (frequency > m_freqClip_2)
    {
        frequency = m_freqClip_2;
    }

    frequency *= m_warpConst_PI;
    frequency = NlToolbox::Math::tan(frequency);

    m_hpCoeff_a1 = (1.f - frequency) / (1.f + frequency);
    m_hpCoeff_b0 = 1.f / (1.f + frequency);
    m_hpCoeff_b1 = m_hpCoeff_b0 * -1.f;


    //*********************** Lowpass Coefficient **************************//
    frequency = _signal[CMB_LPF];

    if (frequency < m_freqClip_24576)
    {
        frequency = m_freqClip_24576;
    }
    if (frequency > m_freqClip_4)
    {
        frequency = m_freqClip_4;
    }

    frequency *= m_warpConst_PI;

    frequency *= 0.159155f;                                        // 2Pi wrap
    frequency -= NlToolbox::Conversion::float2int(frequency);
    frequency *= 6.28319f;

    frequency = NlToolbox::Math::sin(frequency) / NlToolbox::Math::cos(frequency);     // tan -pi..pi

    m_lpCoeff = (1.f - frequency) / (1.f + frequency);


    //********************** Allpass Coefficients **************************//
    frequency = _signal[CMB_APF];

    if (frequency < m_freqClip_24576)
    {
        frequency = m_freqClip_24576;
    }
    if (frequency > m_freqClip_2)
    {
        frequency = m_freqClip_2;
    }

    frequency *= m_warpConst_2PI;

    float resonance = _signal[CMB_APR] * 1.99f - 1.f;
    resonance = NlToolbox::Math::sin(frequency) * (1.f - resonance);

    float tmpVar = 1.f / (1.f + resonance);

    m_apCoeff_1 = (-2.f * NlToolbox::Math::cos(frequency)) * tmpVar;
    m_apCoeff_2 = (1.f - resonance) * tmpVar;


    //*************************** Delaytime ********************************//
    frequency = _signal[CMB_FRQ];

    if (frequency < m_delayFreqClip)
    {
        m_delaySamples = _samplerate / m_delayFreqClip;
    }
    else
    {
        m_delaySamples = _samplerate / frequency;
    }


    //************************ Lowpass Influence ***************************//
    frequency *= m_sampleInterval;

    float stateVar_r = NlToolbox::Math::sinP3_wrap(frequency);
    float stateVar_i = NlToolbox::Math::sinP3_wrap(frequency + 0.25f);

    stateVar_r = stateVar_r * m_lpCoeff;
    stateVar_i = stateVar_i * -m_lpCoeff + 1.f;

    tmpVar = NlToolbox::Math::arctan(stateVar_r / stateVar_i) * -0.159155f;       // (1.f / -6.28318f)

    m_delaySamples = m_delaySamples * tmpVar + m_delaySamples;


    //************************ Allpass Influence ***************************//
    stateVar_i = NlToolbox::Math::sinP3_wrap(frequency) * -1.f * m_apCoeff_1;
    stateVar_r = NlToolbox::Math::sinP3_wrap(frequency + 0.25f) * m_apCoeff_1;

    float stateVar2_i = NlToolbox::Math::sinP3_wrap(frequency + frequency);
    float stateVar2_r = NlToolbox::Math::sinP3_wrap(frequency + frequency + 0.25f);


    float var1_i = stateVar_i - stateVar2_i;
    float var2_i = (stateVar_i - (m_apCoeff_2 * stateVar2_i)) * -1.f;
    float var1_r = stateVar_r + stateVar2_r + m_apCoeff_2;
    float var2_r = stateVar_r + (stateVar2_r * m_apCoeff_2) + 1.f;

    stateVar_r = (var1_r * var2_r) - (var1_i * var2_i);
    stateVar_i = (var1_r * var2_i) + (var2_r * var1_i);

    if (stateVar_r > 0.f)                                            // safe
    {
        stateVar_r += 1e-12;
    }
    else
    {
        stateVar_r -= 1e-12;
    }

    tmpVar = NlToolbox::Math::arctan(stateVar_i / stateVar_r);        // arctan

    if (stateVar_r < 0.f)
    {
        if (stateVar_i > 0.f)
        {
            tmpVar += 3.14159f;
        }
        else
        {
            tmpVar -= 3.14159f;
        }
    }

    if (tmpVar > 0.f)                                       // forced unwrap > 0
    {
        tmpVar -= 6.28318f;
    }

    tmpVar *= 0.159155f;

    m_delaySamples = m_delaySamples * tmpVar + m_delaySamples;


    //**************************** Decay Gain ******************************//
    tmpVar = _signal[CMB_DEC];
    frequency = _signal[CMB_FRQ] * fabs(tmpVar);

    if (frequency < DNC_CONST)         // Min-Clip
    {
        frequency = DNC_CONST;
    }

    frequency = (1.f / frequency) * -6.28318f;

    if (frequency > 0)                 // Exp Clipped
    {
        frequency = 1.f;
    }
    else if (-27.631f > frequency)
    {
        frequency = 0.f;
    }
    else
    {
        frequency = pow(2.71828f, frequency);
    }

    if (tmpVar < 0.f)
    {
        frequency *= -1.f;
    }

    m_decayGain = frequency;
}


