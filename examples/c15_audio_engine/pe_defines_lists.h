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
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,   // Env A (all params)
    13, 14, 15, 16, 17, 18,                     // Osc A (small selection of params)
    19, 20                                      // Master (tune, level)
};

// consecutive paramIds for POLY key event update (defining tcd order of destinations) - listId = 2
// NOTE:    - keyEvent param id order by LPC, list mechanism usable -> keep

const uint32_t paramIds_keyEvent[lst_keyEvent_length] = {
    21, 22, 23, 24, 25
};
