#pragma once

#include "tools.h"
#include "toolbox.h"
#include "oscillator.h"

class Phase22
{
public:
    Phase22();                      // Default Constructor

    Phase22(int _sampleRate,        // Parameterized Constructor
            float _phase,
            float _gain,
            float _pitchOffset,
            float _keyTracking,
            float _mainMixAmoount,
            float _drive,
            float _fold,
            float _asym);

    ~Phase22(){}                    // Destructor


    void setPitch(float _pitch);
    void setVoiceNumber(unsigned int _voiceNumber);
    void setOscParams(unsigned char _instrID, unsigned char _ctrlID, float _ctrlVal);
    void resetPhase();

    float makeNoise();
    float applyShaper(float _currSample, float _drive, float _fold, float _asym);


private:

    float mSampleRate;              // Samplerate
    float mPitch;                   // Pitch of the played note/ key

    struct Phase22modules           // Struct for shared Parameters of both Oscillators and Shapers
    {
        float mGain;                // Module gain
        float mMainMixAmount;       // Mix Amount between Oscillator and Shaper

        Oscillator mOsc;            // Osciallator

        float mPhase;               // Oscillator phase
        float mFrequency;           // Oscillator frequency
        float mPitchOffset;         // Oscillator pitch offset
        float mKeyTracking;         // Oscialltor key tracking amount

        float mDrive;               // Shaper drive amount
        float mFold;                // Shaper fold amount
        float mAsym;                // Shaper asymmetry amount

        float mPmSelf;              // Self Phase Modulation, Oscillator Feedback
        float mPmCross;             // Cross Phase Modulation, Oscialltor Feedback
        float mPmSelfAmount;        // Self Phase Modulation Amount, Shaper Feedback
        float mPmCrossAmount;       // Cross Phase Modulation Amount, Shaper Feedback

        float mSelfMix;             // Resulting Self Phase Modulation Mix
        float mCrossMix;            // Resulting Cross Phase Modulation Mix

    } moduleA, moduleB;


    enum CtrlID: unsigned char  // enums for control IDs novation ReMOTE61
    {
        OFFSETPITCH  = 0x15,
        KEYTRACKING  = 0x16,
        PHASE        = 0x17,
        FLUCT        = 0x18,
        CHIRPFREQ    = 0x26,

        PMSELF       = 0x1A,        //PM Self
        PMCROSS      = 0x1B,        //PM B
        PMFEEDBCK    = 0x1C,        //PM FB
        PMSELFAMNT   = 0x24,        //PM Shaper Self A
        PMCROSSAMNT  = 0x25,        //PM Shaper B

        DRIVE        = 0x29,
        MAINMIX      = 0x2A,
        FOLD         = 0x2D,
        ASYM         = 0x2E,

        GAIN         = 0x2F          //Gain
    };

    enum InstrID: unsigned char
    {
        MODULE_A     = 0xB0,
        MODULE_B     = 0xB1
    };

    inline float calcOscFrequency(float _pitch, float _keyTracking, float _pitchOffset);
};
