#include "dsp_host.h"

dsp_host::dsp_host()
{
    m_mainOut_R = 0.f;
    m_mainOut_L = 0.f;

    m_paramsignaldata[NUM_V][NUM_P] = {};
}



void dsp_host::setSamplerate(unsigned int _samplerate)
{
    m_samplerate = _samplerate;
    // add number of voices!
}


void dsp_host::tickMain()
{

}


void dsp_host::evalMidi()
{

}
