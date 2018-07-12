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

/* Main Configuration                       (prepared for 20 Voices) */

#define dsp_poly_types 2                    // two polyphony types (mono, poly) - (later, a dual type needs to be implemented)
#define dsp_clock_types 4                   // four different parameter types (sync, audio, fast, slow)
#define dsp_number_of_voices 20             // maximum allowed number of voices
#define dsp_take_envelope 1                 // specify which env engine should be used: old (0) or new (1)

const uint32_t dsp_clock_rates[2] = {       // sub-audio clocks are defined in rates (Hz) now
    9600,                                   // fast rate is 9600 Hz
    400                                     // slow rate is 400 Hz
};

/* Main Parameter Definition                -> see Linux Engine - Test 3 */

#define sig_number_of_params 119            // 3 * (15 ENV params) + 2 * (14 OSC + 8 SHP params) + (16 CMB params) + (6 OUT params) + (2 MASTER params) + (6 KEY params)
#define sig_number_of_param_items 233       // (45 + 44 + 16 + 6 + 2 (* 1) MONO params) + (6 (* 20) POLY params)
#define sig_number_of_signal_items 51       // 51 shared signals

/* TCD List Handling */

#define lst_recall_length 109               // 109 preset-relevant parameters
#define lst_keyEvent_length 6               // 6 key event parameters
#define lst_number_of_lists 2               // predefined paramId lists (simplifying recal and key event update TCD sequences)

/* Utility Parameters and Envelope Definition */

#define sig_number_of_utilities 2           // two Utility Parameters: Velocity, Reference Tone
#define sig_number_of_envelopes 5           // five Envelope Units: A, B, C, Gate, Flanger
#define sig_number_of_env_items 81          // 4 POLY Envelopes (A..Gate) = 4 * 20 = 80 items, 1 MONO (Flanger Decay), total: 81 items
#define sig_number_of_env_segments 4        // four segments for ADBDSR-type Envelopes (A, B, C): Attack, Decay 1, Decay 2, Release
#define sig_number_of_env_types 3           // three Envelope types: ADBDSR (A, B, C), Gate (Gate), Decay (Flanger)
#define sig_number_of_env_events 3          // three Envelope Event objects for Envelopes A, B, C (managing Velocity and KeyPos responses)

/* DSP Helper Values */

#define dsp_samples_to_ms 1e-3              // 1000 ms = 1 s
#define dsp_init_pitch_reference 440.f      // standard Frequency of A3 Note
#define dsp_expon_osc_pitch_from -20        // lowest logarithmic Pitch value for Oscillator unit
#define dsp_expon_osc_pitch_range 150       // range of logarithmic Pitch value for Oscillator unit ([-20 ... 130] ST = 150 ST)
#define dsp_expon_lin_pitch_from -150       // lowest logarithmic Pitch value for linear conversion
#define dsp_expon_lin_pitch_range 300       // range of logarithmic Pitch value for linear conversion ([-150 ... 150] ST = 300 ST)
#define dsp_expon_level_from -300           // lowest logarithmic Level value for gain conversion (-300 dB)
#define dsp_expon_level_range 400           // range of logarithmic Level value for gain conversion ([-300 ... 100] dB = 400 dB)
#define dsp_expon_time_from -20             // lowest logarithmic Time value for time conversion (-20 dB)
#define dsp_expon_time_range 110            // range of logarithmic Time value for time conversion ([-20 ... 90] dB = 110 dB)
#define dsp_expon_time_factor 104.0781      // measured value to produce exactly time of 16000 (equals highest time)
#define dsp_comb_max_freqFactor 19.0166     // measured value of highest frequency factor for the Comb Filter to run without bypass (corresponding to Pitch of 119.99 ST)
#define dsp_render_min 1e-9                 // minimal rendered value for exponential transitions

#define env_norm_peak 0.023766461           // equals 1 / 42.0761 (taken from prototype)
#define env_clip_peak 1.412537545           // measured value for LevelKT Clipping, equals +3 dB (candidate)
#define env_init_gateRelease 1              // release time of gate envelopes (in milliseconds)
#define env_highest_finite_time 16000.f     // highest allowed finite time

/* Internal IDs of crucial TCD parameters */

#define par_envelopeA 0                     // item pointer to (consecutive) envelope parameters A (internal ids)
#define par_envelopeB 15                    // item pointer to (consecutive) envelope parameters B (internal ids)
#define par_envelopeC 30                    // item pointer to (consecutive) envelope parameters C (internal ids)
