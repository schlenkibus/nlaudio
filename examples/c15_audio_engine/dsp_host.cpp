#include <iostream>
#include "dsp_host.h"

dsp_host::dsp_host()
{
    m_mainOut_R = 0.f;
    m_mainOut_L = 0.f;

    m_paramsignaldata[dsp_number_of_voices][sig_number_of_params] = {};
}

// Matthias: new proper init
void dsp_host::init(unsigned int _samplerate, unsigned int _polyphony)
{
    m_samplerate = _samplerate;
    m_voices = _polyphony;

    std::cout << "DSP_HOST::INIT(samplerate: " << m_samplerate << ", voices: " << m_voices << ")" << std::endl;
}

void dsp_host::tickMain()
{

}


void dsp_host::evalMidi()
{

}
