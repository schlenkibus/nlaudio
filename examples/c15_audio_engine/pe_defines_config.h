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

#include "dsp_defines_session.h"
#include <stdint.h>

/* main project config */

#define dsp_poly_types 2                    // two polyphony types (mono, poly) - (later, a dual type needs to be implemented)
#define dsp_clock_types 4                   // four different parameter types (sync, audio, fast, slow)
#define dsp_number_of_voices 20             // maximum allowed number of voices

const uint32_t dsp_clock_rates[2] = {       // sub-audio clocks are defined in rates (Hz) now
    9600,                                   // fast rate is 9600 Hz
    400                                     // slow rate is 400 Hz
};

#define dsp_render_min 1e-9                 // minimal rendered value for exponential transitions

/* project parameter definition (prepared for 20 voices) */
#if DSP_TEST_MODE==1

// configuration for test 1
#define sig_number_of_params 26             // 13 ENV_A params, 6 OSC params, 2 MASTER params, 5 POLY params (no EnvC Rate)
#define sig_number_of_param_items 121       // (13 + 6 + 2) MONO params = 21 param itmems; (5 x 20) POLY params = 100 param items; total: 21 + 100 = 121 items
#define sig_number_of_signal_items 10       // small test project requires 10 shared signals
#define lst_recall_length 21                // declare number of paramIds for recall list (MONO - currently)
#define lst_keyEvent_length 5               // declare number of paramIds for key event list (POLY - currently)

#elif DSP_TEST_MODE==2

// configuration for test 2                 -> see Linux Engine - Test 2
#define sig_number_of_params 95             // 3 * (13 ENV params) + 2 * (14 OSC + 8 SHP params) + (4 OUT params) + (2 MASTER params) + (6 KEY params)
#define sig_number_of_param_items 209       // (39 + 44 + 4 + 2 (* 1) MONO params) + (6 (* 20) POLY params)
#define sig_number_of_signal_items 39       // 39 shared signals
#define lst_recall_length 88                // 88 preset-relevant parameters
#define lst_keyEvent_length 6               // 6 key event parameters

#elif DSP_TEST_MODE==3

// configuration for test 3                 -> see Linux Engine - Test 3
#define sig_number_of_params 113            // 3 * (13 ENV params) + 2 * (14 OSC + 8 SHP params) + (16 CMB params) + (6 OUT params) + (2 MASTER params) + (6 KEY params)
#define sig_number_of_param_items 227       // (39 + 44 + 16 + 6 + 2 (* 1) MONO params) + (6 (* 20) POLY params)
#define sig_number_of_signal_items 51       // 51 shared signals
#define lst_recall_length 106               // 106 preset-relevant parameters
#define lst_keyEvent_length 6               // 6 key event parameters

#endif

#define sig_number_of_utilities 2
#define sig_number_of_envelopes 5
#define sig_number_of_env_items 81          // 4 POLY Envelopes (A..Gate) = 4 * 20 = 80 items, 1 MONO (Flanger Decay), total: 81 items
#define sig_number_of_env_segments 4
#define sig_number_of_env_types 3
#define sig_number_of_env_events 3

/* dsp helper values */

#define dsp_samples_to_ms 1e-3
#define dsp_init_pitch_reference 440.f
#define dsp_expon_osc_pitch_from -20
#define dsp_expon_osc_pitch_range 150
#define dsp_expon_lin_pitch_from -150
#define dsp_expon_lin_pitch_range 300
#define dsp_expon_level_from -300
#define dsp_expon_level_range 400
#define dsp_expon_time_from -20
#define dsp_expon_time_range 110
#define dsp_expon_time_factor 104.0781      // measured value to produce exactly time of 16000 (equals highest time)

#define env_norm_peak 0.023766461           // equals 1 / 42.0761 (taken from prototype)
#define env_clip_peak 1.412537545           // equals +3 dB (candidate levelKT clipping)
#define env_init_gateRelease 1              // release time of gate envelopes (in milliseconds)
#define env_highest_finite_time 16000.f     // highest allowed finite time

/* tcd list handling - !!! change for test 2 */

#define lst_number_of_lists 2               // predefined paramId lists (simplifying recal and key event update TCD sequences)

/* internal ids of crucial TCD parameters - remove later, when pe_defines_labels is established */
#define par_envelopeA 0                     // item pointer to (consecutive) envelope parameters A (internal ids)
#define par_envelopeB 13                    // item pointer to (consecutive) envelope parameters B (internal ids)
#define par_envelopeC 26                    // item pointer to (consecutive) envelope parameters C (internal ids)
