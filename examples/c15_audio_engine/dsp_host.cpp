#include <iostream>
#include "dsp_host.h"

/* default constructor - initialize (audio) signals */
dsp_host::dsp_host()
{
    /* init main audio output signal */
    m_mainOut_R = 0.f;
    m_mainOut_L = 0.f;
    /* init shared signal array */
    m_paramsignaldata[dsp_number_of_voices][sig_number_of_params] = {};
}

/* proper init - initialize engine(s) according to sampleRate and polyphony */
void dsp_host::init(uint32_t _samplerate, uint32_t _polyphony)
{
    /* set up configuration */
    m_samplerate = _samplerate;
    m_voices = _polyphony;
    m_clockDivision[2] = _samplerate / dsp_clock_rates[0];
    m_clockDivision[3] = _samplerate / dsp_clock_rates[1];
    m_upsampleFactor = _samplerate / 48000;
    /* initialize components */
    m_params.init(_samplerate, _polyphony);
    m_decoder.init();
    /* init messages to terminal */
    std::cout << "DSP_HOST::INIT(samplerate: " << m_samplerate << ", voices: " << m_voices << ")" << std::endl;
    std::cout << "DSP_HOST::CLOCK_divisions(" << m_clockDivision[0] << ", " << m_clockDivision[1] << ", ";
    std::cout << m_clockDivision[2] << ", " << m_clockDivision[3] << ")" << std::endl;
    std::cout << "DSP_HOST::upsampleFactor: " << m_upsampleFactor << std::endl;
}

/* */
void dsp_host::tickMain()
{
    /* provide indices for items, voices and parameters */
    uint32_t i, v, p;
    /* first: evaluate slow clock status */
    if(m_clockPosition[3] == 0)
    {
        /* render slow mono parameters */
        for(p = 0; p < m_params.m_clockIds.m_data[3].m_data[0].m_length; p++)
        {
            i = m_params.m_head[m_params.m_clockIds.m_data[3].m_data[0].m_data[p]].m_index;
            m_params.tickItem(i);
        }
        /* render slow poly parameters and perform poly slow post processing */
        for(v = 0; v < m_voices; v++)
        {
            for(p = 0; p < m_params.m_clockIds.m_data[3].m_data[1].m_length; p++)
            {
                i = m_params.m_head[m_params.m_clockIds.m_data[3].m_data[1].m_data[p]].m_index + v;
                m_params.tickItem(i);
            }
            m_params.postProcess_slow(m_paramsignaldata[v], v);
        }
    }
    /* second: evaluate fast clock status */
    if(m_clockPosition[2] == 0)
    {
        /* render fast mono parameters */
        for(p = 0; p < m_params.m_clockIds.m_data[2].m_data[0].m_length; p++)
        {
            i = m_params.m_head[m_params.m_clockIds.m_data[2].m_data[0].m_data[p]].m_index;
            m_params.tickItem(i);
        }
        /* render fast poly parameters and perform poly fast post processing */
        for(v = 0; v < m_voices; v++)
        {
            for(p = 0; p < m_params.m_clockIds.m_data[2].m_data[1].m_length; p++)
            {
                i = m_params.m_head[m_params.m_clockIds.m_data[2].m_data[1].m_data[p]].m_index + v;
                m_params.tickItem(i);
            }
            m_params.postProcess_fast(m_paramsignaldata[v], v);
        }
    }
    /* third: evaluate audio clock (always) - mono rendering, poly rendering and post processing */
    for(p = 0; p < m_params.m_clockIds.m_data[1].m_data[0].m_length; p++)
    {
        /* render mono audio parameters */
        i = m_params.m_head[m_params.m_clockIds.m_data[1].m_data[0].m_data[p]].m_index;
        m_params.tickItem(i);
    }
    for(v = 0; v < m_voices; v++)
    {
        /* this is the MAIN POLYPHONIC LOOP - rendering (and post processing) parameters, envelopes and the AUDIO_ENGINE */
        /* render poly audio parameters */
        for(p = 0; p < m_params.m_clockIds.m_data[1].m_data[1].m_length; p++)
        {
            i = m_params.m_head[m_params.m_clockIds.m_data[1].m_data[1].m_data[p]].m_index + v;
            m_params.tickItem(i);
        }
        /* post processing and envelope rendering */
        m_params.postProcess_audio(m_paramsignaldata[v], v);
        /* (TODO) AUDIO_ENGINE: poly dsp phase */
    }
    /* (TODO) AUDIO_ENGINE: mono dsp phase */
    /* finally: update (fast and slow) clock positions */
    m_clockPosition[2] = (m_clockPosition[2] + 1) % m_clockDivision[2];
    m_clockPosition[3] = (m_clockPosition[3] + 1) % m_clockDivision[3];
}

/* */
void dsp_host::evalMidi(uint32_t _status, uint32_t _data0, uint32_t _data1)
{
    /* TCD MIDI evaluation */
    float f;
    switch(m_decoder.getCommandId(_status & 127))
    {
    case 0:
        /* ignore */
        break;
    case 1:
        /* selectVoice */
        m_decoder.m_voiceFrom = m_decoder.m_voiceTo = m_decoder.unsigned14(_data0, _data1);
        voiceSelectionUpdate();
        break;
    case 2:
        /* selectMultipleVoices */
        m_decoder.m_voiceTo = m_decoder.unsigned14(_data0, _data1);
        voiceSelectionUpdate();
        break;
    case 3:
        /* selectParam */
        m_decoder.m_paramFrom = m_decoder.m_paramTo = m_decoder.unsigned14(_data0, _data1);
        paramSelectionUpdate();
        break;
    case 4:
        /* selectMultipleParams */
        m_decoder.m_paramTo = m_decoder.unsigned14(_data0, _data1);
        paramSelectionUpdate();
        break;
    case 5:
        /* setTime */
        f = 1.f / static_cast<float>((m_upsampleFactor * m_decoder.unsigned14(_data0, _data1)) + 1);
        timeUpdate(f);
        break;
    case 6:
        /* setTimeUpper */
        m_decoder.unsigned28upper(_data0, _data1);
        break;
    case 7:
        /* setTimeLower */
        f = 1.f / static_cast<float>((m_upsampleFactor * m_decoder.apply28lower(_data0, _data1)) + 1);
        timeUpdate(f);
        break;
    case 8:
        /* setDestination */
        f = static_cast<float>(m_decoder.unsigned14(_data0, _data1));
        /* distinguish list mode */
        if(m_decoder.m_listId == 0)
        {
            /* list off */
            destinationUpdate(f);
        }
        else
        {
            /* list on */
            listUpdate(f);
        }
        break;
    case 9:
        /* setSignedDestination */
        f = static_cast<float>(m_decoder.signed14(_data0, _data1));
        /* distinguish list mode */
        if(m_decoder.m_listId == 0)
        {
            /* list off */
            destinationUpdate(f);
        }
        else
        {
            /* list on */
            listUpdate(f);
        }
        break;
    case 10:
        /* setDestinationUpper */
        m_decoder.signed28upper(_data0, _data1);
        break;
    case 11:
        /* setDestinationLower */
        f = static_cast<float>(m_decoder.apply28lower(_data0, _data1));
        /* distinguish list mode */
        if(m_decoder.m_listId == 0)
        {
            /* list off */
            destinationUpdate(f);
        }
        else
        {
            /* list on */
            listUpdate(f);
        }
        break;
    case 12:
        /* preload */
        preloadUpdate(_data1, _data0);
        break;
    case 13:
        /* keyUp */
        f = static_cast<float>(m_decoder.unsigned14(_data0, _data1));
        keyUp(m_decoder.m_voiceFrom, f);
        break;
    case 14:
        /* keyDown */
        f = static_cast<float>(m_decoder.unsigned14(_data0, _data1));
        keyDown(m_decoder.m_voiceFrom, f);
        break;
    case 15:
        /* flush (audio_engine trigger needed) */
        break;
    case 16:
        /* selectUtility */
        m_decoder.m_utilityId = m_decoder.unsigned14(_data0, _data1);
        break;
    case 17:
        /* setUtility */
        f = static_cast<float>(m_decoder.unsigned14(_data0, _data1));
        utilityUpdate(f);
        break;
    }
}

/* */
void dsp_host::voiceSelectionUpdate()
{
    /* case ALL voices */
    if(m_decoder.m_voiceTo == 16383)
    {
        m_decoder.m_voiceFrom = 0;
        m_decoder.m_voiceTo = 16382;
    }
    /* reset voice id list */
    m_decoder.m_selectedVoices.reset();
    /* prepare selection event */
    m_decoder.m_event[0] = m_decoder.m_voiceFrom > m_decoder.m_voiceTo ? 1 : 0;
    /* rebuild voice id list by sorting selected and deselected voice ids */
    uint32_t s;
    for(uint32_t v = 0; v < m_voices; v++)
    {
        s = m_decoder.selectionEvent(m_decoder.m_voiceFrom, m_decoder.m_voiceTo, v);
        m_decoder.m_selectedVoices.add(s, v);
    }
}

/* */
void dsp_host::paramSelectionUpdate()
{
    /* case ALL params */
    if(m_decoder.m_paramTo == 16383)
    {
        m_decoder.m_paramFrom = 0;
        m_decoder.m_paramTo = 16382;
    }
    /* reset param id list */
    m_decoder.m_selectedParams.reset();
    /* prepare selection event */
    m_decoder.m_event[0] = m_decoder.m_paramFrom > m_decoder.m_paramTo ? 1 : 0;
    /* rebuild param id list by sorting selected and deselected param ids according to their polyphony */
    uint32_t s;
    for(uint32_t p = 0; p < sig_number_of_params; p++)
    {
        s = m_decoder.selectionEvent(m_decoder.m_paramFrom, m_decoder.m_paramTo, m_params.m_head[p].m_id);
        m_decoder.m_selectedParams.add(m_params.m_head[p].m_polyType, s, p);
    }
}

/* */
void dsp_host::preloadUpdate(uint32_t _mode, uint32_t _listId)
{
    /* provide voice and parameter indices */
    uint32_t v, p;
    /* handle preload according to mode */
    switch(_mode)
    {
    case 0:
        /* disable preload and reset listId */
        m_params.m_preload = 0;
        m_decoder.m_listId = 0;
        break;
    case 1:
        /* enable preload - set preload mode and list id, reset list index */
        m_params.m_preload = 1;
        m_decoder.m_listId = _listId;
        m_decoder.m_listIndex = 0;
        /* reset parameter preload counters */
        for(p = 0; p < sig_number_of_param_items; p++)
        {
            m_params.m_body[p].m_preload = 0;
        }
        /* reset key event preload counters */
        m_params.m_event.m_mono.m_preload = 0;
        for(v = 0; v < m_voices; v++)
        {
            m_params.m_event.m_poly[v].m_preload = 0;
        }
        break;
    case 2:
        /* apply preloaded values - reset preload mode and list id */
        m_params.m_preload = 0;
        m_decoder.m_listId = 0;
        /* apply preloaded values - parameters */
        for(p = 0; p <sig_number_of_params; p++)
        {
            for(v = 0; v < m_params.m_head[p].m_size; v++)
            {
                m_params.applyPreloaded(v, p);
            }
        }
        /* apply preloaded values - key events */
        if(m_params.m_event.m_mono.m_preload > 0)
        {
            m_params.m_event.m_mono.m_preload = 0;
            m_params.keyApplyMono();
        }
        for(v = 0; v < m_voices; v++)
        {
            if(m_params.m_event.m_poly[v].m_preload > 0)
            {
                m_params.m_event.m_poly[v].m_preload = 0;
                m_params.keyApply(v);
                keyApply(v);
            }
        }
        break;
    }
}

/* */
void dsp_host::destinationUpdate(float _value)
{
    /* provide voice and parameter indices */
    uint32_t v, p;
    /* update (selected) mono parameters */
    for(p = 0; p < m_decoder.m_selectedParams.m_data[0].m_data[1].m_length; p++)
    {
        m_params.setDest(0, m_decoder.m_selectedParams.m_data[0].m_data[1].m_data[p], _value);
    }
    /* update (selected) poly parameters - for selected voices */
    for(v = 0; v < m_decoder.m_selectedVoices.m_data[1].m_length; v++)
    {
        for(p = 0; p < m_decoder.m_selectedParams.m_data[1].m_data[1].m_length; p++)
        {
            m_params.setDest(v, m_decoder.m_selectedParams.m_data[1].m_data[1].m_data[p], _value);
        }
    }
}

/* */
void dsp_host::timeUpdate(float _value)
{
    /* provide voice and parameter indices */
    uint32_t v, p;
    /* update (selected) mono parameters */
    for(p = 0; p < m_decoder.m_selectedParams.m_data[0].m_data[1].m_length; p++)
    {
        m_params.setDx(0, m_decoder.m_selectedParams.m_data[0].m_data[1].m_data[p], _value);
    }
    /* update (selected) poly parameters - for selected voices */
    for(v = 0; v < m_decoder.m_selectedVoices.m_data[1].m_length; v++)
    {
        for(p = 0; p < m_decoder.m_selectedParams.m_data[1].m_data[1].m_length; p++)
        {
            m_params.setDx(v, m_decoder.m_selectedParams.m_data[1].m_data[1].m_data[p], _value);
        }
    }
}

/* */
void dsp_host::utilityUpdate(float _value)
{
    /* utility parameters are handled individually and can trigger further events */
    switch(m_decoder.m_utilityId)
    {
    case 0:
        /* ignore - utility parameter 0 serves only as velocity parameter definition */
        break;
    case 1:
        /* set reference tone */
        _value *= m_params.m_utilities[1].m_normalize;
        m_params.m_pitch_reference = m_params.scale(m_params.m_utilities[1].m_scaleId, m_params.m_utilities[1].m_scaleArg, _value);
        /* update pitch related values? */
    }
}

/* */
void dsp_host::listUpdate(float _dest)
{
    /* distinguish list ids */
    switch(m_decoder.m_listId)
    {
    case 1:
        /* recall list traversal */
        m_params.setDest(0, m_decoder.traverseRecall(), _dest);
        break;
    case 2:
        /* key event traversal (polyphonic - voice selection up to sender! */
        m_params.setDest(m_decoder.m_voiceFrom, m_decoder.traverseKeyEvent(), _dest);
        break;
    }
}

/* */
void dsp_host::keyUp(uint32_t _voiceId, float _velocity)
{
    /* update values */
    m_params.keyUp(_voiceId, _velocity);
    /* handle preload */
    if(m_params.m_preload == 0)
    {
        /* direct key apply */
        m_params.keyApplyMono();
        m_params.keyApply(_voiceId);
        keyApply(_voiceId);
    }
    else
    {
        /* preload key event (increase preload counters) */
        m_params.m_event.m_mono.m_preload++;
        m_params.m_event.m_poly[_voiceId].m_preload++;
    }
}

/* */
void dsp_host::keyDown(uint32_t _voiceId, float _velocity)
{
    /* update values */
    m_params.keyDown(_voiceId, _velocity);
    /* handle preload */
    if(m_params.m_preload == 0)
    {
        /* direct key apply */
        m_params.keyApplyMono();
        m_params.keyApply(_voiceId);
        keyApply(_voiceId);
    }
    else
    {
        /* preload key event (increase preload counters) */
        m_params.m_event.m_mono.m_preload++;
        m_params.m_event.m_poly[_voiceId].m_preload++;
    }
}

/* */
void dsp_host::keyApply(uint32_t _voiceId)
{
    /* keyDown events cause triggers to the AUDIO_ENGINE */
    if(m_params.m_event.m_poly[_voiceId].m_type == 1)
    {
        /* determine note steal */
        if(m_params.m_body[m_params.m_head[par_noteSteal].m_index].m_signal == 1)
        {
            /* AUDIO_ENGINE: trigger voice-steal */
        }
        else
        {
            /* AUDIO_ENGINE: trigger non-voice-steal */
        }
        /* update and reset oscillator phases */
        m_paramsignaldata[_voiceId][7] = m_params.m_body[m_params.m_head[24].m_index + _voiceId].m_signal;  // POLY PHASE_A -> OSC_A Phase
        /* AUDIO_ENGINE: reset oscillator phases */
    }
}
