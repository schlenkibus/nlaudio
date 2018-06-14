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
const float param_definition[sig_number_of_params][10] = {

    // - - - ENVELOPE A - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
    {        0,      3,      0,      16000,  12,     -20,    -1,     0,      0,      16000  },         // 0   ENV_A_ATTACK_TIME
    {        2,      3,      0,      16000,  12,     -20,    -1,     0,      0,      16000  },         // 1   ENV_A_DECAY1_TIME
    {        4,      2,      0,      16000,  0,      0,      -1,     0,      0,      16000  },         // 2   ENV_A_BREAKPOINT_LEVEL
    {        6,      3,      0,      16000,  12,     -20,    -1,     0,      0,      16000  },         // 3   ENV_A_DECAY2_TIME
    {        8,      2,      0,      16000,  0,      0,      -1,     0,      0,      16000  },         // 4   ENV_A_SUSTAIN_LEVEL
    {        10,     3,      0,      16000,  12,     -20,    -1,     0,      0,      16160  },         // 5   ENV_A_RELEASE_TIME
    {        12,     2,      0,      300,    7,      0,      -1,     0,      1,      7200   },         // 6   ENV_A_GAIN
    {        14,     0,      0,      256,    0,      0,      -1,     0,      0,      15360  },         // 7   ENV_A_LEVEL_VELOCITY
    {        15,     0,      0,      200,    0,      0,      -1,     0,      0,      12000  },         // 8   ENV_A_ATTACK_VELOCITY
    {        16,     0,      0,      200,    0,      0,      -1,     0,      0,      12000  },         // 9   ENV_A_RELEASE_VELOCITY
    {        17,     0,      0,      8000,   0,      0,      -1,     0,      1,      8000   },         // 10  ENV_A_LEVEL_KEYTRACK
    {        18,     0,      0,      16000,  0,      0,      -1,     0,      0,      16000  },         // 11  ENV_A_TIME_KEYTRACK
    {        294,    0,      0,      8000,   0,      0,      -1,     0,      1,      8000   },         // 12  ENV_A_ATTACK_CURVE

    // - - - ENVELOPE B - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
    {        19,     3,      0,      16000,  12,     -20,    -1,     0,      0,      16000  },         // 13  ENV_B_ATTACK_TIME
    {        21,     3,      0,      16000,  12,     -20,    -1,     0,      0,      16000  },         // 14  ENV_B_DECAY1_TIME
    {        23,     2,      0,      16000,  0,      0,      -1,     0,      0,      16000  },         // 15  ENV_B_BREAKPOINT_LEVEL
    {        25,     3,      0,      16000,  12,     -20,    -1,     0,      0,      16000  },         // 16  ENV_B_DECAY2_TIME
    {        27,     2,      0,      16000,  0,      0,      -1,     0,      0,      16000  },         // 17  ENV_B_SUSTAIN_LEVEL
    {        29,     3,      0,      16000,  12,     -20,    -1,     0,      0,      16160  },         // 18  ENV_B_RELEASE_TIME
    {        31,     2,      0,      300,    7,      0,      -1,     0,      1,      7200   },         // 19  ENV_B_GAIN
    {        33,     0,      0,      256,    0,      0,      -1,     0,      0,      15360  },         // 20  ENV_B_LEVEL_VELOCITY
    {        34,     0,      0,      200,    0,      0,      -1,     0,      0,      12000  },         // 21  ENV_B_ATTACK_VELOCITY
    {        35,     0,      0,      200,    0,      0,      -1,     0,      0,      12000  },         // 22  ENV_B_RELEASE_VELOCITY
    {        36,     0,      0,      8000,   0,      0,      -1,     0,      1,      8000   },         // 23  ENV_B_LEVEL_KEYTRACK
    {        37,     0,      0,      16000,  0,      0,      -1,     0,      0,      16000  },         // 24  ENV_B_TIME_KEYTRACK
    {        295,    0,      0,      8000,   0,      0,      -1,     0,      1,      8000   },         // 25  ENV_B_ATTACK_CURVE

    // - - - ENVELOPE C - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
    {        38,     3,      0,      16000,  12,     -20,    -1,     0,      0,      16000  },         // 26  ENV_C_ATTACK_TIME
    {        40,     3,      0,      16000,  12,     -20,    -1,     0,      0,      16000  },         // 27  ENV_C_DECAY1_TIME
    {        42,     2,      0,      8000,   0,      0,      -1,     0,      1,      8000   },         // 28  ENV_C_BREAKPOINT_LEVEL
    {        44,     3,      0,      16000,  12,     -20,    -1,     0,      0,      16000  },         // 29  ENV_C_DECAY2_TIME
    {        297,    2,      0,      8000,   0,      0,      -1,     0,      1,      8000   },         // 30  ENV_C_SUSTAIN_LEVEL
    {        46,     3,      0,      16000,  12,     -20,    -1,     0,      0,      16160  },         // 31  ENV_C_RELEASE_TIME
    {        500,    2,      0,      300,    7,      0,      -1,     0,      1,      7200   },         // 32  (GAIN PLACEHOLDER)
    {        48,     0,      0,      256,    0,      0,      -1,     0,      0,      15360  },         // 33  ENV_C_LEVEL_VELOCITY
    {        49,     0,      0,      200,    0,      0,      -1,     0,      0,      12000  },         // 34  ENV_C_ATTACK_VELOCITY
    {        50,     0,      0,      200,    0,      0,      -1,     0,      0,      12000  },         // 35  ENV_C_RELEASE_VELOCITY
    {        51,     0,      0,      8000,   0,      0,      -1,     0,      1,      8000   },         // 36  ENV_C_LEVEL_KEYTRACK
    {        52,     0,      0,      16000,  0,      0,      -1,     0,      0,      16000  },         // 37  ENV_C_TIME_KEYTRACK
    {        296,    0,      0,      8000,   0,      0,      -1,     0,      1,      8000   },         // 38  ENV_C_ATTACK_CURVE

    // - - - OSCILLATOR A - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
    {        53,     3,      0,      100,    8,      -20,    -1,     0,      0,      15000  },         // 39  OSC_A_PITCH
    {        55,     3,      0,      10000,  0,      0,      -1,     0,      0,      10000  },         // 40  OSC_A_PITCH_KEYTRACK
    {        56,     3,      0,      100,    0,      0,      -1,     0,      1,      8000   },         // 41  OSC_A_PITCH_ENV_C
    {        57,     3,      0,      16000,  4,      0.95,   -1,     0,      0,      16000  },         // 42  OSC_A_FLUCTUATION
    {        59,     3,      0,      16000,  0,      0,      -1,     0,      0,      16000  },         // 43  OSC_A_FLUCTUATION_ENV_C
    {        60,     2,      0,      8000,   4,      0.5,    -1,     0,      1,      8000   },         // 44  OSC_A_PM_SELF
    {        62,     3,      0,      16000,  0,      0,      -1,     0,      0,      16000  },         // 45  OSC_A_PM_SELF_ENV_A
    {        63,     3,      0,      8000,   0,      0,      7,      1,      1,      8000   },         // 46  OSC_A_PM_SELF_SHAPER
    {        64,     2,      0,      8000,   4,      1,      -1,     0,      1,      8000   },         // 47  OSC_A_PM_B
    {        66,     3,      0,      16000,  0,      0,      -1,     0,      0,      16000  },         // 48  OSC_A_PM_B_ENV_B
    {        67,     3,      0,      8000,   0,      0,      9,      1,      1,      8000   },         // 49  OSC_A_PM_B_SHAPER
    {        68,     2,      0,      8000,   4,      0.5,    -1,     0,      1,      8000   },         // 50  OSC_A_PM_FB
    {        70,     3,      0,      16000,  0,      0,      -1,     0,      0,      16000  },         // 51  OSC_A_PM_FB_ENV_C
    {        303,    3,      0,      200,    9,      80,     -1,     0,      0,      12000  },         // 52  OSC_A_CHIRP

    // - - - SHAPER A - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
    {        71,     2,      0,      200,    10,     0.18,   -1,     0,      0,      10000  },         // 53  SHP_A_DRIVE
    {        73,     3,      0,      16000,  0,      0,      -1,     0,      0,      16000  },         // 54  SHP_A_DRIVE_ENV_A
    {        74,     3,      0,      16000,  0,      0,      13,     1,      0,      16000  },         // 55  SHP_A_FOLD
    {        75,     3,      0,      16000,  0,      0,      14,     1,      0,      16000  },         // 56  SHP_A_ASYMETRY
    {        76,     2,      0,      8000,   0,      0,      15,     1,      1,      8000   },         // 57  SHP_A_MIX
    {        78,     2,      0,      16000,  5,      0,      16,     1,      0,      16000  },         // 58  SHP_A_FEEDBACK_MIX
    {        80,     3,      0,      16000,  0,      0,      -1,     1,      0,      16000  },         // 59  SHP_A_FEEDBACK_ENV_C
    {        81,     2,      0,      16000,  0,      0,      18,     1,      0,      16000  },         // 60  SHP_A_RINGMOD

    // - - - OSCILLATOR B - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
    {        83,     3,      0,      100,    8,      -20,    -1,     0,      0,      15000  },         // 61  OSC_B_PITCH
    {        85,     3,      0,      10000,  0,      0,      -1,     0,      0,      10000  },         // 62  OSC_B_PITCH_KEYTRACK
    {        86,     3,      0,      100,    0,      0,      -1,     0,      1,      8000   },         // 63  OSC_B_PITCH_ENV_C
    {        87,     3,      0,      16000,  4,      0.95,   -1,     0,      0,      16000  },         // 64  OSC_B_FLUCTUATION
    {        89,     3,      0,      16000,  0,      0,      -1,     0,      0,      16000  },         // 65  OSC_B_FLUCTUATION_ENV_C
    {        90,     2,      0,      8000,   4,      0.5,    -1,     0,      1,      8000   },         // 66  OSC_B_PM_SELF
    {        92,     3,      0,      16000,  0,      0,      -1,     0,      0,      16000  },         // 67  OSC_B_PM_SELF_ENV_B
    {        93,     3,      0,      8000,   0,      0,      22,     1,      1,      8000   },         // 68  OSC_B_PM_SELF_SHAPER
    {        94,     2,      0,      8000,   4,      1,      -1,     0,      1,      8000   },         // 69  OSC_B_PM_A
    {        96,     3,      0,      16000,  0,      0,      -1,     0,      0,      16000  },         // 70  OSC_B_PM_A_ENV_A
    {        97,     3,      0,      8000,   0,      0,      24,     1,      1,      8000   },         // 71  OSC_B_PM_A_SHAPER
    {        98,     2,      0,      8000,   4,      0.5,    -1,     0,      1,      8000   },         // 72  OSC_B_PM_FB
    {        100,    3,      0,      16000,  0,      0,      -1,     0,      0,      16000  },         // 73  OSC_B_PM_FB_ENV_C
    {        304,    3,      0,      200,    9,      80,     -1,     0,      0,      12000  },         // 74  OSC_B_CHIRP

    // - - - SHAPER B - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
    {        101,    2,      0,      200,    10,     0.18,   -1,     0,      0,      10000  },         // 75  SHP_B_DRIVE
    {        103,    3,      0,      16000,  0,      0,      -1,     0,      0,      16000  },         // 76  SHP_B_DRIVE_ENV_B
    {        104,    3,      0,      16000,  0,      0,      28,     1,      0,      16000  },         // 77  SHP_B_FOLD
    {        105,    3,      0,      16000,  0,      0,      29,     1,      0,      16000  },         // 78  SHP_B_ASYMETRY
    {        106,    2,      0,      8000,   0,      0,      30,     1,      1,      8000   },         // 79  SHP_B_MIX
    {        108,    2,      0,      16000,  5,      0,      31,     1,      0,      16000  },         // 80  SHP_B_FEEDBACK_MIX
    {        110,    3,      0,      16000,  0,      0,      -1,     1,      0,      16000  },         // 81  SHP_B_FEEDBACK_ENV_C
    {        111,    2,      0,      16000,  0,      0,      33,     1,      0,      16000  },         // 82  SHP_B_RINGMOD

    // - - - OUTPUT MIXER - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
    {        169,    2,      0,      4000,   0,      0,      -1,     0,      1,      8000   },         // 83  OUT_A_LEVEL
    {        171,    2,      0,      16000,  0,      0.5,    -1,     0,      1,      8000   },         // 84  OUT_A_PAN
    {        172,    2,      0,      4000,   0,      0,      -1,     0,      1,      8000   },         // 85  OUT_B_LEVEL
    {        174,    2,      0,      16000,  0,      0.5,    -1,     0,      1,      8000   },         // 86  OUT_B_PAN

    // - - - MASTER - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
    {        247,    2,      0,      16000,  4,      4,      38,     0,      0,      16000  },         // 87  MASTER_VOLUME
    {        248,    3,      0,      100,    0,      0,      -1,     0,      1,      4800   },         // 88  MASTER_TUNE

    // - - - POLY KEY EVENT - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
    {        400,    0,      1,      14400,  0,      0,      -1,     0,      1,      7200   },         // 89  KEY_PHASE_A
    {        401,    0,      1,      14400,  0,      0,      -1,     0,      1,      7200   },         // 90  KEY_PHASE_B
    {        406,    0,      1,      1000,   0,      0,      -1,     0,      1,      1000   },         // 91  KEY_NOTE_PITCH
    {        407,    0,      1,      16000,  0,      0.5,    -1,     0,      0,      16000  },         // 92  KEY_VOICE_PAN
    {        408,    0,      1,      16000,  0,      0,      -1,     0,      0,      16000  },         // 93  KEY_ENV_C_RATE
    {        409,    0,      1,      1,      0,      0,      -1,     0,      0,      1      }          // 94  KEY_VOICE_STEAL

};

#elif DSP_TEST_MODE==3



#endif

// utility parameters
const float utility_definition[sig_number_of_utilities][3] = {
    //       RANGE   SCALE   (ARG)
    {        4096,   0,      0          },                                      // velocity definition (for now: tcd range 4096)
    {        100,    0,      400        }                                       // pitch reference (A3)
};
