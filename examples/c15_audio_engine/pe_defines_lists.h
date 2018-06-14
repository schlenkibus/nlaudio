/******************************************************************************/
/** @file       pe_defines_lists.h
    @date       2018-03-13
    @version    1.0
    @author     Matthias Seeber
    @brief      predefined id list declaration (recall, key event)
                ..
    @todo
*******************************************************************************/

#pragma once

#include <stdint.h>
#include "dsp_defines_session.h"
#include "pe_defines_config.h"

// consecutive paramIds for MONO recall update (defining tcd order of destinations) - listId = 1
// NOTE:    - recall param id order by PLAYGROUND, list mechanism fragile/risky in this case -> discard

#if DSP_TEST_MODE==1

const uint32_t paramIds_recall[lst_recall_length] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,   // Env A (all params)
    13, 14, 15, 16, 17, 18,                     // Osc A (small selection of params)
    19, 20                                      // Master (tune, level)
};

// consecutive paramIds for POLY key event update (defining tcd order of destinations) - listId = 2
// NOTE:    - keyEvent param id order by LPC, list mechanism usable -> keep

const uint32_t paramIds_keyEvent[lst_keyEvent_length] = {
    21, 22, 23, 24, 25
};

#elif DSP_TEST_MODE==2

const uint32_t paramIds_recall[lst_recall_length] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,         // Env A (all params)
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,         // Env B (all params)
    26, 27, 28, 29, 30, 31, 33, 34, 35, 36, 37, 38,             // Env C (all params - #32: gain placeholder!)
    39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,     // Osc A (all params - no phase)
    53, 54, 55, 56, 57, 58, 59, 60,                             // Shp A (all params)
    61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74,     // Osc B (all params - no phase)
    75, 76, 77, 78, 79, 80, 81, 82,                             // Shp B (all params)
    83, 84, 85, 86,                                             // Out Mix (A & B - Lvl & Pan)
    87, 88                                                      // Master (Vol, Tune)
};

// consecutive paramIds for POLY key event update (defining tcd order of destinations) - listId = 2
// NOTE:    - keyEvent param id order by LPC, list mechanism usable -> keep

const uint32_t paramIds_keyEvent[lst_keyEvent_length] = {
    89, 90, 91, 92, 93, 94
};

#elif DSP_TEST_MODE==3

const uint32_t paramIds_recall[lst_recall_length] = {
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,                     // Env A (all params)
    13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,                     // Env B (all params)
    26,  27,  28,  29,  30,  31,  33,  34,  35,  36,  37,  38,                          // Env C (all params - #32: gain placeholder!)
    39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,                // Osc A (all params - no phase)
    53,  54,  55,  56,  57,  58,  59,  60,                                              // Shp A (all params)
    61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,                // Osc B (all params - no phase)
    75,  76,  77,  78,  79,  80,  81,  82,                                              // Shp B (all params)
    83,  84,  85 , 86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,      // Comb (all params)
    99,  100, 101, 102, 103, 104,                                                       // Out Mix (A, B, Comb - Lvl & Pan)
    105, 106                                                                            // Master (Vol, Tune)
};

// consecutive paramIds for POLY key event update (defining tcd order of destinations) - listId = 2
// NOTE:    - keyEvent param id order by LPC, list mechanism usable -> keep

const uint32_t paramIds_keyEvent[lst_keyEvent_length] = {
    107, 108, 109, 110, 111, 112
};

#endif
