#include "pe_env_engine2.h"

/**************************************             **************************************/

/* */
void env_object_adbdsr_split::start(const uint32_t _voiceId)
{
    /* */
    env_body_split* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[m_startIndex].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[m_startIndex].m_dx[_voiceId];
    body->m_start_magnitude = body->m_signal_magnitude;
    body->m_start_timbre = body->m_signal_timbre;
    /* */
    body->m_state = m_segment[m_startIndex].m_state;
    body->m_index = m_startIndex;
    body->m_next = m_segment[m_startIndex].m_next;
}

/* */
void env_object_adbdsr_split::stop(const uint32_t _voiceId)
{
    /* */
    env_body_split* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[m_startIndex].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[m_startIndex].m_dx[_voiceId];
    body->m_start_magnitude = body->m_signal_magnitude;
    body->m_start_timbre = body->m_signal_timbre;
    /* */
    body->m_state = m_segment[m_stopIndex].m_state;
    body->m_index = m_stopIndex;
    body->m_next = m_segment[m_stopIndex].m_next;
}

/* */
void env_object_adbdsr_split::tick(const uint32_t _voiceId)
{
    /* */
    env_body_split* body = &m_body[_voiceId];
    const uint32_t segment = body->m_index;
    const float diff_magnitude = m_segment[segment].m_dest_magnitude[_voiceId] - body->m_start_magnitude;
    const float diff_timbre = m_segment[segment].m_dest_timbre[_voiceId] - body->m_start_timbre;
    /* */
    switch(body->m_state)
    {
    case 0:
        /* idle */
        break;
    case 1:
        /* linear rendering (decay1 phase) */
        if(body->m_x < 1.f)
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * body->m_x);
            body->m_signal_timbre = body->m_start_timbre + (diff_timbre * body->m_x);
            /* */
            body->m_x += m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
            body->m_signal_timbre = m_segment[segment].m_dest_timbre[_voiceId];
            /* */
            nextSegment(_voiceId);
        }
        break;
    case 2:
        /* exponential, quasi-infinite rendering (decay2 phase) */
        if(body->m_y > dsp_render_min)
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * (1.f - body->m_y));
            body->m_signal_timbre = body->m_start_timbre + (diff_timbre * (1.f - body->m_y));
            /* */
            body->m_y *= 1.f - m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + diff_magnitude;
            body->m_signal_timbre = body->m_start_timbre + diff_timbre;
        }
        break;
    case 3:
        /* exponential, quasi-finite rendering (release phase) */
        if(body->m_y > dsp_render_min)
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * (1.f - body->m_y));
            body->m_signal_timbre = body->m_start_timbre + (diff_timbre * (1.f - body->m_y));
            /* */
            body->m_y *= 1.f - m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
            body->m_signal_timbre = m_segment[segment].m_dest_timbre[_voiceId];
            /* */
            nextSegment(_voiceId);
        }
        break;
    case 4:
        /* polynomial rendering (attack phase) */
        if(body->m_x < 1.f)
        {
            /* */
            float x = NlToolbox::Curves::SquaredCurvature(body->m_x, m_segment[segment].m_curve);
            x = NlToolbox::Curves::SquaredCurvature(x, m_segment[segment].m_curve);
            x = NlToolbox::Curves::SquaredCurvature(x, m_segment[segment].m_curve);
            x = NlToolbox::Curves::SquaredCurvature(x, m_segment[segment].m_curve);
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * body->m_x);
            body->m_signal_timbre = body->m_start_timbre + (diff_timbre * body->m_x);
            /* */
            body->m_x *= 1.f - m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
            body->m_signal_timbre = m_segment[segment].m_dest_timbre[_voiceId];
            /* */
            nextSegment(_voiceId);
        }
        break;
    }
}

/* */
void env_object_adbdsr_split::nextSegment(const uint32_t _voiceId)
{
    /* */
    env_body_split* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[body->m_index].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[body->m_index].m_dx[_voiceId];
    body->m_start_magnitude = body->m_signal_magnitude;
    body->m_start_timbre = body->m_signal_timbre;
    /* */
    body->m_state = m_segment[body->m_next].m_state;
    body->m_index = body->m_next;
    body->m_next = m_segment[body->m_index].m_next;
}

/* */
void env_object_adbdsr_split::setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value)
{
    /* */
    m_segment[_segmentId].m_dx[_voiceId] = _value;
}

/* */
void env_object_adbdsr_split::setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const float _magnitude, const float _timbre)
{
    /* */
    m_segment[_segmentId].m_dest_magnitude[_voiceId] = _magnitude;
    m_segment[_segmentId].m_dest_timbre[_voiceId] = _timbre;
}

/**************************************             **************************************/

/* */
void env_object_adbdsr_retrig::start(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[m_startIndex].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[m_startIndex].m_dx[_voiceId];
    body->m_start_magnitude = (1.f - m_retriggerHardness) * body->m_signal_magnitude;
    /* */
    body->m_state = m_segment[m_startIndex].m_state;
    body->m_index = m_startIndex;
    body->m_next = m_segment[m_startIndex].m_next;
}

/* */
void env_object_adbdsr_retrig::stop(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[m_startIndex].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[m_startIndex].m_dx[_voiceId];
    body->m_start_magnitude = body->m_signal_magnitude;
    /* */
    body->m_state = m_segment[m_stopIndex].m_state;
    body->m_index = m_stopIndex;
    body->m_next = m_segment[m_stopIndex].m_next;
}

/* */
void env_object_adbdsr_retrig::tick(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    const uint32_t segment = body->m_index;
    const float diff_magnitude = m_segment[segment].m_dest_magnitude[_voiceId] - body->m_start_magnitude;
    /* */
    switch(body->m_state)
    {
    case 0:
        /* idle */
        break;
    case 1:
        /* linear rendering (decay1 phase) */
        if(body->m_x < 1.f)
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * body->m_x);
            /* */
            body->m_x += m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
            /* */
            nextSegment(_voiceId);
        }
        break;
    case 2:
        /* exponential, quasi-infinite rendering (decay2 phase) */
        if(body->m_y > dsp_render_min)
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * (1.f - body->m_y));
            /* */
            body->m_y *= 1.f - m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + diff_magnitude;
        }
        break;
    case 3:
        /* exponential, quasi-finite rendering (release phase) */
        if(body->m_y > dsp_render_min)
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * (1.f - body->m_y));
            /* */
            body->m_y *= 1.f - m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
            /* */
            nextSegment(_voiceId);
        }
        break;
    case 4:
        /* polynomial rendering (attack phase) */
        if(body->m_x < 1.f)
        {
            /* */
            float x = NlToolbox::Curves::SquaredCurvature(body->m_x, m_segment[segment].m_curve);
            x = NlToolbox::Curves::SquaredCurvature(x, m_segment[segment].m_curve);
            x = NlToolbox::Curves::SquaredCurvature(x, m_segment[segment].m_curve);
            x = NlToolbox::Curves::SquaredCurvature(x, m_segment[segment].m_curve);
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * body->m_x);
            /* */
            body->m_x *= 1.f - m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
            /* */
            nextSegment(_voiceId);
        }
        break;
    }
}

/* */
void env_object_adbdsr_retrig::nextSegment(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[body->m_index].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[body->m_index].m_dx[_voiceId];
    body->m_start_magnitude = body->m_signal_magnitude;
    /* */
    body->m_state = m_segment[body->m_next].m_state;
    body->m_index = body->m_next;
    body->m_next = m_segment[body->m_index].m_next;
}

/* */
void env_object_adbdsr_retrig::setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value)
{
    /* */
    m_segment[_segmentId].m_dx[_voiceId] = _value;
}

/* */
void env_object_adbdsr_retrig::setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const float _magnitude)
{
    /* */
    m_segment[_segmentId].m_dest_magnitude[_voiceId] = _magnitude;
}

/**************************************             **************************************/

/* */
void env_object_gate::start(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[m_startIndex].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[m_startIndex].m_dx[_voiceId];
    body->m_start_magnitude = body->m_signal_magnitude;
    /* */
    body->m_state = m_segment[m_startIndex].m_state;
    body->m_index = m_startIndex;
    body->m_next = m_segment[m_startIndex].m_next;
}

/* */
void env_object_gate::stop(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[m_startIndex].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[m_startIndex].m_dx[_voiceId];
    body->m_start_magnitude = body->m_signal_magnitude;
    /* */
    body->m_state = m_segment[m_stopIndex].m_state;
    body->m_index = m_stopIndex;
    body->m_next = m_segment[m_stopIndex].m_next;
}

/* */
void env_object_gate::tick(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    const uint32_t segment = body->m_index;
    const float diff_magnitude = m_segment[segment].m_dest_magnitude[_voiceId] - body->m_start_magnitude;
    /* */
    switch(body->m_state)
    {
    case 0:
        /* idle */
        break;
    case 1:
        /* linear rendering (attack phase) */
        if(body->m_x < 1.f)
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * body->m_x);
            /* */
            body->m_x += m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
            /* */
            nextSegment(_voiceId);
        }
        break;
    case 2:
        /* exponential, quasi-finite rendering (release phase) */
        if(body->m_y > dsp_render_min)
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * (1.f - body->m_y));
            /* */
            body->m_y *= 1.f - m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
            /* */
            nextSegment(_voiceId);
        }
        break;
    }
}

/* */
void env_object_gate::nextSegment(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[body->m_index].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[body->m_index].m_dx[_voiceId];
    body->m_start_magnitude = body->m_signal_magnitude;
    /* */
    body->m_state = m_segment[body->m_next].m_state;
    body->m_index = body->m_next;
    body->m_next = m_segment[body->m_index].m_next;
}

/* */
void env_object_gate::setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value)
{
    /* */
    m_segment[_segmentId].m_dx[_voiceId] = _value;
}

/* */
void env_object_gate::setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const float _magnitude)
{
    /* */
    m_segment[_segmentId].m_dest_magnitude[_voiceId] = _magnitude;
}

/**************************************             **************************************/

/* */
void env_object_decay::start(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[m_startIndex].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[m_startIndex].m_dx[_voiceId];
    body->m_start_magnitude = body->m_signal_magnitude;
    /* */
    body->m_state = m_segment[m_startIndex].m_state;
    body->m_index = m_startIndex;
    body->m_next = m_segment[m_startIndex].m_next;
}

/* */
void env_object_decay::tick(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    const uint32_t segment = body->m_index;
    const float diff_magnitude = m_segment[segment].m_dest_magnitude[_voiceId] - body->m_start_magnitude;
    /* */
    switch(body->m_state)
    {
    case 0:
        /* idle */
        break;
    case 1:
        /* linear rendering (attack phase) */
        if(body->m_x < 1.f)
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * body->m_x);
            /* */
            body->m_x += m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
            /* */
            nextSegment(_voiceId);
        }
        break;
    case 2:
        /* exponential, quasi-finite rendering (release phase) */
        if(body->m_y > dsp_render_min)
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * (1.f - body->m_y));
            /* */
            body->m_y *= 1.f - m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
            /* */
            nextSegment(_voiceId);
        }
        break;
    }
}

/* */
void env_object_decay::nextSegment(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[body->m_index].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[body->m_index].m_dx[_voiceId];
    body->m_start_magnitude = body->m_signal_magnitude;
    /* */
    body->m_state = m_segment[body->m_next].m_state;
    body->m_index = body->m_next;
    body->m_next = m_segment[body->m_index].m_next;
}

/* */
void env_object_decay::setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value)
{
    /* */
    m_segment[_segmentId].m_dx[_voiceId] = _value;
}

/* */
void env_object_decay::setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const float _magnitude)
{
    /* */
    m_segment[_segmentId].m_dest_magnitude[_voiceId] = _magnitude;
}

/**************************************             **************************************/

/* */
void env_engine2::init(const uint32_t _voices)
{
    /* */
    for(uint32_t v = 0; v < _voices; v++)
    {
        m_env_g.m_segment[1].m_dx[v] = 1.f;
        m_env_f.m_segment[1].m_dx[v] = 1.f;
    }
}

/* */
void env_engine2::tickMono()
{
    /* */
    m_env_f.tick(0);
}

/* */
void env_engine2::tickPoly(const uint32_t _voiceId)
{
    /* */
    m_env_a.tick(_voiceId);
    m_env_b.tick(_voiceId);
    m_env_c.tick(_voiceId);
    m_env_g.tick(_voiceId);
}
