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
#include "pe_defines_config.h"

// consecutive paramIds for MONO recall update (defining tcd order of destinations) - listId = 1
// NOTE:    - recall param id order by PLAYGROUND, list mechanism fragile/risky in this case -> discard

const uint32_t paramIds_recall[lst_recall_length] = {
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  14,                // Env A (all params - #13: retrigger hardness placeholder!)
    15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  29,                // Env B (all params - #28: retrigger hardness placeholder!)
    30,  31,  32,  33,  34,  35,  37,  38,  39,  40,  41,  42,  43,                     // Env C (all params - #36: gain placeholder! #44: split placeholder!)
    45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,                // Osc A (all params - no phase)
    59,  60,  61,  62,  63,  64,  65,  66,                                              // Shp A (all params)
    67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,                // Osc B (all params - no phase)
    81,  82,  83,  84,  85,  86,  87,  88,                                              // Shp B (all params)
    89,  90,  91 , 92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104,     // Comb (all params)
    105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117,                    // SFV (all params)
                                                                                        // (later: FB Mix)
    127, 128, 129, 130, 131, 132, 133, 134, 135 ,136, 137, 138,                         // Out Mix (all params)
    139, 140, 141, 142, 143, 144, 145, 146,                                             // Cabinet (all params)
                                                                                        // (later: Gap Filter)
                                                                                        // (later: Flanger)
                                                                                        // (later: Echo)
                                                                                        // (later: Reverb)
    176, 177                                                                            // Master (Vol, Tune)
};

// consecutive paramIds for POLY key event update (defining tcd order of destinations) - listId = 2
// NOTE:    - keyEvent param id order by LPC, list mechanism usable -> keep

const uint32_t paramIds_keyEvent[lst_keyEvent_length] = {
    178, 179, 180, 181, 182, 183
};
