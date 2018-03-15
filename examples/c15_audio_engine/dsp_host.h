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
const uint32_t NUM_V = 20;  // maximal number of voices
const uint32_t NUM_P = 26;  // actual number of parameters (test project 1)

class dsp_host
{
public:
    dsp_host();         //default Constructor (init function does the job actually, see below)
    /* first local variables */
    unsigned int m_samplerate;
    unsigned int m_voices;
    /* hosting shared param signal array */
    float m_paramsignaldata[NUM_V][NUM_P];
    /* main signal output (left, right) */
    float m_mainOut_R, m_mainOut_L;
    /* local data structures */
    decoder m_decoder;
    /* proper init (samplerate & polyphony) */
    void init(unsigned int _samplerate, unsigned int _polyphony);
    /* the two main interaction methods: perform sample tick, eval MIDI message */
    void tickMain();
    void evalMidi();
};
