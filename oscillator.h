/******************************************************************************/
/** @file		oscillator.h
    @date		2016-06-28
    @version	0.1
    @author		Anton Schmied[2016-07-05]
    @brief		An implementation of an Oscillator Class
                as used in the C15 and implemented in Reaktor
*******************************************************************************/

#pragma once
#include "tools.h"
#include "toolbox.h"

class Oscillator
{
public:
    Oscillator();                   // Default Constructor

    Oscillator(int _samplerate,     // Parameterized Contructor
               float _oscPhase,
               float _fluctAmnt,
               float _modRadians);

    ~Oscillator(){}                 // Class Destructor

    float applyOscillator(float _oscFreq);

    float applyChirpFilter(float _input);

    float randomize();
    float calcRandVal();

    void setOscFrequency(float _oscFreq);
    void setFluctuation(float _oscFluctAmnt);
    void setModulationRadians(float _modRadians);

    void setSeed(signed int _voiceNumber);
    void setupChirpFilter(float _chirpFrequency);

    void resetPhase(float _phase);


private:

    float mSampleRate;              // Samplerate
    float mOscPhase;                // Oscillator Phase
    float mFluctAmnt;               // Fluctuation Amount
    float mModRadians;              // Modulated phase Radians

    float mPhaseInc;                // Phase increment
    float mPhaseStateVar;           // Phase state Variable

    bool mEdge;                     // Phase Edge
    signed int mRandValStateVar;    // Random value state variable

    NlToolbox::Filters::ChirpFilter mChirpFilter;

    // Random Module


    /// these functions should be in a toolbox as independant modules!
    inline float oscSinP3(float x);                 // careful, this one is a bit different than the rest

};
