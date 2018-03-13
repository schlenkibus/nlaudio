/******************************************************************************/
/** @file		dsp_host.h
    @date
    @version
    @author
    @brief		main container object holding param and audio engine
                (including shared rendering signals, tcd decoder)
    @todo
*******************************************************************************/

#pragma once

#include <stdint.h>
#include "paramengine.h"
#include "tcd_decoder.h"

/******************************************************************************/
/** Global Defines - potentially temporal
*******************************************************************************/
const uint32_t NUM_V = 20;
const uint32_t NUM_P = 1;

class dsp_host
{
public:
    dsp_host();         //default Constructor

    void setSamplerate(unsigned int _samplerate);       // gets Samplerate from Pascals Jobhandle (good idea?)
    // better: init function (samplerate, num_voices); array would be initialized to MAX_VOICES (always 20)
    void tickMain();
    void evalMidi();

    /* first local variables */
    unsigned int m_samplerate;
    float m_paramsignaldata[NUM_V][NUM_P];
    /* main signal output (left, right) */
    float m_mainOut_R, m_mainOut_L;
    /* local data structures */
    decoder m_decoder;
};
