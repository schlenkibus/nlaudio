/******************************************************************************/
/** @file		tcd_decoder.h
    @date       2018-03-13
    @version    1.0
    @author     Matthias Seeber
    @brief		..

    @todo
*******************************************************************************/

#pragma once

#include <stdint.h>
#include "pe_utilities.h"
#include "pe_defines_protocol.h"
#include "pe_defines_lists.h"

struct decoder
{
    /* local variables */
    uint32_t m_voiceFrom;
    uint32_t m_voiceTo;
    uint32_t m_paramFrom;
    uint32_t m_paramTo;
    uint32_t m_value;
    uint32_t m_sign;
    uint32_t m_utilityId = 0;
    uint32_t m_listId = 0;
    uint32_t m_listIndex = 0;
    /* local data structures */
    const int32_t m_getSign[2] = {1, -1};
    uint32_t m_event[5];
    dual_id_list m_selectedVoices;
    polyDual_id_list m_selectedParams;
    id_list m_listTraversal[lst_number_of_lists];
    /* proper init */
    void init();
    /* tcd command evaluation */
    uint32_t getCommandId(const uint32_t _status);
    /* tcd argument parsing */
    uint32_t unsigned14(const uint32_t _data0, const uint32_t _data1);
    int32_t signed14(const uint32_t _data0, const uint32_t _data1);
    void unsigned28upper(const uint32_t _data0, const uint32_t _data1);
    void signed28upper(const uint32_t _data0, const uint32_t _data1);
    int32_t apply28lower(const uint32_t _data0, const uint32_t _data1);
    /* tcd voice and parameter selection event evaluation */
    uint32_t selectionEvent(const uint32_t _from, const uint32_t _to, const uint32_t _id);
    /* tcd list traversal */
    uint32_t traverseRecall();
    uint32_t traverseKeyEvent();
};
