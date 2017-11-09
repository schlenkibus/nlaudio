/******************************************************************************/
/** @file		soundgenerator.h
    @date		2017-05-11
    @version	1.0
    @author		Anton Schmied[2016-07-20]
    @brief		An implementation of the sound generating Class with two
                oscillators and two shapers as used in the C15 and implemented
                in Reaktor
    @note       This class is implemented for one voice only
                could think about an array aproach for all voices
                within the class ...

    @todo       Feedback, Envelope Influence
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "nlglobaldefines.h"

class Soundgenerator
{
public:
    Soundgenerator();                                               // Default Constructor

    Soundgenerator(float _pitchOffset_A, float _pitchOffset_B,
                   float _keyTracking_A, float _keyTracking_B,
                   float _fluct_A, float _fluct_B,
                   float _phase_A, float _phase_B,
                   float _pmSelf_A, float _pmSelf_B,
                   float _pmSelfShaper_A, float _pmSelfShaper_B,
                   float _pmCross_A, float _pmCross_B,
                   float _pmCrossShaper_A, float _pmCrossShaper_B,
                   float _chirpPitch_A, float _chirpPitch_B,
                   float _pmFeedback_A, float _pmFeedback_B,
//                   float _pitchEnvC_A, float _pitchEnvC_B,
//                   float _fluctEnvC_A, float _fluctEnvC_B,
                   float _pmSelfEnvA_A, float _pmSelfEnvB_B,
                   float _pmCrossEnvB_A, float _pmCrossEnvA_B,
                   float _pmFeedbackEnvC_A, float _pmFeedbackEnvC_B,
                   float _drive_A, float _drive_B,
                   float _fold_A, float _fold_B,
                   float _asym_A, float _asym_B,
                   float _shaperMix_A, float _shaperMix_B,
                   float _feedbackMix_A, float _feedbackMix_B,
                   float _ringMod_A, float _ringMod_B,
                   float _driveEnvA_A, float _driveEnvB_B,
                   float _feedbackMixEnvC_A, float _feedbackMixEnvC_B);

    ~Soundgenerator();                     // Destructor

    float mSampleA, mSampleB;              // Generated Samples

    void generateSound(float _feedbackSample, float _envRamp_A, float _envRamp_B, float _envRamp_C, float _gateRamp);
    void setGenParams(unsigned char _instrID, unsigned char _ctrlID, float _ctrlVal);

    void setPitch(float _pitch);
    void setVoiceNumber(uint32_t _voiceNumber);
    void resetPhase();

private:
    float mPitch;                          // Pitch of the played note/ key


    //*********************** Oscillator A Variables **************************//

    float mModuleA_PitchOffset;             // Oscillator Pitch Offset
    float mModuleA_Fluct;                   // Fluctuation Amount
    float mModuleA_PmSelf;                  // Self Phase Modulation Amount, Oscillator Feedback
    float mModuleA_PmCross;                 // Cross Phase Modulation Amount, Oscilltor Feedback
    float mModuleA_PmFeedback;

    float mModuleA_KeyTracking;             // Oscialltor Key Tracking Amount
    float mModuleA_ResetPhase;              // Oscillator Reset Phase
    float mModuleA_PmSelfShaper;            // Self Phase Modulation Amount, Shaper Feedback
    float mModuleA_PmCrossShaper;           // Cross Phase Modulation Amount, Shaper Feedback

    NlToolbox::Filters::ChirpFilter* pModuleA_ChirpFilter;   // Chirp Filter instance

//    float mModuleA_Pitch_EnvC_Amnt;         // Envelope C Modulation Amount on Pitch Offset
//    float mModuleA_Fluct_EnvC_Amnt;         // Envelope C Modulation Amount on Fluctuation
    float mModuleA_PmSelf_EnvA_Amnt;        // Envelope A Modulation Amount on PM Self
    float mModuleA_PmCross_EnvB_Amnt;       // Envelope B Modulation Amount on PM Cross
    float mModuleA_PmFeedback_EnvC_Amnt;    // Envelope C Modulation Amount on PM Feedback

    float mModuleA_OscFreq;                 // Oscillator Frequency, dependant on Key pitch, Keytracking Amount and Pitch Offset
    float mModuleA_OscPhase;                // Oscillator Phase
    float mModuleA_PhaseStateVar;           // Phase State Variable
    float mModuleA_PhaseInc;                // Phase Increment, dependant on Osc Frequency, Fluctuation Amount and Randam Value

    int32_t mModuleA_RandVal;               // Random Value State Variable, set by the active Voice Number

    float mModuleA_SelfMix;                 // Mix between the Modules own Osciallator and Shaper Samples
    float mModuleA_CrossMix;                // Mix between the Modules own Osciallator Sample and the opposite Modules Shaper Sample


    //************************* Shaper A Variables ****************************//

    float mModuleA_Drive;                   // Shaper Drive Control
    float mModuleA_Fold;                    // Shaper Fold Control
    float mModuleA_Asym;                    // Shaper Asymmetry Control

    float mModuleA_ShaperMix;               // Mix Amount between Oscillator and Shaper
    float mModuleA_FeedbackMix;             // Feedback Mix Level
    float mModuleA_RingMod;                 // Ring Modulation Amount

    float mModuleA_Drive_EnvA_Amnt;         // Envelope A Modulation Amount
    float mModuleA_FeedbackMix_EnvC_Amnt;   // Envelope C Modulation Amount


    //*********************** Oscillator B Variables **************************//

    float mModuleB_PitchOffset;
    float mModuleB_Fluct;
    float mModuleB_PmSelf;
    float mModuleB_PmCross;
    float mModuleB_PmFeedback;

    float mModuleB_KeyTracking;
    float mModuleB_ResetPhase;
    float mModuleB_PmSelfShaper;
    float mModuleB_PmCrossShaper;

    NlToolbox::Filters::ChirpFilter* pModuleB_ChirpFilter;

//    float mModuleB_Pitch_EnvC_Amnt;         // Envelope C Modulation Amount on Pitch Offset
//    float mModuleB_Fluct_EnvC_Amnt;         // Envelope C Modulation Amount on Fluctuation
    float mModuleB_PmSelf_EnvB_Amnt;        // Envelope B Modulation Amount on PM Self
    float mModuleB_PmCross_EnvA_Amnt;       // Envelope A Modulation Amount on PM Cross
    float mModuleB_PmFeedback_EnvC_Amnt;    // Envelope C Modulation Amount on PM Feedback

    float mModuleB_OscFreq;
    float mModuleB_OscPhase;
    float mModuleB_PhaseStateVar;
    float mModuleB_PhaseInc;

    int32_t mModuleB_RandVal;

    float mModuleB_SelfMix;
    float mModuleB_CrossMix;


    //************************* Shaper B Variables ****************************//

    float mModuleB_Drive;
    float mModuleB_Fold;
    float mModuleB_Asym;

    float mModuleB_ShaperMix;
    float mModuleB_FeedbackMix;
    float mModuleB_RingMod;

    float mModuleB_Drive_EnvB_Amnt;         // Envelope B Modulation Amount
    float mModuleB_FeedbackMix_EnvC_Amnt;   // Envelope C Modulation Amount


    //************************** Smoothing Variabels **************************//

    inline void applyASmoother();
    inline void applyBSmoother();

    uint32_t mModuleA_SmootherMask      , mModuleB_SmootherMask;

    // Mask ID: 1
    float mModuleA_PmSelf_base          , mModuleB_PmSelf_base;
    float mModuleA_PmSelf_target        , mModuleB_PmSelf_target;
    float mModuleA_PmSelf_diff          , mModuleB_PmSelf_diff;
    float mModuleA_PmSelf_ramp          , mModuleB_PmSelf_ramp;

    // Mask ID: 2
    float mModuleA_PmSelfShaper_base    , mModuleB_PmSelfShaper_base;
    float mModuleA_PmSelfShaper_target  , mModuleB_PmSelfShaper_target;
    float mModuleA_PmSelfShaper_diff    , mModuleB_PmSelfShaper_diff;
    float mModuleA_PmSelfShaper_ramp    , mModuleB_PmSelfShaper_ramp;

    // Mask ID: 3
    float mModuleA_PmCross_base         , mModuleB_PmCross_base;
    float mModuleA_PmCross_target       , mModuleB_PmCross_target;
    float mModuleA_PmCross_diff         , mModuleB_PmCross_diff;
    float mModuleA_PmCross_ramp         , mModuleB_PmCross_ramp;

    // Mask ID: 4
    float mModuleA_PmCrossShaper_base   , mModuleB_PmCrossShaper_base;
    float mModuleA_PmCrossShaper_target , mModuleB_PmCrossShaper_target;
    float mModuleA_PmCrossShaper_diff   , mModuleB_PmCrossShaper_diff;
    float mModuleA_PmCrossShaper_ramp   , mModuleB_PmCrossShaper_ramp;

    // Mask ID: 5
    float mModuleA_Drive_base           , mModuleB_Drive_base;
    float mModuleA_Drive_target         , mModuleB_Drive_target;
    float mModuleA_Drive_diff           , mModuleB_Drive_diff;
    float mModuleA_Drive_ramp           , mModuleB_Drive_ramp;

    // Mask ID: 6
    float mModuleA_Fold_base            , mModuleB_Fold_base;
    float mModuleA_Fold_target          , mModuleB_Fold_target;
    float mModuleA_Fold_diff            , mModuleB_Fold_diff;
    float mModuleA_Fold_ramp            , mModuleB_Fold_ramp;

    // Mask ID: 7
    float mModuleA_Asym_base            , mModuleB_Asym_base;
    float mModuleA_Asym_target          , mModuleB_Asym_target;
    float mModuleA_Asym_diff            , mModuleB_Asym_diff;
    float mModuleA_Asym_ramp            , mModuleB_Asym_ramp;

    // Mask ID: 8
    float mModuleA_ShaperMix_base       , mModuleB_ShaperMix_base;
    float mModuleA_ShaperMix_target     , mModuleB_ShaperMix_target;
    float mModuleA_ShaperMix_diff       , mModuleB_ShaperMix_diff;
    float mModuleA_ShaperMix_ramp       , mModuleB_ShaperMix_ramp;

    // Mask ID: 9
    float mModuleA_RingMod_base         , mModuleB_RingMod_base;
    float mModuleA_RingMod_target       , mModuleB_RingMod_target;
    float mModuleA_RingMod_diff         , mModuleB_RingMod_diff;
    float mModuleA_RingMod_ramp         , mModuleB_RingMod_ramp;

    // Mask ID: 10
    float mModuleA_FeedbackMix_base     , mModuleB_FeedbackMix_base;
    float mModuleA_FeedbackMix_target   , mModuleB_FeedbackMix_target;
    float mModuleA_FeedbackMix_diff     , mModuleB_FeedbackMix_diff;
    float mModuleA_FeedbackMix_ramp     , mModuleB_FeedbackMix_ramp;

    // Mask ID: 11
    float mModuleA_PmFeedback_base      , mModuleB_PmFeedback_base;
    float mModuleA_PmFeedback_target    , mModuleB_PmFeedback_target;
    float mModuleA_PmFeedback_diff      , mModuleB_PmFeedback_diff;
    float mModuleA_PmFeedback_ramp      , mModuleB_PmFeedback_ramp;

    // Mask ID: 14
    float mModuleA_PmSelf_EnvA_Amnt_base    , mModuleB_PmSelf_EnvB_Amnt_base;
    float mModuleA_PmSelf_EnvA_Amnt_target  , mModuleB_PmSelf_EnvB_Amnt_target;
    float mModuleA_PmSelf_EnvA_Amnt_diff    , mModuleB_PmSelf_EnvB_Amnt_diff;
    float mModuleA_PmSelf_EnvA_Amnt_ramp    , mModuleB_PmSelf_EnvB_Amnt_ramp;

    // Mask ID: 15
    float mModuleA_PmCross_EnvB_Amnt_base   , mModuleB_PmCross_EnvA_Amnt_base;
    float mModuleA_PmCross_EnvB_Amnt_target , mModuleB_PmCross_EnvA_Amnt_target;
    float mModuleA_PmCross_EnvB_Amnt_diff   , mModuleB_PmCross_EnvA_Amnt_diff;
    float mModuleA_PmCross_EnvB_Amnt_ramp   , mModuleB_PmCross_EnvA_Amnt_ramp;

    // Mask ID: 16
    float mModuleA_PmFeedback_EnvC_Amnt_base   , mModuleB_PmFeedback_EnvC_Amnt_base;
    float mModuleA_PmFeedback_EnvC_Amnt_target , mModuleB_PmFeedback_EnvC_Amnt_target;
    float mModuleA_PmFeedback_EnvC_Amnt_diff   , mModuleB_PmFeedback_EnvC_Amnt_diff;
    float mModuleA_PmFeedback_EnvC_Amnt_ramp   , mModuleB_PmFeedback_EnvC_Amnt_ramp;

    // Mask ID: 17
    float mModuleA_Drive_EnvA_Amnt_base     , mModuleB_Drive_EnvB_Amnt_base;
    float mModuleA_Drive_EnvA_Amnt_target   , mModuleB_Drive_EnvB_Amnt_target;
    float mModuleA_Drive_EnvA_Amnt_diff     , mModuleB_Drive_EnvB_Amnt_diff;
    float mModuleA_Drive_EnvA_Amnt_ramp     , mModuleB_Drive_EnvB_Amnt_ramp;

    // Mask ID: 18
    float mModuleA_FeedbackMix_EnvC_Amnt_base   , mModuleB_FeedbackMix_EnvC_Amnt_base;
    float mModuleA_FeedbackMix_EnvC_Amnt_target , mModuleB_FeedbackMix_EnvC_Amnt_target;
    float mModuleA_FeedbackMix_EnvC_Amnt_diff   , mModuleB_FeedbackMix_EnvC_Amnt_diff;
    float mModuleA_FeedbackMix_EnvC_Amnt_ramp   , mModuleB_FeedbackMix_EnvC_Amnt_ramp;

    //*************************** Internal Functions **************************//

    inline float setOscFreq(float _pitch, float _keyTracking, float _pitchOffset);
    inline float setPhaseInc(float _oscFreq, float _fluctAmnt, int32_t& _randVal);


    //******************************* Module IDs ******************************//

    enum InstrID: unsigned char
    {
        OSC_A       = 0xB1,
        OSC_B       = 0xB2,
        SHAPER_A    = 0xB3,
        SHAPER_B    = 0xB4
    };


    //**************************** OSC Controls IDs ***************************//

    enum OscCtrlID: unsigned char
    {
#ifdef REMOTE61                     // novation ReMOTE61
        OFFSETPITCH     = 0x15,
        FLUCT           = 0x16,
        PMSELF          = 0x17,     // PM Self
        PMCROSS         = 0x18,     // PM B / PM A
        PMFEEDBACK      = 0x19,     // PM FB

        PITCH_ENV       = 0x1F,
        FLUCT_ENV       = 0x20,
        PMSELF_ENV      = 0x21,     // ENV A/B
        PMCROSS_ENV     = 0x22,     // EBV B/A
        PMFEEDBACK_ENV  = 0x23,

        KEYTRACKING     = 0x29,
        PHASE           = 0x2A,
        PMSELFSHAPER    = 0x2B,     // PM Shaper Self  A/B
        PMCROSSSHAPER   = 0x2C,     // PM Shaper Cross B/A
        CHIRPFREQ       = 0x2D
#else
        OFFSETPITCH,
        FLUCT,
        PMSELF,
        PMCROSS,
        PMFEEDBCK,

        PITCH_ENV,
        FLUCT_ENV,
        PMSELF_ENV,
        PMCROSS_ENV,
        PMFEEDBACK_ENV,

        KEYTRACKING,
        PHASE,
        PMSELFSHAPER,
        PMCROSSSHAPER,
        CHIRPFREQ
#endif
    };


    //************************** Shaper Controls IDs **************************//

    enum ShaperCtrlID: unsigned char
    {
#ifdef REMOTE61
        DRIVE        = 0x15,
        FOLD         = 0x16,
        ASYM         = 0x17,
        SHAPER_MIX   = 0x18,
        FEEDBACK_MIX = 0x19,
        RING_MOD     = 0x1A,

        DRIVE_ENV   = 0x1F,  //Env A/B
        FBMIX_ENV   = 0x23
#else
        DRIVE,
        FOLD,
        ASYM,
        SHAPERMIX,
        FEEDBACK_MIX,
        RINGMOD,

        DRIVE_ENV,
        FBMIX_ENV
#endif
    };
};
