/******************************************************************************/
/** @file		soundgenerator.h
    @date		2016-07-20
    @version	0.2
    @author		Anton Schmied[2016-07-20]
    @brief		An implementation of the sound generating Class
                with two oscillators and two shapers
                as used in the C15 and implemented in Reaktor

    @note       Oder man macht ein ganzes array an werten und
                nur einen soundgenerator ... uuurgh ...
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "oscillator.h"
#include "shaper.h"

class Soundgenerator
{
public:
    Soundgenerator();                      // Default Constructor

    Soundgenerator(uint32_t _sampleRate,        // Parameterized Constructor
                    float _phase,
                    float _pitchOffset,
                    float _keyTracking,
                    float _mainMixAmount);

    ~Soundgenerator(){}                    // Destructor

    float mPitch;                          // Pitch of the played note/ key
    float mSampleA, mSampleB;              // Generated Samples

    void generateSound();

    void setPitch(float _pitch);
    void setVoiceNumber(uint32_t _voiceNumber);
    void resetPhase();
    void setGenParams(unsigned char _instrID, unsigned char _ctrlID, float _ctrlVal);

private:

    float mSampleRate;              // Samplerate

    struct Generatormodules         // Struct for shared Parameters of both Oscillators and Shapers
    {
        float mShaperMixAmount;       // Mix Amount between Oscillator and Shaper

        Oscillator mOsc;            // Osciallator
        Shaper mShaper;             // Shaper

        float mPhase;               // Oscillator phase
        float mPitchOffset;         // Oscillator pitch offset
        float mKeyTracking;         // Oscialltor key tracking amount

        float mPmSelf;              // Self Phase Modulation, Oscillator Feedback
        float mPmCross;             // Cross Phase Modulation, Oscialltor Feedback
        float mPmSelfShaper;        // Self Phase Modulation Amount, Shaper Feedback
        float mPmCrossShaper;       // Cross Phase Modulation Amount, Shaper Feedback

        float mSelfMix;             // Mix between modules own osciallator and shaper samples
        float mCrossMix;            // Mix between the modules own osciallator sample and the opposite modules shaper samples

        float mRingMod;             // Ring modulation amount

    } moduleA, moduleB;


    enum CtrlID: unsigned char      // enums for control IDs
    {
#ifdef REMOTE61                     // novation ReMOTE61
        OFFSETPITCH  = 0x15,
        KEYTRACKING  = 0x16,
        PHASE        = 0x17,
        FLUCT        = 0x18,
        CHIRPFREQ    = 0x26,

        PMSELF       = 0x1A,        //PM Self
        PMCROSS      = 0x1B,        //PM B / PM A
        PMFEEDBCK    = 0x1C,        //PM FB
        PMSELFSHAPER = 0x24,        //PM Shaper Self A
        PMCROSSSHAPER= 0x25,        //PM Shaper B

        DRIVE        = 0x29,
        MAINMIX      = 0x2A,
        RING         = 0x2C,
        FOLD         = 0x2D,
        ASYM         = 0x2E,
#endif
    };

    enum InstrID: unsigned char
    {
        MODULE_A     = 0xB0,
        MODULE_B     = 0xB1
    };

    inline float calcOscFrequency(float _pitch, float _keyTracking, float _pitchOffset);
};
