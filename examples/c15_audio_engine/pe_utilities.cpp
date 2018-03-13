#include "pe_utilities.h"

/* single ID list methods (reset list, add id to list) */

void id_list::reset()
{
    m_length = 0;
}

void id_list::add(const uint32_t id)
{
    m_data[m_length] = id;
    m_length++;
}

/* dual ID list methods (reset list, add id to specified sub-list) */

void dual_id_list::reset()
{
    m_data[0].reset();
    m_data[1].reset();
}

void dual_id_list::add(const uint32_t listId, const uint32_t id)
{
    m_data[listId].add(id);
}

/* polyDual ID list methods (reset list, add id to specified sub-list) */

void polyDual_id_list::reset()
{
    for(uint32_t i = 0; i < dsp_poly_types; i++)
    {
        m_data[i].reset();
    }
}

void polyDual_id_list::add(const uint32_t polyId, const uint32_t listId, const uint32_t id)
{
    m_data[polyId].add(listId, id);
}

/* poly ID list methods (reset list, add id to specified sub-list) */

void poly_id_list::reset()
{
    for(uint32_t i = 0; i < dsp_poly_types; i++)
    {
        m_data[i].reset();
    }
}

void poly_id_list::add(const uint32_t polyType, const uint32_t id)
{
    m_data[polyType].add(id);
}

/* clock ID list methods (reset list, add id to specified sub-list) */

void clock_id_list::reset()
{
    for(uint32_t i = 0; i < dsp_clock_types; i++)
    {
        m_data[i].reset();
    }
}

void clock_id_list::add(const uint32_t clockType, const uint32_t polyType, const uint32_t id)
{
    m_data[clockType].add(polyType, id);
}

/* dual clock ID list methods (reset list, add id to specified sub-list) */

void dual_clock_id_list::reset()
{
    m_data[0].reset();
    m_data[1].reset();
}

void dual_clock_id_list::add(const uint32_t listId, const uint32_t clockType, const uint32_t polyType, const uint32_t id)
{
    m_data[listId].add(clockType, polyType, id);
}

/* env ID list methods (reset list, add id to list) */

void env_id_list::reset()
{
    m_length = 0;
}

void env_id_list::add(const uint32_t id)
{
    m_data[m_length] = id;
    m_length++;
}

/* dual env ID list methods (reset list, add id to specified sub-list) */

void dual_env_id_list::reset()
{
    m_data[0].reset();
    m_data[1].reset();
}

void dual_env_id_list::add(const uint32_t listId, const uint32_t id)
{
    m_data[listId].add(id);
}
