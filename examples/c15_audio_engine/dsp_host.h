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

/* dsp_host: main dsp object, holding TCD Decoder, Parameter Engine, Audio Engine, shared Signal Array, main signal (L, R) */
class dsp_host
{
public:
    dsp_host();                                     // default Constructor (init function does the job actually, see below)
    /* first local variables */
    uint32_t m_samplerate;                          // project sample rate is given at program startup by commandline
    uint32_t m_voices;                              // project polyphony is given at program startup by commandline
    /* hosting shared param signal array */
    float m_paramsignaldata[dsp_number_of_voices][sig_number_of_params];
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
