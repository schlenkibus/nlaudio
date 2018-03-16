/******************************************************************************/
/** @file       pe_defines_config.h
    @date       2018-03-13
    @version    1.0
    @author     Matthias Seeber
    @brief      crucial definitions of the param engine architecture
                ..
    @todo
*******************************************************************************/

#pragma once

#include <stdint.h>

/* main project config */

#define dsp_poly_types 2                    // two polyphony types (mono, poly)
#define dsp_clock_types 4                   // four different parameter types (sync, audio, fast, slow)
#define dsp_number_of_voices 20             // maximum of allowed voices

const uint32_t dsp_clock_divisions[2][2] = {
    {5, 120},                               // (sample) clock divisions for fast, slow subclocks (Samplerate = 48000 Hz)
    {10, 240}                               // (sample) clock divisions for fast, slow subclocks (Samplerate = 96000 Hz) (mode: constant rate, variable divisions)
};

const uint32_t dsp_upsample_factor[2] = {1, 2};

#define dsp_render_min 1e-9

/* project parameter definition (prepared for 20 voices) */

#define sig_number_of_params 26             // 13 ENV_A params, 6 OSC params, 2 MASTER params, 5 POLY params (no EnvC Rate)
#define sig_number_of_param_items 121       // (13 + 6 + 2) MONO params = 21 param itmems; (5 x 20) POLY params = 100 param items; total: 21 + 100 = 121 items
#define sig_number_of_signal_items 10       // small test project requires 10 shared signals
#define sig_number_of_utilities 2
#define sig_number_of_envelopes 5
#define sig_number_of_env_items 81          // 4 POLY Envelopes (A..Gate) = 4 * 20 = 80 items, 1 MONO (Flanger Decay), total: 81 items
#define sig_number_of_env_segments 4
#define sig_number_of_env_types 3
#define sig_number_of_env_events 3

/* dsp helper values */

#define dsp_samples_to_ms 1e-3
#define dsp_init_pitch_reference 440
#define dsp_expon_osc_pitch_from -20
#define dsp_expon_osc_pitch_range 150
#define dsp_expon_lin_pitch_from -150
#define dsp_expon_lin_pitch_range 300
#define dsp_expon_level_from -300
#define dsp_expon_level_range 400
#define dsp_expon_time_from -20
#define dsp_expon_time_range 110
#define dsp_expon_time_factor 104.0781

#define env_norm_peak 0.023766461           // equals 1 / 42.0761 (taken from prototype)
#define env_clip_peak 1.412537545           // equals +3 dB (candidate levelKT clipping)

/* tcd list handling */

#define lst_number_of_lists 2               // predefined paramId lists (simplifying recal and key event update TCD sequences)
#define lst_recall_length 21                // declare number of paramIds for recall list (MONO - currently)
#define lst_keyEvent_length 5               // declare number of paramIds for key event list (POLY - currently)
