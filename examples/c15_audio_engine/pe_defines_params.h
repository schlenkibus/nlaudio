/******************************************************************************/
/** @file       pe_defines_params.h
    @date       2018-03-13
    @version    1.0
    @author     Matthias Seeber
    @brief      c15 parameter definition (according to parameter list)
                ..
    @todo
*******************************************************************************/

#pragma once

#include "dsp_defines_session.h"
#include "pe_defines_config.h"

// define: param id, clock type, poly type, range, scale id, scale arg, postID (-1: ignore, > -1: array pos) [7]
// convention: define poly params with spread = 0 !!
// postId for direct copy/distribute post processing

#if DSP_TEST_MODE==1

// parameter definition: (test 1)
const float param_definition[sig_number_of_params][8] = {

    // ENVELOPE A:
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD
    {        0,      3,      0,      16000,  12,     -20,    -1,     0      },         // 0  ENV_A_ATTACK_TIME
    {        2,      3,      0,      16000,  12,     -20,    -1,     0      },         // 1  ENV_A_DECAY1_TIME
    {        4,      2,      0,      16000,  0,      0,      -1,     0      },         // 2  ENV_A_BREAKPOINT_LEVEL
    {        6,      3,      0,      16000,  12,     -20,    -1,     0      },         // 3  ENV_A_DECAY2_TIME
    {        8,      2,      0,      16000,  0,      0,      -1,     0      },         // 4  ENV_A_SUSTAIN_LEVEL
    {        10,     3,      0,      16000,  12,     -20,    -1,     0      },         // 5  ENV_A_RELEASE_TIME
    {        12,     2,      0,      300,    7,      0,      -1,     0      },         // 6  ENV_A_GAIN
    {        14,     0,      0,      256,    0,      0,      -1,     0      },         // 7  ENV_A_LEVEL_VELOCITY
    {        15,     0,      0,      200,    0,      0,      -1,     0      },         // 8  ENV_A_ATTACK_VELOCITY
    {        16,     0,      0,      200,    0,      0,      -1,     0      },         // 9  ENV_A_RELEASE_VELOCITY
    {        17,     0,      0,      8000,   0,      0,      -1,     0      },         // 10 ENV_A_LEVEL_KEYTRACK
    {        18,     0,      0,      16000,  0,      0,      -1,     0      },         // 11 ENV_A_TIME_KEYTRACK
    {        294,    0,      0,      8000,   0,      0,      -1,     0      },         // 12 ENV_A_ATTACK_CURVE
    // END ENVELOPE A - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // OSCILLATOR A:
    {        53,     3,      0,      100,    8,      -20,    -1,     0      },         // 13 OSC_A_PITCH
    {        55,     3,      0,      10000,  0,      0,      -1,     0      },         // 14 OSC_A_PITCH_KT
    {        57,     3,      0,      16000,  4,      0.95,   5,      1      },         // 15 OSC_A_FLUCTUATION (direct, spread)
    {        60,     2,      0,      8000,   4,      0.5,    -1,     0      },         // 16 OSC_A_PM_SELF
    {        62,     3,      0,      16000,  0,      0,      -1,     0      },         // 17 OSC_A_PM_SELF_ENV_A
    {        302,    3,      0,      200,    9,      80,     -1,     0      },         // 18 OSC_A_CHIRP
    // END OSCILLATOR A - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // MASTER:
    {        247,    2,      0,      16000,  4,      4,      9,      0      },         // 19 MASTER_VOLUME (direct, spread)
    {        248,    3,      0,      100,    0,      0,      -1,     0      },         // 20 MASTER_TUNE
    // END MASTER - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // POLY KEY: (Env C Rate discarded for now)
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST
    {        405,    0,      1,      1,      0,      0,      -1,     0      },         // 21 POLY_NOTE_STEAL
    {        406,    0,      1,      1000,   0,      0,      -1,     0      },         // 22 POLY_PITCH
    {        407,    0,      1,      16000,  0,      0.5,    -1,     0      },         // 23 POLY_VOICE_PAN
    {        408,    0,      1,      14400,  0,      0,      -1,     0      },         // 24 POLY_PHASE_A
    {        409,    0,      1,      14400,  0,      0,      -1,     0      }          // 25 POLY_PHASE_B
    // END POLY KEY - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

};

#elif DSP_TEST_MODE==2

// parameter definition: (test 2)
const float param_definition[sig_number_of_params][8] = {

    // - - - ENVELOPE A - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD
    {        0,      3,      0,      16000,  12,     -20,    -1,     0      },         // 0   ENV_A_ATTACK_TIME
    {        2,      3,      0,      16000,  12,     -20,    -1,     0      },         // 1   ENV_A_DECAY1_TIME
    {        4,      2,      0,      16000,  0,      0,      -1,     0      },         // 2   ENV_A_BREAKPOINT_LEVEL
    {        6,      3,      0,      16000,  12,     -20,    -1,     0      },         // 3   ENV_A_DECAY2_TIME
    {        8,      2,      0,      16000,  0,      0,      -1,     0      },         // 4   ENV_A_SUSTAIN_LEVEL
    {        10,     3,      0,      16000,  12,     -20,    -1,     0      },         // 5   ENV_A_RELEASE_TIME
    {        12,     2,      0,      300,    7,      0,      -1,     0      },         // 6   ENV_A_GAIN
    {        14,     0,      0,      256,    0,      0,      -1,     0      },         // 7   ENV_A_LEVEL_VELOCITY
    {        15,     0,      0,      200,    0,      0,      -1,     0      },         // 8   ENV_A_ATTACK_VELOCITY
    {        16,     0,      0,      200,    0,      0,      -1,     0      },         // 9   ENV_A_RELEASE_VELOCITY
    {        17,     0,      0,      8000,   0,      0,      -1,     0      },         // 10  ENV_A_LEVEL_KEYTRACK
    {        18,     0,      0,      16000,  0,      0,      -1,     0      },         // 11  ENV_A_TIME_KEYTRACK
    {        294,    0,      0,      8000,   0,      0,      -1,     0      },         // 12  ENV_A_ATTACK_CURVE

    // - - - ENVELOPE B - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD
    {        19,     3,      0,      16000,  12,     -20,    -1,     0      },         // 13  ENV_B_ATTACK_TIME
    {        21,     3,      0,      16000,  12,     -20,    -1,     0      },         // 14  ENV_B_DECAY1_TIME
    {        23,     2,      0,      16000,  0,      0,      -1,     0      },         // 15  ENV_B_BREAKPOINT_LEVEL
    {        25,     3,      0,      16000,  12,     -20,    -1,     0      },         // 16  ENV_B_DECAY2_TIME
    {        27,     2,      0,      16000,  0,      0,      -1,     0      },         // 17  ENV_B_SUSTAIN_LEVEL
    {        29,     3,      0,      16000,  12,     -20,    -1,     0      },         // 18  ENV_B_RELEASE_TIME
    {        31,     2,      0,      300,    7,      0,      -1,     0      },         // 19  ENV_B_GAIN
    {        33,     0,      0,      256,    0,      0,      -1,     0      },         // 20  ENV_B_LEVEL_VELOCITY
    {        34,     0,      0,      200,    0,      0,      -1,     0      },         // 21  ENV_B_ATTACK_VELOCITY
    {        35,     0,      0,      200,    0,      0,      -1,     0      },         // 22  ENV_B_RELEASE_VELOCITY
    {        36,     0,      0,      8000,   0,      0,      -1,     0      },         // 23  ENV_B_LEVEL_KEYTRACK
    {        37,     0,      0,      16000,  0,      0,      -1,     0      },         // 24  ENV_B_TIME_KEYTRACK
    {        295,    0,      0,      8000,   0,      0,      -1,     0      },         // 25  ENV_B_ATTACK_CURVE

    // - - - ENVELOPE C - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD
    {        38,     3,      0,      16000,  12,     -20,    -1,     0      },         // 26  ENV_C_ATTACK_TIME
    {        40,     3,      0,      16000,  12,     -20,    -1,     0      },         // 27  ENV_C_DECAY1_TIME
    {        42,     2,      0,      16000,  0,      0,      -1,     0      },         // 28  ENV_C_BREAKPOINT_LEVEL
    {        44,     3,      0,      16000,  12,     -20,    -1,     0      },         // 29  ENV_C_DECAY2_TIME
    {        297,    2,      0,      16000,  0,      0,      -1,     0      },         // 30  ENV_C_SUSTAIN_LEVEL
    {        46,     3,      0,      16000,  12,     -20,    -1,     0      },         // 31  ENV_C_RELEASE_TIME
    {        500,    2,      0,      300,    7,      0,      -1,     0      },         // 32  (PLACEHOLDER)
    {        48,     0,      0,      256,    0,      0,      -1,     0      },         // 33  ENV_C_LEVEL_VELOCITY
    {        49,     0,      0,      200,    0,      0,      -1,     0      },         // 34  ENV_C_ATTACK_VELOCITY
    {        50,     0,      0,      200,    0,      0,      -1,     0      },         // 35  ENV_C_RELEASE_VELOCITY
    {        51,     0,      0,      8000,   0,      0,      -1,     0      },         // 36  ENV_C_LEVEL_KEYTRACK
    {        52,     0,      0,      16000,  0,      0,      -1,     0      },         // 37  ENV_C_TIME_KEYTRACK
    {        296,    0,      0,      8000,   0,      0,      -1,     0      },         // 38  ENV_C_ATTACK_CURVE

    // - - - OSCILLATOR A - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD

    // - - - SHAPER A - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD

    // - - - OSCILLATOR B - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD

    // - - - SHAPER B - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD

    // - - - OUTPUT MIXER - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD

    // - - - MASTER - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD

    // - - - POLY EVENT - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD

};

#endif

// utility parameters
const float utility_definition[sig_number_of_utilities][3] = {
    //       RANGE   SCALE   (ARG)
    {        4096,   0,      0          },                                      // velocity definition (for now: tcd range 4096)
    {        100,    0,      400        }                                       // pitch reference (A3)
};
