#include <iostream>
#include "dsp_host.h"

/* default constructor - initialize (audio) signals */
dsp_host::dsp_host()
{
    /* init main audio output signal */
    m_mainOut_R = 0.f;
    m_mainOut_L = 0.f;
    /* init shared signal array */
    m_paramsignaldata[dsp_number_of_voices][sig_number_of_signal_items] = {};
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

    /* Audio Engine */
    initAudioEngine(static_cast<float>(_samplerate), _polyphony);
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

            /*Trigger forF ilter Coefficients -> the function is made*/
            setFilterCoefficients(m_paramsignaldata[v], v);
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

    m_mainOut_L = 0.f;
    m_mainOut_R = 0.f;

    /*set the current fadepoint*/
    float currFadeVal = m_raised_cos_table[m_tableCounter];

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

        /* AUDIO_ENGINE: poly dsp phase */
        makePolySound(m_paramsignaldata[v], v);
    }

    /* AUDIO_ENGINE: mono dsp phase */
    makeMonoSound(m_paramsignaldata[0]);

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
        m_flushnow = true;
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
        resetOscPhase(m_paramsignaldata[_voiceId], _voiceId);
    }
}

/* End of Main Definition, Test functionality below:
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
 */

/* testing the engine - midi input */
void dsp_host::testMidi(uint32_t _status, uint32_t _data0, uint32_t _data1)
{
    /* */
    uint32_t chan = _status & 15;
    uint32_t type = (_status & 127) >> 4;
    /* */
    std::cout << "MIDI IN (chan: " << chan << ", type: ";
    /* */
    switch(type)
    {
    case 0:
        /* NOTE OFF (explicit) */
        std::cout << "NOTE_OFF, pitch: " << _data0 << ", velocity: " << _data1;
        testNoteOff(_data0, _data1);
        break;
    case 1:
        /* NOTE ON (if velocity > 0) */
        if(_data1 > 0)
        {
            std::cout << "NOTE_ON, pitch: " << _data0 << ", velocity: " << _data1;
            testNoteOn(_data0, _data1);
        }
        else
        {
            std::cout << "NOTE_OFF, pitch: " << _data0 << ", velocity: " << _data1;
            testNoteOff(_data0, _data1);
        }
        break;
    case 2:
        /* POLY AFTERTOUCH */
        std::cout << "POLY_AT, pitch: " << _data0 << ", value: " << _data1;
        break;
    case 3:
        /* CONTROL CHANGE */
        std::cout << "CONTROL_CHANGE, ctrl: " << _data0 << ", value: " << _data1;
        testRouteControls(_data0, _data1);
        break;
    case 4:
        /* PROGRAM CHANGE */
        std::cout << "PROGRAM_CHANGE, nr: " << _data0;
        break;
    case 5:
        /* MONO AFTERTOUCH */
        std::cout << "MONO_AT, value: " << _data0;
        break;
    case 6:
        /* PITCH BEND */
        std::cout << "PITCH_BEND, value: " << _data0 << ", " << _data1;
        break;
    }
    std::cout << ")" << std::endl;
}

/* test key down */
void dsp_host::testNoteOn(uint32_t _pitch, uint32_t _velocity)
{
    /* get current voiceId and trigger list sequence for key event */
    m_test_noteId[_pitch] = m_test_voiceId + 1;             // (plus one in order to distinguish from zero)
    /* prepare pitch and velocity */
    int32_t notePitch = (_pitch - 60) * 1000;
    uint32_t noteVel = static_cast<uint32_t>(static_cast<float>(_velocity) * m_test_normalizeMidi * utility_definition[0][0]);
    /* key event sequence */
    evalMidi(47, 2, 1);                                     // enable preload (key event list mode)
    evalMidi(0, 0, m_test_voiceId);                         // select voice: current
    evalMidi(5, 0, 0);                                      // voice steal: 0
    testParseDestination(notePitch);                        // note pitch
    evalMidi(5, 0, 0);                                      // voice pan: 0
    evalMidi(5, 0, 0);                                      // phase A: 0
    evalMidi(5, 0, 0);                                      // phase B: 0
    evalMidi(23, noteVel >> 7, noteVel & 127);              // key down: velocity
    evalMidi(47, 0, 2);                                     // apply preloaded values
    /* take current voiceId and increase it (wrapped around polyphony) - sloppy approach */
    m_test_voiceId = (m_test_voiceId + 1) % m_voices;
}

/* test key up */
void dsp_host::testNoteOff(uint32_t _pitch, uint32_t _velocity)
{
    /* get note's voiceId and prepare velocity */
    uint32_t usedVoiceId = m_test_noteId[_pitch] - 1;       // (subtract one in order to get real id)
    m_test_noteId[_pitch] = 0;                              // clear voiceId assignment
    uint32_t noteVel = static_cast<uint32_t>(static_cast<float>(_velocity) * m_test_normalizeMidi * utility_definition[0][0]);
    /* key event sequence */
    evalMidi(47, 0, 1);                                     // enable preload (no list mode)
    evalMidi(0, 0, usedVoiceId);                            // select voice: used voice (by note number)
    evalMidi(7, noteVel >> 7, noteVel & 127);               // key up: velocity
    evalMidi(47, 0, 2);                                     // apply preloaded values
}

/* midi ctrl routing */
void dsp_host::testRouteControls(uint32_t _id, uint32_t _value)
{
    /* based on the controller layout, transform control changes into tcd messages */
    switch(testMidiMapping[testMidiDevice][_id])
    {
    case 0:
        /* ignore */
        break;
    case 1:
        /* set param 0 - env a attack */
        testEditParameter(0, _value);
        break;
    case 2:
        /* set param 1 - env a decay1 */
        testEditParameter(1, _value);
        break;
    case 3:
        /* set param 2 - env a breakpoit */
        testEditParameter(2, _value);
        break;
    case 4:
        /* set param 3 - env a decay2 */
        testEditParameter(3, _value);
        break;
    case 5:
        /* set param 4 - env a sustain */
        testEditParameter(4, _value);
        break;
    case 6:
        /* set param 5 - env a release */
        testEditParameter(5, _value);
        break;
    case 7:
        /* set param 6 - env a gain */
        testEditParameter(6, 24 * ((2 * _value) - 127));    // bipolar
        break;
    case 8:
        /* set param 7 - env a level velocity */
        testEditParameter(7, 60 * _value);
        break;
    case 9:
        /* set param 8 - env a attack velocity */
        testEditParameter(8, 60 * _value);
        break;
    case 10:
        /* set param 9 - env a release velocity */
        testEditParameter(9, 60 * _value);
        break;
    case 11:
        /* set param 10 - env a level keytrack */
        testEditParameter(10, ((2 * _value) - 127));        // bipolar
        break;
    case 12:
        /* set param 11 - env a time keytrack */
        testEditParameter(11, _value);
        break;
    case 13:
        /* set param 12 - env a attack curve */
        testEditParameter(12, ((2 * _value) - 127));        // bipolar
        break;
    case 14:
        /* set param 13 - osc a pitch */
        testEditParameter(13, 150 * _value);
        break;
    case 15:
        /* set param 14 - osc a pitch keytrack */
        testEditParameter(14, _value);
        break;
    case 16:
        /* set param 15 - osc a fluctuation */
        testEditParameter(15, _value);
        break;
    case 17:
        /* set param 16 - osc a pm self */
        testEditParameter(16, ((2 * _value) - 127));        // bipolar
        break;
    case 18:
        /* set param 17 - osc a pm self env a */
        testEditParameter(17, _value);
        break;
    case 19:
        /* set param 18 - osc a chirp */
        testEditParameter(18, 60 * _value);
        break;
    case 20:
        /* set param 19 - master volume */
        testEditParameter(19, _value);
        break;
    case 21:
        /* set param 20 - master tune */
        testEditParameter(20, 48 * ((2 * _value) - 127));        // bipolar
        break;
    case 22:
        /* set global time */
        testSetGlobalTime(_value * 126);                    // approach 14 bit full time (16000)
        break;
    case 23:
        /* recall preset 0 */
        testLoadPreset(0);
        break;
    case 24:
        /* recall preset 1 */
        testLoadPreset(1);
        break;
    case 25:
        /* recall preset 2 */
        testLoadPreset(2);
        break;
    case 26:
        /* trigger flush */
        testFlush();
        break;
    case 27:
        /* print param head */
        testGetParamHeadData();
        break;
    case 28:
        /* print param body */
        testGetParamRenderData();
        break;
    case 29:
        /* print signal */
        testGetSignalData();
        break;
    case 30:
        /* set reference */
        testSetReference(80 * _value);
        break;
    case 31:
        /* init */
        testInit();
        break;
    case 32:
        /* infinity release */
        testEditParameter(5, 16160);
    }
}

/* edit a particular parameter */
void dsp_host::testEditParameter(uint32_t _id, int32_t _value)
{
    /* prepare id and value - currently, every value is unipolar */
    uint32_t tcdId = m_params.m_head[_id].m_id;
    int32_t tcdVal = static_cast<int32_t>(static_cast<float>(_value) * m_test_normalizeMidi * param_definition[_id][3]);
    std::cout << "\nSET_PARAM(" << tcdId << ", " << tcdVal << ")" << std::endl;
    /* select parameter, set destination */
    evalMidi(1, tcdId >> 7, tcdId & 127);                       // select param: id
    testParseDestination(tcdVal);                               // parse destination
}

/* set transition time */
void dsp_host::testSetGlobalTime(uint32_t _value)
{
    std::cout << "\nSET_TIME(" << _value << ")" << std::endl;
    /* select all voices, params and update time */
    _value *= static_cast<uint32_t>(m_params.m_millisecond);    // convert time accordingly in samples
    evalMidi(0, 127, 127);                                      // select all voices
    evalMidi(1, 127, 127);                                      // select all params
    if(_value < 16384)
    {
        /* T */
        evalMidi(2, _value >> 7, _value & 127);                 // set time
    }
    else
    {
        /* TU + TL */
        uint32_t upper = _value >> 14;
        _value &= 16383;
        evalMidi(34, upper >> 7, upper & 127);                  // set time upper
        evalMidi(50, _value >> 7, _value & 127);                // set time lower
    }
}

/* set reference frequency */
void dsp_host::testSetReference(uint32_t _value)
{
    /* prepare value */
    uint32_t val = static_cast<uint32_t>(static_cast<float>(_value) * m_test_normalizeMidi * utility_definition[1][0]);
    std::cout << "\nSET_REFERENCE(" << val << ")" << std::endl;
    /* select and update reference utility */
    evalMidi(8, 0, 1);                                          // select utility (reference tone)
    evalMidi(24, val >> 7, val & 127);                          // update utility
}

/* preset recall approach */
void dsp_host::testLoadPreset(uint32_t _presetId)
{
    std::cout << "\nRECALL(" << _presetId << ")" << std::endl;
    /* run a recall sequence based on the given preset id (predefined presets in pe_defines_testconfig.h) */
    /* recall sequence - no flush */
    evalMidi(47, 1, 1);                                         // enable preload (recall list mode)
    for(uint32_t p = 0; p < testRecallSequenceLength; p++)
    {
        /* traverse normalized recall array */
        testParseDestination(testPresetData[_presetId][p]);
    }
    evalMidi(47, 0, 2);                                         // apply preloaded values
}

/* trigger flush */
void dsp_host::testFlush()
{
    std::cout << "\nFLUSH" << std::endl;
    /* pass the trigger TCD message */
    evalMidi(39, 0, 0);                                         // flush
}

/* glance at current signals */
void dsp_host::testGetSignalData()
{
    /* print out the signal array to the terminal */
    std::cout << "\nPARAM_SIGNAL:" << std::endl;
    for(uint32_t p = 0; p < sig_number_of_signal_items; p++)
    {
        std::cout << p << " - ";
        for(uint32_t v = 0; v < m_voices; v++)
        {
            std::cout << m_paramsignaldata[v][p] << ", ";
        }
        std::cout << std::endl;
    }
}

/* glance at parameter definition */
void dsp_host::testGetParamHeadData()
{
    /* print out the parameter definitions to the terminal */
    std::cout << "\nPARAM_HEAD:" << std::endl;
    for(uint32_t p = 0; p < sig_number_of_params; p++)
    {
        param_head* obj = &m_params.m_head[p];
        std::cout << "id: " << obj->m_id << ", ";
        std::cout << "index: " << obj->m_index << ", ";
        std::cout << "size: " << obj->m_size << ", ";
        std::cout << "clock: " << obj->m_clockType << ", ";
        std::cout << "poly: " << obj->m_polyType << ", ";
        std::cout << "scaleId: " << obj->m_scaleId << ", ";
        std::cout << "postId: " << obj->m_postId << ", ";
        std::cout << "norm: " << obj->m_normalize << ", ";
        std::cout << "scaleArg: " << obj->m_scaleArg << std::endl;
    }
}

/* glance at parameter rendering status */
void dsp_host::testGetParamRenderData()
{
    /* print out the parameter rendering status to the terminal */
    std::cout << "\nPARAM_BODY:" << std::endl;
    for(uint32_t p = 0; p < sig_number_of_params; p++)
    {
        param_head* obj = &m_params.m_head[p];
        uint32_t index = obj->m_index;
        for(uint32_t i = 0; i < obj->m_size; i++)
        {
            param_body* item = &m_params.m_body[index];
            std::cout << "P(" << obj->m_id << ", " << i << "):\t";
            std::cout << "state: " << item->m_state << ",\tpreload: " << item->m_preload;
            std::cout << ",\tsignal: " << item->m_signal << ",\tdx:[" << item->m_dx[0] << ", " << item->m_dx[1] << "]";
            std::cout << ",\tx: " << item->m_x << ",\tstart: " << item->m_start;
            std::cout << ",\tdiff: " << item->m_diff << ",\tdest: " << item->m_dest << std::endl;
            index++;
        }
    }
}

/* prepare destinations */
void dsp_host::testParseDestination(int32_t _value)
{
    /* prepare value */
    int32_t val = abs(_value);
    uint32_t upper = val >> 14;
    /* determine fitting destination format */
    if(_value < -8191)
    {
        /* DU + DL (negative) */
        evalMidi(37, (upper >> 7) + 64, upper & 127);
        evalMidi(53, (val & 16383) >> 7, val & 127);
    }
    else if(_value < 0)
    {
        /* DS (negative) */
        evalMidi(21, (val >> 7) + 64, val & 127);
    }
    else if(_value < 16384)
    {
        /* D */
        evalMidi(5, _value >> 7, _value & 127);
    }
    else
    {
        /* DU + DL (positive) */
        evalMidi(37, upper >> 7, upper & 127);
        evalMidi(53, (val & 16383) >> 7, val & 127);
    }
}
void dsp_host::testInit()
{
    /* */
    std::cout << "\nINIT SEQUENCE" << std::endl;
    evalMidi(0, 127, 127);      // select all voices
    evalMidi(1, 127, 127);      // select all parameters
    evalMidi(2, 0, 0);          // set time to zero
    testLoadPreset(1);          // load default preset
}




/******************************************************************************/
/**
*******************************************************************************/

void dsp_host::initAudioEngine(float _samplerate, uint32_t _polyphony)
{
    //****************************** Fade n Flush ****************************//
    m_flushnow = false;
    m_tableCounter = 0;

    float fade_time = 0.003f;
    float sample_interval = 1.f / _samplerate;
    m_fadeSamples = fade_time * _samplerate * 2 + 1;
    m_flushIndex = fade_time * _samplerate;
    m_raised_cos_table.resize(m_fadeSamples);

    for (uint32_t ind = 0; ind < m_raised_cos_table.size(); ind++)
    {
        float x = 1.5708f * sample_interval * ind / fade_time;
        m_raised_cos_table[ind] = pow(cos(x), 2);
    }

    //******************************* DSP Module *****************************//
    for (uint32_t p = 0; p < _polyphony; p++)
    {
        m_soundgenerator[p].init(_samplerate, p);
    }
}



/******************************************************************************/
/**
*******************************************************************************/

void dsp_host::makePolySound(float *_signal, uint32_t _voiceID)
{
    //***************************** Soundgenerator ***************************//
    //************************* Oscillators n Shapers ************************//
    m_soundgenerator[_voiceID].generateSound(0.f, _signal);

    //****************************** Outputmixer *****************************//
    m_mainOut_L += (m_soundgenerator[_voiceID].m_sampleA);
    m_mainOut_R += (m_soundgenerator[_voiceID].m_sampleA);
}



/******************************************************************************/
/**
*******************************************************************************/

void dsp_host::makeMonoSound(float *_signal)
{
    //****************************** Fade n Flush ****************************//
    if (m_flushnow)
    {
        if (m_tableCounter == m_flushIndex)
        {
            printf("flushing all buffers ... \n'");
        }

        if (m_tableCounter > m_fadeSamples)
        {
            m_tableCounter = 0;
            m_flushnow = false;
        }
        else
        {
            m_tableCounter++;
        }
    }
    //****************************** Mono Modules ****************************//

    //******************************* Soft Clip ******************************//
    m_mainOut_L *= _signal[MST_VOL];            /// -> reverb output here!

    m_mainOut_L *= 0.1588f;
    if (m_mainOut_L > 0.25f)
    {
        m_mainOut_L = 0.25f;
    }
    if (m_mainOut_L < -0.25f)
    {
        m_mainOut_L = -0.25f;
    }

    m_mainOut_L += -0.25f;
    m_mainOut_L += m_mainOut_L;

    m_mainOut_L = 0.5f - fabs(m_mainOut_L);

    float sample_square = m_mainOut_L * m_mainOut_L;
    m_mainOut_L = m_mainOut_L * ((2.26548 * sample_square - 5.13274) * sample_square + 3.14159);


    m_mainOut_R *= _signal[MST_VOL];            /// -> reverb output here!
    m_mainOut_R *= 0.1588f;

    if (m_mainOut_R > 0.25f)
    {
        m_mainOut_R = 0.25f;
    }
    if (m_mainOut_R < -0.25f)
    {
        m_mainOut_R = -0.25f;
    }

    m_mainOut_R += -0.25f;
    m_mainOut_R += m_mainOut_R;

    m_mainOut_R = 0.5f - fabs(m_mainOut_R);

    sample_square = m_mainOut_R * m_mainOut_R;
    m_mainOut_R = m_mainOut_R * ((2.26548 * sample_square - 5.13274) * sample_square + 3.14159);
}



/******************************************************************************/
/**
*******************************************************************************/

inline void dsp_host::resetOscPhase(float *_signal, uint32_t _voiceID)
{
    m_soundgenerator[_voiceID].resetPhase(_signal[OSC_A_PHS], 0.f);
}



/******************************************************************************/
/**
*******************************************************************************/

inline void dsp_host::setFilterCoefficients(float *_signal, uint32_t _voiceID)
{
    //************************ Osciallator Chirp Filter **********************//
    m_soundgenerator[_voiceID].m_chirpFilter_A.setCoeffs(_signal[OSC_A_CHI]);
    m_soundgenerator[_voiceID].m_chirpFilter_B.setCoeffs(0.f);                  /// _signal[OSC_B_CHI]
}

