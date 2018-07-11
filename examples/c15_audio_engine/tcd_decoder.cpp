#include "tcd_decoder.h"

/* proper init */

void decoder::init()
{
    uint32_t i;
    /* construct traversable paramID lists (recall, key event) */
    for(i = 0; i < lst_recall_length; i++)
    {
        m_listTraversal[0].add(paramIds_recall[i]);                     // construct ID list for recall list traversal (former serial/select mode)
    }
    for(i = 0; i < lst_keyEvent_length; i++)
    {
        m_listTraversal[1].add(paramIds_keyEvent[i]);                   // construct ID list for key event traversal (former serial/select mode)
    }
}

/* evaluate MIDI status and return TCD Command ID (0 - ignore, >0 - valid tcd commands, see pe_defines_protocol.h) */

uint32_t decoder::getCommandId(const uint32_t _status)
{
    return tcd_protocol[_status];                                       // lookup in predefined command map, return command id (according to MIDI STATUS)
}

/* tcd argument parsing (no safety mechanisms, data bytes are expected within [0...127] range!) */

uint32_t decoder::unsigned14(const uint32_t _data0, const uint32_t _data1)
{
    return((_data0 << 7) + _data1);                                     // parse and return an unsigned 14-bit value according to MIDI DATA bytes
}

int32_t decoder::signed14(const uint32_t _data0, const uint32_t _data1)
{
    return(m_getSign[_data0 >> 6] * unsigned14((_data0 & 63), _data1)); // parse and return a signed 14-bit value according to MIDI DATA bytes
}

void decoder::unsigned28upper(const uint32_t _data0, const uint32_t _data1)
{
    m_sign = 1;                                                         // overwrite sign (always positive)
    m_value = unsigned14(_data0, _data1) << 14;                         // parse and hold an unsigned 14-bit value for the upper 14 bits of 28 bit value according to MIDI DATA bytes
}

void decoder::signed28upper(const uint32_t _data0, const uint32_t _data1)
{
    m_sign = m_getSign[_data0 >> 6];                                    // determine sign according to MSB of first MIDI DATA byte
    m_value = unsigned14((_data0 & 63), _data1) << 14;                  // parse and hold an unsigned 13-bit value for the upper 14 bits of 28 bit value according to MIDI DATA bytes
}

int32_t decoder::apply28lower(const uint32_t _data0, const uint32_t _data1)
{
    m_value += unsigned14(_data0, _data1);                              // parse an unsigned 14-bit value according to MIDI DATA bytes and add it to current value
    return(m_sign * m_value);                                           // parse resulting value (sign * magnitude) and return it
}

/* handle TCD selection by event-based evaluation (check ID against FROM, TO) */

uint32_t decoder::selectionEvent(const uint32_t _from, const uint32_t _to, const int32_t _id)
{
    m_event[1] = _id - _from;
    m_event[2] = _to - _id;
    m_event[3] = m_event[1] | m_event[2];
    m_event[4] = m_event[1] & m_event[2];
    return(0 > m_event[3 + m_event[0]] ? 0 : 1);
}

/* decoder list traversal (recall, key event) */

uint32_t decoder::traverseRecall()
{
    /* monophonic implementation - compatible with current Reaktor5 Rendering Engine */
    const uint32_t id = m_listTraversal[0].m_data[m_listIndex];
    // increase (and wrap) index
    m_listIndex = (m_listIndex + 1) % m_listTraversal[0].m_length;
    // return current
    return id;
}

uint32_t decoder::traverseKeyEvent()
{
    /* polyphonic implementation - voice selection needs to be done by sender! */
    const uint32_t id = m_listTraversal[1].m_data[m_listIndex];
    // increase (and wrap) index
    m_listIndex = (m_listIndex + 1) % m_listTraversal[1].m_length;
    // return current
    return id;
}
