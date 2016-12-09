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
#include "shaper.h"


class OneSoundgenerator
{
public:
    OneSoundgenerator();                        // Default Constructor

    OneSoundgenerator(int _sampleRate,          // Parameterized Constructor
                    float _phase,
                    float _pitchOffset,
                    float _keyTracking,
                    float _mainMixAmount);

    ~OneSoundgenerator(){}                      // Destructor

    // we need 12 of each ...
    float mPitch[12];
    float mSampleA[12], mSampleB[12];

    void generateSound();

    void setPitch(float _pitch, unsigned int _voiceNumber);
    void resetPhase(unsigned int _voiceNumber);
//    void setVoiceNumer();           // Eigentlich kann das in dem Oscillator passieren ... Zumindest in dieser Version, oder was?

    void setGenParams(unsigned char _instrID, unsigned char _ctrlID, float _ctrlVal);

private:
    float mSampleRate;              // Samplerate

    struct Generatormodules         // Struct for shared Parameters of both Oscillators and Shapers
    {
        Oscillator mOsc[12];        // 12 Oscillators, for each voice
        Shaper mShaper;             // Shaper

        float mPhase;               // Oscillator phase
        float mPitchOffset;         // Oscillator pitch offset
        float mKeyTracking;         // Oscialltor key tracking amount

        float mPmSelf;              // Self Phase Modulation, Oscillator Feedback
        float mPmCross;             // Cross Phase Modulation, Oscialltor Feedback
        float mPmSelfShaper;        // Self Phase Modulation Amount, Shaper Feedback
        float mPmCrossShaper;       // Cross Phase Modulation Amount, Shaper Feedback

        float mShaperMixAmount;     // Mix Amount between Oscillator and Shaper
        float mRingMod;             // Ring modulation amount

        float mSelfMix[12];         // Mix between modules own osciallator and shaper samples
        float mCrossMix[12];        // Mix between the modules own osciallator sample and the opposite modules shaper samples

    } moduleA, moduleB;


    enum CtrlID: unsigned char
    {
        // enums for control IDs novation ReMOTE61

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
    };

    enum InstrID: unsigned char
    {
        MODULE_A     = 0xB0,
        MODULE_B     = 0xB1
    };

    inline float calcOscFrequency(float _pitch, float _keyTracking, float _pitchOffset);
};
