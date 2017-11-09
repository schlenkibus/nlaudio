/******************************************************************************/
/** @file		onesoundgenerator.h
    @date		2016-12-01
    @version	0.1
    @author		Anton Schmied[2016-12-01]
    @brief		Alternative implementation of the sound generating Class
                This solution shuld work with a single generator and twelve for
                each voice
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "oscillator.h"

class OneSoundgenerator
{
public:
    OneSoundgenerator();                        // Default Constructor

    OneSoundgenerator(uint32_t _sampleRate,     // Parameterized Constructor
                    float _phase,
                    float _pitchOffset,
                    float _keyTracking,
                    float _mainMixAmount);

    ~OneSoundgenerator(){}                      // Destructor

    // we need 12 of each ...
    float mPitch[NUM_VOICES];
    float mSampleA[NUM_VOICES], mSampleB[NUM_VOICES];

    void generateSound();

    void setPitch(float _pitch, uint32_t _voiceNumber);
    void resetPhase(uint32_t _voiceNumber);

    void setGenParams(unsigned char _instrID, unsigned char _ctrlID, float _ctrlVal);

private:
    float mSampleRate;              // Samplerate

    struct Generatormodules         // Struct for shared Parameters of both Oscillators and Shapers
    {
        Oscillator mOsc[NUM_VOICES];    // 12 Oscillators, for each voice

        float mDrive;
        float mFold;
        float mAsym;

        float mPhase;                   // Oscillator phase
        float mPitchOffset;             // Oscillator pitch offset
        float mKeyTracking;             // Oscialltor key tracking amount

        float mPmSelf;                  // Self Phase Modulation, Oscillator Feedback
        float mPmCross;                 // Cross Phase Modulation, Oscialltor Feedback
        float mPmSelfShaper;            // Self Phase Modulation Amount, Shaper Feedback
        float mPmCrossShaper;           // Cross Phase Modulation Amount, Shaper Feedback

        float mShaperMixAmount;         // Mix Amount between Oscillator and Shaper
        float mRingMod;                 // Ring modulation amount

        float mSelfMix[NUM_VOICES];     // Mix between modules own osciallator and shaper samples
        float mCrossMix[NUM_VOICES];    // Mix between the modules own osciallator sample and the opposite modules shaper samples

    } moduleA, moduleB;


    enum CtrlID: unsigned char      // enums for control IDs
    {
#ifdef REMOTE61                     // novation ReMOTE61
        OFFSETPITCH  = 0x15,
        KEYTRACKING  = 0x16,
        PHASE        = 0x17,
        FLUCT        = 0x18,
        CHIRPFREQ    = 0x26,

        PMSELF       = 0x1A,        // PM Self
        PMCROSS      = 0x1B,        // PM B / PM A
        PMFEEDBCK    = 0x1C,        // PM FB
        PMSELFSHAPER = 0x24,        // PM Shaper Self A
        PMCROSSSHAPER= 0x25,        // PM Shaper B

        DRIVE        = 0x29,
        MAINMIX      = 0x2A,
        RING         = 0x2C,
        FOLD         = 0x2D,
        ASYM         = 0x2E,
#else
        OFFSETPITCH,
        KEYTRACKING,
        PHASE,
        FLUCT,
        CHIRPFREQ,

        PMSELF,        //PM Self
        PMCROSS,        //PM B / PM A
        PMFEEDBCK,        //PM FB
        PMSELFSHAPER,        //PM Shaper Self A
        PMCROSSSHAPER,        //PM Shaper B

        DRIVE,
        MAINMIX,
        RING,
        FOLD,
        ASYM,
#endif
    };

    enum InstrID: unsigned char
    {
        MODULE_A     = 0xB0,
        MODULE_B     = 0xB1
    };

    inline float calcOscFrequency(float _pitch, float _keyTracking, float _pitchOffset);
};
