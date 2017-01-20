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
#include "nltoolbox.h"

class Oscillator
{
public:
    Oscillator();                   // Default Constructor

    Oscillator(uint32_t _samplerate,     // Parameterized Contructor
               float _oscFreq,
               float _oscPhase,
               float _modPhase,
               float _fluctAmnt,
               float _chirpFreq);

    ~Oscillator(){}                 // Class Destructor

    float applyOscillator();

    void setOscFreq(float _oscFreq);

    void resetPhase(float _phase);
    void setModPhase(float _modPhase);

    void calcInc();

    void setFluctuation(float _oscFluctAmnt);
    void setSeed(uint32_t _voiceNumber);
    float calcRandVal();

    void setChirpFreq(float _chirpFrequency);


private:

    float mSampleRate;              // Samplerate
    float mOscFreq;                 // Oscillator Frequnecy

    float mOscPhase;                // Oscillator Phase
    float mModPhase;                // Modulated phase Radians
    float mPhaseStateVar;           // Phase state Variable

    float mPhaseInc;                // Phase increment

    float mFluctAmnt;               // Fluctuation Amount
    int32_t mRandValStateVar;       // Random value state variable

    NlToolbox::Filters::ChirpFilter mChirpFilter;   // Chirp Filter instance

    inline float oscSinP3(float _currPhase);        // this function should probably be in a toolbox as independant module
                                                    // careful, this one is a bit different than sinP3(), that already is
                                                    // part of the toolbox family

};
