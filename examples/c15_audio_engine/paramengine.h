/******************************************************************************/
/** @file		paramengine.h
    @date		2018-03-08
    @version    0.1
    @author		Matthias Seeber & Anton Schmied[2018-01-23]
    @brief		an implementation of the Parameter Renderer
                and TDC Interpreter

    @todo
*******************************************************************************/

#pragma once

#include <stdint.h>
#include "pe_defines_params.h"
#include "pe_exponentiator.h"

#if dsp_take_envelope == 0
#include "pe_env_engine.h"
#elif dsp_take_envelope == 1
#include "pe_env_engine2.h"
#endif

#include "pe_key_event.h"
#include "pe_utilities.h"
#include "pe_defines_labels.h"
#include "dsp_defines_signallabels.h"
#include "nltoolbox.h"

/* */
struct param_head
{
    /* */
    int32_t m_id;
    uint32_t m_index;
    uint32_t m_size;
    uint32_t m_clockType;
    uint32_t m_polyType;
    uint32_t m_scaleId;
    int32_t m_postId;
    float m_normalize;
    float m_scaleArg;
};

/* */
struct param_body
{
    /* */
    uint32_t m_state = 0;
    uint32_t m_preload = 0;
    float m_signal = 0.f;
    float m_dx[2] = {0.f, 0.f};
    float m_x = 0.f;
    float m_start = 0.f;
    float m_diff = 0.f;
    float m_dest = 0.f;
};

/* */
struct param_utility
{
    /* */
    uint32_t m_scaleId;
    float m_normalize;
    float m_scaleArg;
};

/* */
struct paramengine
{
    /* local variables */
    uint32_t m_preload = 0;
    /* pitch reference (A3 = 440.0 Hz) */
    float m_pitch_reference = dsp_init_pitch_reference;
    /* samplerate dependant parameters */
    float m_nyquist_frequency;
    float m_millisecond;
    /* clock and polyphony definitions */
    float m_timeFactors[dsp_clock_types];
    uint32_t m_routePolyphony[dsp_poly_types] = {1, dsp_number_of_voices};
    /* access to envelope parameters */
    const uint32_t m_envIds[sig_number_of_env_events] = {par_envelopeA, par_envelopeB, par_envelopeC};
    /* local data structures */
    clock_id_list m_clockIds;
    dual_clock_id_list m_postIds;
    param_head m_head[sig_number_of_params];
    param_body m_body[sig_number_of_param_items];
    exponentiator m_convert;
    param_utility m_utilities[sig_number_of_utilities];
#if dsp_take_envelope == 0
    env_engine m_envelopes;
#elif dsp_take_envelope == 1
    env_engine2 m_new_envelopes;
#endif
    poly_key_event m_event;
    NlToolbox::Curves::Shaper_1_BP m_combDecayCurve;
    NlToolbox::Curves::Shaper_1_BP m_svfLBH1Curve;
    NlToolbox::Curves::Shaper_1_BP m_svfLBH2Curve;
    const float m_svfResFactor = 1.f / 60.f;
    NlToolbox::Curves::Shaper_2_BP m_svfResonanceCurve;
    /* proper init */
    void init(uint32_t _sampleRate, uint32_t _voices);
    /* helper */
    float evalNyquist(float _freq);
    /* TCD mechanism */
    float scale(const uint32_t _scaleId, const float _scaleArg, float _value);              // provided tcd scale functions
    void setDx(const uint32_t _voiceId, const uint32_t _paramId, float _value);             // param dx update
    void setDest(const uint32_t _voiceId, const uint32_t _paramId, float _value);           // param dest update
    void applyPreloaded(const uint32_t _voiceId, const uint32_t _paramId);                  // param apply preloaded
    void applyDest(const uint32_t _index);                                                  // param apply dest (non-sync types)
    void applySync(const uint32_t _index);                                                  // param apply dest (sync types)
    /* rendering */
    void tickItem(const uint32_t _index);                                                   // parameter rendering
    /* key events */
    void keyDown(const uint32_t _voiceId, float _velocity);                                 // key events: key down (note on) mechanism
    void keyUp(const uint32_t _voiceId, float _velocity);                                   // key events: key up (note off) mechanism
    void keyApply(const uint32_t _voiceId);                                                 // key events: apply key event
    void keyApplyMono();                                                                    // key events: apply mono event
#if dsp_take_envelope == 0
    /* OLD envelope updates */
    void envUpdateStart(const uint32_t _voiceId, const uint32_t _envId, const float _pitch, const float _velocity, const float _retriggerHardness);
    void envUpdateStop(const uint32_t _voiceId, const uint32_t _envId, const float _pitch, const float _velocity);
    void envUpdateTimes(const uint32_t _voiceId, const uint32_t _envId);
    void envUpdateLevels(const uint32_t _voiceId, const uint32_t _envId);
#elif dsp_take_envelope == 1
    /* NEW envelopes updates */
    void newEnvUpdateStart(const uint32_t _voiceId, const float _pitch, const float _velocity);
    void newEnvUpdateStop(const uint32_t _voiceId, const float _pitch, const float _velocity);
    void newEnvUpdateTimes(const uint32_t _voiceId);
    void newEnvUpdateLevels(const uint32_t _voiceId);
#endif
    /* simplified polyphonic post processing approach (one function per clock) */
    void postProcessPoly_slow(float *_signal, const uint32_t _voiceId);                         // poly slow post processing (distribution, copy, env c event signal!)
    void postProcessPoly_fast(float *_signal, const uint32_t _voiceId);                         // poly fast post processing (distribution, copy)
    void postProcessPoly_audio(float *_signal, const uint32_t _voiceId);                        // poly audio post processing (distribution, copy, envelopes, param combination)
    void postProcessMono_slow(float *_signal);                                                  // mono slow post processing
    void postProcessMono_fast(float *_signal);                                                  // mono fast post processing
    void postProcessMono_audio(float *_signal);                                                 // mono audio post processing
};
