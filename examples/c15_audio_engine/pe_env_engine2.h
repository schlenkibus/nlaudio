/******************************************************************************/
/** @file           pe_env_engine2.h
    @date           2018-06-25
    @version        0.1
    @author         Matthias Seeber
    @brief          Envelope Generator
                    (rendering mono and poly envelope signals according to
                    current states of involved parameters)
                    ADBDSR Envelopes now come in two flavors:
                    - Split (Magnitude/Timbre): Env A, B (new Split Parameter)
                    - Retrigger: Env C (new RetriggerHardness Parameter)
                    - Gate and Flanger Envelope functionalities remain as they are
                    (Envelope C may evolve to split and retrigger in the future,
                    possibly affecting the Output Mixer individual levels)
    @todo
*******************************************************************************/

#pragma once

#include <stdint.h>
#include "nltoolbox.h"
#include "pe_defines_config.h"

/**************************************             **************************************/

/* */
struct env_segment_single
{
    /* */
    uint32_t m_state = 0;
    uint32_t m_next = 0;
    /* */
    float m_dx[dsp_number_of_voices] = {};
    float m_dest_magnitude[dsp_number_of_voices] = {};
    float m_curve = 0.f;
};

/* */
struct env_segment_split : env_segment_single
{
    /* */
    float m_dest_timbre[dsp_number_of_voices] = {};
};

/**************************************             **************************************/

/* */
struct env_body_single
{
    /* */
    uint32_t m_state = 0;
    uint32_t m_next = 0;
    uint32_t m_index = 0;
    /* */
    float m_x = 0.f;
    float m_y = 0.f;
    float m_start_magnitude = 0.f;
    float m_signal_magnitude = 0.f;
};

/* */
struct env_body_split : env_body_single
{
    /* */
    float m_start_timbre = 0.f;
    float m_signal_timbre = 0.f;
};

/**************************************             **************************************/

/* */
struct env_object_adbdsr_split
{
    /* */
    const uint32_t m_startIndex = 1;
    const uint32_t m_stopIndex = 4;
    float m_split = 0.f;
    /* */
    env_body_split m_body[dsp_number_of_voices];
    env_segment_split m_segment[5];
    /* */
    void start(const uint32_t _voiceId);
    void stop(const uint32_t _voiceId);
    void tick(const uint32_t _voiceId);
    void nextSegment(const uint32_t _voiceId);
    void setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value);
    void setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const float _magnitude, const float _timbre);
};

/* */
struct env_object_adbdsr_retrig
{
    /* */
    const uint32_t m_startIndex = 1;
    const uint32_t m_stopIndex = 4;
    float m_retriggerHardness = 0.f;
    /* */
    env_body_single m_body[dsp_number_of_voices];
    env_segment_single m_segment[5];
    /* */
    void start(const uint32_t _voiceId);
    void stop(const uint32_t _voiceId);
    void tick(const uint32_t _voiceId);
    void nextSegment(const uint32_t _voiceId);
    void setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value);
    void setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const float _magnitude);
};

/* */
struct env_object_gate
{
    /* */
    const uint32_t m_startIndex = 1;
    const uint32_t m_stopIndex = 2;
    /* */
    env_body_single m_body[dsp_number_of_voices];
    env_segment_single m_segment[3];
    /* */
    void start(const uint32_t _voiceId);
    void stop(const uint32_t _voiceId);
    void tick(const uint32_t _voiceId);
    void nextSegment(const uint32_t _voiceId);
    void setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value);
    void setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const float _magnitude);
};

/* */
struct env_object_decay
{
    /* */
    const uint32_t m_startIndex = 1;
    /* */
    env_body_single m_body[1];
    env_segment_single m_segment[3];
    /* */
    void start(const uint32_t _voiceId);
    void tick(const uint32_t _voiceId);
    void nextSegment(const uint32_t _voiceId);
    void setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value);
    void setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const float _magnitude);
};

/**************************************             **************************************/

/* */
struct env_engine2
{
    /* */
    env_object_adbdsr_split m_env_a;                // Envelope A
    env_object_adbdsr_split m_env_b;                // Envelope B
    env_object_adbdsr_retrig m_env_c;               // Envelope C
    env_object_gate m_env_g;                        // Envelope G (Gate)
    env_object_decay m_env_f;                       // Envelope F (Flanger)
    /* */
    void init(const uint32_t _voices);              // proper initialization
    /* */
    void tickMono();                                // rendering function for monophonic Envelopes
    void tickPoly(const uint32_t _voiceId);         // rendering function for polyphonic Envelopes
};
