/******************************************************************************/
/** @file		soundgenerator.cpp
    @date		2017-05-11
    @version	1.0
    @author		Anton Schmied[2016-07-20]
    @brief		Soundgenerator Class member and method definitions
*******************************************************************************/

#include "soundgenerator.h"

/******************************************************************************/
/** Soundgenerator Default Constructor
 * @brief    initialization of the modules local variabels with default values
 *           Note the presets are the same for A and B modules (Osc + Shaper)
 *           SampleRate:            48 kHz
 *           Pitch Offset:          60.f ct
 *           Key Tracking:          1.f
 *           Fluct:                 0.f
 *           Reset Phase:           0.f
 *           PM Self:               0.f
 *           PM Self Shaper:        0.f
 *           PM Cross:              0.f
 *           PM Cross Shaper:       0.f
 *           Chirp Pitch:           120.f
 *           Drive:                 10.f
 *           Fold:                  0.5f
 *           Asym:                  0.f
 *           Shaper Mix:            0.f
 *           Feedback Mx:           0.f
 *           Ring Modulation:       0.f
*******************************************************************************/

Soundgenerator::Soundgenerator()
{
    mPitch = 0.f;

    //******************************* Outputs ********************************//
    mSampleA = 0.f;
    mSampleB = 0.f;


    //**************************** Oscillator A *******************************//
    mModuleA_PitchOffset = 60.f;
    mModuleA_Fluct = 0.f;
    mModuleA_PmSelf = 0.f;
    mModuleA_PmCross = 0.f;
    mModuleA_PmFeedback = 0.f;

    mModuleA_KeyTracking = 1.f;
    mModuleA_ResetPhase = 0.f;
    mModuleA_PmSelfShaper = 0.f;
    mModuleA_PmCrossShaper = 0.f;

    pModuleA_ChirpFilter = new NlToolbox::Filters::ChirpFilter();

//    mModuleA_Pitch_EnvC_Amnt = 0.f;
//    mModuleA_Fluct_EnvC_Amnt = 0.f;
    mModuleA_PmSelf_EnvA_Amnt = 0.f;
    mModuleA_PmCross_EnvB_Amnt = 0.f;
    mModuleA_PmFeedback_EnvC_Amnt = 0.f;

    mModuleA_OscPhase = 0.f;
    mModuleA_OscFreq = 0.f;
    mModuleA_PhaseStateVar = 0.f;
    mModuleA_PhaseInc = 0.f;

    mModuleA_RandVal = 0;

    mModuleA_SelfMix = 0.f;
    mModuleA_CrossMix = 0.f;


    //****************************** Shaper A *********************************//
    mModuleA_Drive = 0.18f;
    mModuleA_Fold = 0.5f;
    mModuleA_Asym = 0.f;

    mModuleA_ShaperMix = 0.f;
    mModuleA_FeedbackMix = 0.f;
    mModuleA_RingMod = 0.f;

    mModuleA_Drive_EnvA_Amnt = 0.f;
    mModuleA_FeedbackMix_EnvC_Amnt = 0.f;

    mModuleA_SelfMix = 0.f;
    mModuleA_CrossMix = 0.f;


    //**************************** Oscillator B *******************************//
    mModuleB_PitchOffset = 60.f;
    mModuleB_Fluct = 0.f;
    mModuleB_PmSelf = 0.f;
    mModuleB_PmCross = 0.f;
    mModuleB_PmFeedback = 0.f;

    mModuleB_KeyTracking = 1.f;
    mModuleB_ResetPhase = 0.f;
    mModuleB_PmSelfShaper = 0.f;
    mModuleB_PmCrossShaper = 0.f;

    pModuleB_ChirpFilter = new NlToolbox::Filters::ChirpFilter();

//    mModuleB_Pitch_EnvC_Amnt = 0.f;
//    mModuleB_Fluct_EnvC_Amnt = 0.f;
    mModuleB_PmSelf_EnvB_Amnt = 0.f;
    mModuleB_PmCross_EnvA_Amnt = 0.f;
    mModuleB_PmFeedback_EnvC_Amnt = 0.f;

    mModuleB_OscFreq = 0.f;
    mModuleB_OscPhase = 0.f;
    mModuleB_PhaseStateVar = 0.f;
    mModuleB_PhaseInc = 0.f;

    mModuleB_RandVal = 0;

    mModuleB_SelfMix = 0.f;
    mModuleB_CrossMix = 0.f;


    //****************************** Shaper B *********************************//
    mModuleB_Drive = 0.18f;
    mModuleB_Fold = 0.5f;
    mModuleB_Asym = 0.f;

    mModuleB_ShaperMix = 0.f;
    mModuleB_RingMod = 0.f;
    mModuleB_FeedbackMix = 0.f;

    mModuleB_Drive_EnvB_Amnt = 0.f;
    mModuleB_FeedbackMix_EnvC_Amnt = 0.f;

    mModuleB_SelfMix = 0.f;
    mModuleB_CrossMix = 0.f;


    //****************************** Smoothing ********************************//
    mModuleA_SmootherMask = 0x00000000;

    mModuleA_PmSelf_ramp =  1.f;
    mModuleA_PmSelfShaper_ramp = 1.f;
    mModuleA_PmCross_ramp = 1.f;
    mModuleA_PmCrossShaper_ramp = 1.f;
    mModuleA_Drive_ramp = 1.f;
    mModuleA_Fold_ramp = 1.f;
    mModuleA_Asym_ramp = 1.f;
    mModuleA_ShaperMix_ramp = 1.f;
    mModuleA_FeedbackMix_ramp = 1.f;
    mModuleA_RingMod_ramp = 1.f;

    mModuleA_PmFeedback_ramp = 1.f;
    mModuleA_PmSelf_EnvA_Amnt_ramp = 1.f;
    mModuleA_PmCross_EnvB_Amnt_ramp = 1.f;
    mModuleA_PmFeedback_EnvC_Amnt_ramp = 1.f;

    mModuleA_Drive_EnvA_Amnt_ramp = 1.f;
    mModuleA_FeedbackMix_EnvC_Amnt_ramp = 1.f;

    mModuleB_SmootherMask = 0x00000000;

    mModuleB_PmSelf_ramp = 1.f;
    mModuleB_PmSelfShaper_ramp = 1.f;
    mModuleB_PmCross_ramp = 1.f;
    mModuleB_PmCrossShaper_ramp = 1.f;
    mModuleB_Drive_ramp = 1.f;
    mModuleB_Fold_ramp  = 1.f;
    mModuleB_Asym_ramp = 1.f;
    mModuleB_ShaperMix_ramp = 1.f;
    mModuleB_FeedbackMix_ramp = 1.f;
    mModuleB_RingMod_ramp = 1.f;

    mModuleB_PmFeedback_ramp = 1.f;
    mModuleB_PmSelf_EnvB_Amnt_ramp = 1.f;
    mModuleB_PmCross_EnvA_Amnt_ramp = 1.f;
    mModuleB_PmFeedback_EnvC_Amnt_ramp = 1.f;

    mModuleB_Drive_EnvB_Amnt_ramp = 1.f;
    mModuleB_FeedbackMix_EnvC_Amnt_ramp = 1.f;
}



/******************************************************************************/
/** Soundgenerator Parameterized Constructor
 *  @brief    initialization of the modules local variabels with custom
 *            parameters
*******************************************************************************/

Soundgenerator::Soundgenerator(float _pitchOffset_A, float _pitchOffset_B,
                               float _keyTracking_A, float _keyTracking_B,
                               float _fluct_A, float _fluct_B,
                               float _phase_A, float _phase_B,
                               float _pmSelf_A, float _pmSelf_B,
                               float _pmSelfShaper_A, float _pmSelfShaper_B,
                               float _pmCross_A, float _pmCross_B,
                               float _pmCrossShaper_A, float _pmCrossShaper_B,
                               float _chirpPitch_A, float _chirpPitch_B,
                               float _pmFeedback_A, float _pmFeedback_B,
//                               float _pitchEnvC_A, float _pitchEnvC_B,
//                               float _fluctEnvC_A, float _fluctEnvC_B,
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
                               float _feedbackMixEnvC_A, float _feedbackMixEnvC_B)
{
    mPitch = 0.f;

    //******************************* Outputs ********************************//
    mSampleA = 0.f;
    mSampleB = 0.f;


    //**************************** Oscillator A *******************************//
    mModuleA_PitchOffset = _pitchOffset_A;
    mModuleA_Fluct = _fluct_A;
    mModuleA_PmSelf = _pmSelf_A;
    mModuleA_PmCross = _pmCross_A;
    mModuleA_PmFeedback = _pmFeedback_A;

    mModuleA_KeyTracking = _keyTracking_A;
    mModuleA_ResetPhase = _phase_A;
    mModuleA_PmSelfShaper = _pmSelfShaper_A;
    mModuleA_PmCrossShaper = _pmCrossShaper_A;

    pModuleA_ChirpFilter = new NlToolbox::Filters::ChirpFilter(SAMPLERATE, NlToolbox::Conversion::pitch2freq(_chirpPitch_A - 1.5f));

//    mModuleA_Pitch_EnvC_Amnt = _pitchEnvC_A;
//    mModuleA_Fluct_EnvC_Amnt = _fluctEnvC_A;
    mModuleA_PmSelf_EnvA_Amnt = _pmSelfEnvA_A;
    mModuleA_PmCross_EnvB_Amnt = _pmCrossEnvB_A;
    mModuleA_PmFeedback_EnvC_Amnt = _pmFeedbackEnvC_A;

    mModuleA_OscPhase = 0.f;
    mModuleA_OscFreq = 0.f;
    mModuleA_PhaseStateVar = 0.f;
    mModuleA_PhaseInc = 0.f;

    mModuleA_RandVal = 0;

    mModuleA_SelfMix = 0.f;
    mModuleA_CrossMix = 0.f;


    //****************************** Shaper A *********************************//
    mModuleA_Drive = _drive_A;
    mModuleA_Fold = _fold_A;
    mModuleA_Asym = _asym_A;

    mModuleA_ShaperMix = _shaperMix_A;
    mModuleA_FeedbackMix = _feedbackMix_A;
    mModuleA_RingMod = _ringMod_A;

    mModuleA_Drive_EnvA_Amnt = _driveEnvA_A;
    mModuleA_FeedbackMix_EnvC_Amnt = _feedbackMixEnvC_A;

    mModuleA_SelfMix = 0.f;
    mModuleA_CrossMix = 0.f;


    //**************************** Oscillator B *******************************//
    mModuleB_PitchOffset = _pitchOffset_B;
    mModuleB_Fluct = _fluct_B;
    mModuleB_PmSelf = _pmSelf_B;
    mModuleB_PmCross = _pmCross_B;
    mModuleB_PmFeedback = _pmFeedback_B;

    mModuleB_KeyTracking = _keyTracking_B;
    mModuleB_ResetPhase = _phase_B;
    mModuleB_PmSelfShaper = _pmSelfShaper_B;
    mModuleB_PmCrossShaper = _pmCrossShaper_B;

    pModuleB_ChirpFilter = new NlToolbox::Filters::ChirpFilter(SAMPLERATE, NlToolbox::Conversion::pitch2freq(_chirpPitch_B - 1.5f));

//    mModuleB_Pitch_EnvC_Amnt = _pitchEnvC_B;
//    mModuleB_Fluct_EnvC_Amnt = _fluctEnvC_B;
    mModuleB_PmSelf_EnvB_Amnt = _pmSelfEnvB_B;
    mModuleB_PmCross_EnvA_Amnt = _pmCrossEnvA_B;
    mModuleB_PmFeedback_EnvC_Amnt = _pmFeedbackEnvC_B;

    mModuleB_OscFreq = 0.f;
    mModuleB_OscPhase = 0.f;
    mModuleB_PhaseStateVar = 0.f;
    mModuleB_PhaseInc = 0.f;

    mModuleB_RandVal = 0;

    mModuleB_SelfMix = 0.f;
    mModuleB_CrossMix = 0.f;


    //****************************** Shaper B *********************************//
    mModuleB_Drive = _drive_B;
    mModuleB_Fold = _fold_B;
    mModuleB_Asym = _asym_B;

    mModuleB_ShaperMix = _shaperMix_B;
    mModuleB_FeedbackMix = _feedbackMix_B;
    mModuleB_RingMod = _ringMod_B;

    mModuleB_Drive_EnvB_Amnt = _driveEnvB_B;
    mModuleB_FeedbackMix_EnvC_Amnt = _feedbackMixEnvC_B;

    mModuleB_SelfMix = 0.f;
    mModuleB_CrossMix = 0.f;


    //****************************** Smoothing ********************************//
    mModuleA_SmootherMask = 0x00000000;

    mModuleA_PmSelf_ramp =  1.f;
    mModuleA_PmSelfShaper_ramp = 1.f;
    mModuleA_PmCross_ramp = 1.f;
    mModuleA_PmCrossShaper_ramp = 1.f;
    mModuleA_Drive_ramp = 1.f;
    mModuleA_Fold_ramp = 1.f;
    mModuleA_Asym_ramp = 1.f;
    mModuleA_ShaperMix_ramp = 1.f;
    mModuleA_FeedbackMix_ramp = 1.f;
    mModuleA_RingMod_ramp = 1.f;

    mModuleA_PmFeedback_ramp = 1.f;
    mModuleA_PmSelf_EnvA_Amnt_ramp = 1.f;
    mModuleA_PmCross_EnvB_Amnt_ramp = 1.f;
    mModuleA_PmFeedback_EnvC_Amnt_ramp = 1.f;

    mModuleA_Drive_EnvA_Amnt_ramp = 1.f;
    mModuleA_FeedbackMix_EnvC_Amnt_ramp = 1.f;

    mModuleB_SmootherMask = 0x00000000;

    mModuleB_PmSelf_ramp = 1.f;
    mModuleB_PmSelfShaper_ramp = 1.f;
    mModuleB_PmCross_ramp = 1.f;
    mModuleB_PmCrossShaper_ramp = 1.f;
    mModuleB_Drive_ramp = 1.f;
    mModuleB_Fold_ramp  = 1.f;
    mModuleB_Asym_ramp = 1.f;
    mModuleB_ShaperMix_ramp = 1.f;
    mModuleB_FeedbackMix_ramp = 1.f;
    mModuleB_RingMod_ramp = 1.f;

    mModuleB_PmFeedback_ramp = 1.f;
    mModuleB_PmSelf_EnvB_Amnt_ramp = 1.f;
    mModuleB_PmCross_EnvA_Amnt_ramp = 1.f;
    mModuleB_PmFeedback_EnvC_Amnt_ramp = 1.f;

    mModuleB_Drive_EnvB_Amnt_ramp = 1.f;
    mModuleB_FeedbackMix_EnvC_Amnt_ramp = 1.f;
}



/******************************************************************************/
/** Soundgenerator Destructor
 * @brief
*******************************************************************************/

Soundgenerator::~Soundgenerator()
{
    delete pModuleA_ChirpFilter;
    delete pModuleB_ChirpFilter;
}



/******************************************************************************/
/** @brief    main function which calculates the modulated phase depending
 *            on mix amounts (self modulation, cross modulation, feedback modulation),
 *            calculates the samples of the 2 oscillators and 2 shapers
*******************************************************************************/

void Soundgenerator::generateSound(float _feedbackSample, float _envRamp_A, float _envRamp_B, float _envRamp_C, float _gateRamp)
{
    //*********************************** Smoothing *************************************//
    if (mModuleA_SmootherMask)
    {
        applyASmoother();
    }

    if (mModuleB_SmootherMask)
    {
        applyBSmoother();
    }


    //********************************* Modulation A ************************************//    
    float tmpVar = mModuleA_SelfMix * mModuleA_PmSelf * ((_envRamp_A * mModuleA_PmSelf_EnvA_Amnt) + (1.f - mModuleA_PmSelf_EnvA_Amnt));
    tmpVar = tmpVar + mModuleB_CrossMix * mModuleA_PmCross * ((_envRamp_B * mModuleA_PmCross_EnvB_Amnt) + (1.f - mModuleA_PmCross_EnvB_Amnt));
    tmpVar = tmpVar + _feedbackSample * mModuleA_PmFeedback * ((_envRamp_C * mModuleA_PmFeedback_EnvC_Amnt) + (1.f - mModuleA_PmFeedback_EnvC_Amnt));


    //********************************** Oscillator A ***********************************//
    tmpVar = pModuleA_ChirpFilter->applyFilter(tmpVar);
    tmpVar += mModuleA_OscPhase;

    tmpVar += (-0.25f);
    tmpVar -= round(tmpVar);                            // Wrap

    if (fabs(mModuleA_PhaseStateVar - tmpVar) > 0.5f)   // Checke edge
    {
        mModuleA_PhaseInc = setPhaseInc(mModuleA_OscFreq, mModuleA_Fluct, mModuleA_RandVal);
    }

    mModuleA_PhaseStateVar = tmpVar;

    mModuleA_OscPhase += mModuleA_PhaseInc;
    mModuleA_OscPhase -= round(mModuleA_OscPhase);

    tmpVar += tmpVar;                                   // oscSinP3
    tmpVar = fabs(tmpVar);
    tmpVar = 0.5f - tmpVar;

    float squareTmpVar = tmpVar * tmpVar;
    float oscSampleA = tmpVar * ((2.26548f * squareTmpVar - 5.13274f) * squareTmpVar + 3.14159f);


    //********************************* Modulation B ************************************//
    tmpVar = mModuleB_SelfMix * mModuleB_PmSelf * ((_envRamp_B * mModuleB_PmSelf_EnvB_Amnt) + (1.f - mModuleB_PmSelf_EnvB_Amnt));
    tmpVar = tmpVar + mModuleA_CrossMix * mModuleB_PmCross * ((_envRamp_A * mModuleB_PmCross_EnvA_Amnt) + (1.f - mModuleB_PmCross_EnvA_Amnt));
    tmpVar = tmpVar + _feedbackSample * mModuleB_PmFeedback * ((_envRamp_C * mModuleB_PmFeedback_EnvC_Amnt) + (1.f - mModuleB_PmFeedback_EnvC_Amnt));


    //********************************** Oscillator B ***********************************//
    tmpVar = pModuleB_ChirpFilter->applyFilter(tmpVar);
    tmpVar += mModuleB_OscPhase;

    tmpVar += (-0.25f);
    tmpVar -= round(tmpVar);                            // Wrap

    if (fabs(mModuleB_PhaseStateVar - tmpVar) > 0.5f)   // Check edge
    {
        mModuleB_PhaseInc = setPhaseInc(mModuleB_OscFreq, mModuleB_Fluct, mModuleB_RandVal);
    }

    mModuleB_PhaseStateVar = tmpVar;

    mModuleB_OscPhase += mModuleB_PhaseInc;
    mModuleB_OscPhase -= round(mModuleB_OscPhase);

    tmpVar += tmpVar;                                   // oscSinP3
    tmpVar = fabs(tmpVar);
    tmpVar = 0.5f - tmpVar;

    squareTmpVar = tmpVar * tmpVar;

    float oscSampleB = tmpVar * ((2.26548f * squareTmpVar - 5.13274f) * squareTmpVar + 3.14159f);


    //************************************ Shaper A *************************************//
    tmpVar = (_envRamp_A * mModuleA_Drive_EnvA_Amnt + (1.f - mModuleA_Drive_EnvA_Amnt)) * mModuleA_Drive + 0.18f;

    float shaperSampleA = oscSampleA * tmpVar;
    tmpVar = shaperSampleA;

    shaperSampleA = NlToolbox::Math::sinP3(shaperSampleA);
    shaperSampleA = NlToolbox::Others::threeRanges(shaperSampleA, tmpVar, mModuleA_Fold);

    squareTmpVar = shaperSampleA * shaperSampleA + (-0.5f);

    shaperSampleA = NlToolbox::Others::parAsym(shaperSampleA, squareTmpVar, mModuleA_Asym);


    //************************************ Shaper B *************************************//
    tmpVar = (_envRamp_B * mModuleB_Drive_EnvB_Amnt + (1.f - mModuleB_Drive_EnvB_Amnt)) * mModuleB_Drive + 0.18f;

    float shaperSampleB = oscSampleB * tmpVar;
    tmpVar = shaperSampleB;

    shaperSampleB = NlToolbox::Math::sinP3(shaperSampleB);
    shaperSampleB = NlToolbox::Others::threeRanges(shaperSampleB, tmpVar, mModuleB_Fold);

    squareTmpVar = shaperSampleB * shaperSampleB + (-0.5f);

    shaperSampleB = NlToolbox::Others::parAsym(shaperSampleB, squareTmpVar, mModuleB_Asym);


    //*********************************** Crossfades ************************************//
    mModuleA_SelfMix = NlToolbox::Crossfades::bipolarCrossFade(oscSampleA, shaperSampleA, mModuleA_PmSelfShaper);
    mModuleA_CrossMix = NlToolbox::Crossfades::bipolarCrossFade(oscSampleA, shaperSampleA, mModuleB_PmCrossShaper);

    mModuleB_SelfMix = NlToolbox::Crossfades::bipolarCrossFade(oscSampleB, shaperSampleB, mModuleB_PmSelfShaper);
    mModuleB_CrossMix = NlToolbox::Crossfades::bipolarCrossFade(oscSampleB, shaperSampleB, mModuleA_PmCrossShaper);

    mSampleA = NlToolbox::Crossfades::bipolarCrossFade(oscSampleA, shaperSampleA, mModuleA_ShaperMix);
    mSampleB = NlToolbox::Crossfades::bipolarCrossFade(oscSampleB, shaperSampleB, mModuleB_ShaperMix);


    //******************************* Envelope Influence ********************************//
    mSampleA *= _envRamp_A;
    mSampleB *= _envRamp_B;


    //********************************** Feedback Mix ***********************************//
    tmpVar = (1.f - mModuleA_FeedbackMix_EnvC_Amnt) * _gateRamp + mModuleA_FeedbackMix_EnvC_Amnt * _envRamp_C;
    tmpVar *= _feedbackSample;
    mSampleA = (mSampleA * (1.f - mModuleA_FeedbackMix)) + (tmpVar * mModuleA_FeedbackMix);

    tmpVar = (1.f - mModuleB_FeedbackMix_EnvC_Amnt) * _gateRamp + mModuleB_FeedbackMix_EnvC_Amnt * _envRamp_C;
    tmpVar *= _feedbackSample;
    mSampleB = (mSampleB * (1.f - mModuleB_FeedbackMix)) + (tmpVar * mModuleB_FeedbackMix);


    //******************************** Ring Modulation **********************************//
    tmpVar = mSampleA * mSampleB;

    mSampleA = NlToolbox::Crossfades::bipolarCrossFade(mSampleA, tmpVar, mModuleA_RingMod);
    mSampleB = NlToolbox::Crossfades::bipolarCrossFade(mSampleB, tmpVar, mModuleB_RingMod);
}



/******************************************************************************/
/** @brief    main function which calculates the modulated phase depending
 *            on mix amounts (self modulation, cross modulation, feedback modulation),
 *            calculates the samples of the 2 oscillators and 2 shapers
 *            -> reads Envelope signals from global array PARAMSIGNALDATA
*******************************************************************************/

void Soundgenerator::generateSound(float _feedbackSample, float *polyPtr)
{
    float env_A = polyPtr[ENV_A];
    float env_B = polyPtr[ENV_B];
    float env_C = polyPtr[ENV_C];
    float env_Gate = polyPtr[ENV_GATE];

    //*********************************** Smoothing *************************************//
    if (mModuleA_SmootherMask)
    {
        applyASmoother();
    }

    if (mModuleB_SmootherMask)
    {
        applyBSmoother();
    }


    //********************************* Modulation A ************************************//
    float tmpVar = mModuleA_SelfMix * mModuleA_PmSelf * ((env_A * mModuleA_PmSelf_EnvA_Amnt) + (1.f - mModuleA_PmSelf_EnvA_Amnt));
    tmpVar = tmpVar + mModuleB_CrossMix * mModuleA_PmCross * ((env_B * mModuleA_PmCross_EnvB_Amnt) + (1.f - mModuleA_PmCross_EnvB_Amnt));
    tmpVar = tmpVar + _feedbackSample * mModuleA_PmFeedback * ((env_C * mModuleA_PmFeedback_EnvC_Amnt) + (1.f - mModuleA_PmFeedback_EnvC_Amnt));


    //********************************** Oscillator A ***********************************//
    tmpVar = pModuleA_ChirpFilter->applyFilter(tmpVar);
    tmpVar += mModuleA_OscPhase;

    tmpVar += (-0.25f);
    tmpVar -= round(tmpVar);                            // Wrap

    if (fabs(mModuleA_PhaseStateVar - tmpVar) > 0.5f)   // Checke edge
    {
        mModuleA_PhaseInc = setPhaseInc(mModuleA_OscFreq, mModuleA_Fluct, mModuleA_RandVal);
    }

    mModuleA_PhaseStateVar = tmpVar;

    mModuleA_OscPhase += mModuleA_PhaseInc;
    mModuleA_OscPhase -= round(mModuleA_OscPhase);

    tmpVar += tmpVar;                                   // oscSinP3
    tmpVar = fabs(tmpVar);
    tmpVar = 0.5f - tmpVar;

    float squareTmpVar = tmpVar * tmpVar;
    float oscSampleA = tmpVar * ((2.26548f * squareTmpVar - 5.13274f) * squareTmpVar + 3.14159f);


    //********************************* Modulation B ************************************//
    tmpVar = mModuleB_SelfMix * mModuleB_PmSelf * ((env_B * mModuleB_PmSelf_EnvB_Amnt) + (1.f - mModuleB_PmSelf_EnvB_Amnt));
    tmpVar = tmpVar + mModuleA_CrossMix * mModuleB_PmCross * ((env_A * mModuleB_PmCross_EnvA_Amnt) + (1.f - mModuleB_PmCross_EnvA_Amnt));
    tmpVar = tmpVar + _feedbackSample * mModuleB_PmFeedback * ((env_C * mModuleB_PmFeedback_EnvC_Amnt) + (1.f - mModuleB_PmFeedback_EnvC_Amnt));


    //********************************** Oscillator B ***********************************//
    tmpVar = pModuleB_ChirpFilter->applyFilter(tmpVar);
    tmpVar += mModuleB_OscPhase;

    tmpVar += (-0.25f);
    tmpVar -= round(tmpVar);                            // Wrap

    if (fabs(mModuleB_PhaseStateVar - tmpVar) > 0.5f)   // Check edge
    {
        mModuleB_PhaseInc = setPhaseInc(mModuleB_OscFreq, mModuleB_Fluct, mModuleB_RandVal);
    }

    mModuleB_PhaseStateVar = tmpVar;

    mModuleB_OscPhase += mModuleB_PhaseInc;
    mModuleB_OscPhase -= round(mModuleB_OscPhase);

    tmpVar += tmpVar;                                   // oscSinP3
    tmpVar = fabs(tmpVar);
    tmpVar = 0.5f - tmpVar;

    squareTmpVar = tmpVar * tmpVar;

    float oscSampleB = tmpVar * ((2.26548f * squareTmpVar - 5.13274f) * squareTmpVar + 3.14159f);


    //************************************ Shaper A *************************************//
    tmpVar = (env_A * mModuleA_Drive_EnvA_Amnt + (1.f - mModuleA_Drive_EnvA_Amnt)) * mModuleA_Drive + 0.18f;

    float shaperSampleA = oscSampleA * tmpVar;
    tmpVar = shaperSampleA;

    shaperSampleA = NlToolbox::Math::sinP3(shaperSampleA);
    shaperSampleA = NlToolbox::Others::threeRanges(shaperSampleA, tmpVar, mModuleA_Fold);

    squareTmpVar = shaperSampleA * shaperSampleA + (-0.5f);

    shaperSampleA = NlToolbox::Others::parAsym(shaperSampleA, squareTmpVar, mModuleA_Asym);


    //************************************ Shaper B *************************************//
    tmpVar = (env_B * mModuleB_Drive_EnvB_Amnt + (1.f - mModuleB_Drive_EnvB_Amnt)) * mModuleB_Drive + 0.18f;

    float shaperSampleB = oscSampleB * tmpVar;
    tmpVar = shaperSampleB;

    shaperSampleB = NlToolbox::Math::sinP3(shaperSampleB);
    shaperSampleB = NlToolbox::Others::threeRanges(shaperSampleB, tmpVar, mModuleB_Fold);

    squareTmpVar = shaperSampleB * shaperSampleB + (-0.5f);

    shaperSampleB = NlToolbox::Others::parAsym(shaperSampleB, squareTmpVar, mModuleB_Asym);


    //*********************************** Crossfades ************************************//
    mModuleA_SelfMix = NlToolbox::Crossfades::bipolarCrossFade(oscSampleA, shaperSampleA, mModuleA_PmSelfShaper);
    mModuleA_CrossMix = NlToolbox::Crossfades::bipolarCrossFade(oscSampleA, shaperSampleA, mModuleB_PmCrossShaper);

    mModuleB_SelfMix = NlToolbox::Crossfades::bipolarCrossFade(oscSampleB, shaperSampleB, mModuleB_PmSelfShaper);
    mModuleB_CrossMix = NlToolbox::Crossfades::bipolarCrossFade(oscSampleB, shaperSampleB, mModuleA_PmCrossShaper);

    mSampleA = NlToolbox::Crossfades::bipolarCrossFade(oscSampleA, shaperSampleA, mModuleA_ShaperMix);
    mSampleB = NlToolbox::Crossfades::bipolarCrossFade(oscSampleB, shaperSampleB, mModuleB_ShaperMix);


    //******************************* Envelope Influence ********************************//
    mSampleA *= env_A;
    mSampleB *= env_B;


    //********************************** Feedback Mix ***********************************//
    tmpVar = (1.f - mModuleA_FeedbackMix_EnvC_Amnt) * env_Gate + mModuleA_FeedbackMix_EnvC_Amnt * env_C;
    tmpVar *= _feedbackSample;
    mSampleA = (mSampleA * (1.f - mModuleA_FeedbackMix)) + (tmpVar * mModuleA_FeedbackMix);

    tmpVar = (1.f - mModuleB_FeedbackMix_EnvC_Amnt) * env_Gate + mModuleB_FeedbackMix_EnvC_Amnt * env_C;
    tmpVar *= _feedbackSample;
    mSampleB = (mSampleB * (1.f - mModuleB_FeedbackMix)) + (tmpVar * mModuleB_FeedbackMix);


    //******************************** Ring Modulation **********************************//
    tmpVar = mSampleA * mSampleB;

    mSampleA = NlToolbox::Crossfades::bipolarCrossFade(mSampleA, tmpVar, mModuleA_RingMod);
    mSampleB = NlToolbox::Crossfades::bipolarCrossFade(mSampleB, tmpVar, mModuleB_RingMod);
}



/******************************************************************************/
/** @brief    pitch set function, which calls the function for calculating the
 *            individual oscialltor frequencies. Dependant on previously
 *            Key Tracking Amount and Pitch Offset
 *  @param    incoming pitch
*******************************************************************************/

void Soundgenerator::setPitch(float _pitch)
{
    mPitch = _pitch;

    mModuleA_OscFreq = setOscFreq(mPitch, mModuleA_KeyTracking, mModuleA_PitchOffset);
    mModuleA_PhaseInc = setPhaseInc(mModuleA_OscFreq, mModuleA_Fluct, mModuleA_RandVal);

    mModuleB_OscFreq = setOscFreq(mPitch, mModuleB_KeyTracking, mModuleB_PitchOffset);
    mModuleB_PhaseInc = setPhaseInc(mModuleB_OscFreq, mModuleB_Fluct, mModuleB_RandVal);
}



/******************************************************************************/
/** @brief    depending on the voice number, the seed in the oscillators is set
 *            for fluctuation/ randomization purposes
 *  @param    voice number 0 - 11
*******************************************************************************/

void Soundgenerator::setVoiceNumber(uint32_t _voiceNumber)
{
    mModuleA_RandVal = _voiceNumber + 1;
    mModuleB_RandVal = _voiceNumber + 1 + 111;
}



/******************************************************************************/
/** @brief    phase reset for each oscillator, which happens with every note on
 *            event
*******************************************************************************/

void Soundgenerator::resetPhase()
{
    mModuleA_OscPhase = mModuleA_ResetPhase;
    mModuleB_OscPhase = mModuleB_ResetPhase;
}



/*****************************************************************************/
/** @brief    interface method which converts and scales the incoming midi
 *            values and passes these to the respective methods or parameters
 *  @param    midi instrument ID -> enum  InstrID (Soundgenerator.h)
 *  @param    midi control ID -> enum CtrlID (Soundgenerator.h)
 *  @param    midi control value [0 ... 127]
******************************************************************************/

void Soundgenerator::setGenParams(unsigned char _instrID, unsigned char _ctrlID, float _ctrlVal)
{
    switch (_instrID)
    {
        /********************** Oscillator A Controls **********************/
        case InstrID::OSC_A:

        switch (_ctrlID)
        {
            case OscCtrlID::OFFSETPITCH:
                _ctrlVal -= 20;
#ifdef PRINT_PARAMVALUES
                printf("Osc A - Pitch Offset: %f\n", _ctrlVal);
#endif
                mModuleA_PitchOffset = _ctrlVal;
                mModuleA_OscFreq = setOscFreq(mPitch, mModuleA_KeyTracking, mModuleA_PitchOffset);
                mModuleA_PhaseInc = setPhaseInc(mModuleA_OscFreq, mModuleA_Fluct, mModuleA_RandVal);
                break;

            case OscCtrlID::FLUCT:
                _ctrlVal /= 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Osc A - Fluct: %f\n", _ctrlVal);
#endif
                mModuleA_Fluct = (_ctrlVal * _ctrlVal * 0.95f);
                break;

            case OscCtrlID::PMSELF:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Osc A - PM Self: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 1: Pm Self
                mModuleA_PmSelf_target = fabs(_ctrlVal) * _ctrlVal * 0.5f;
                mModuleA_PmSelf_base = mModuleA_PmSelf;
                mModuleA_PmSelf_diff = mModuleA_PmSelf_target - mModuleA_PmSelf_base;

                mModuleA_SmootherMask |= 0x0001;
                mModuleA_PmSelf_ramp = 0.f;
                break;

            case OscCtrlID::PMCROSS:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Osc A - PM B: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 3: Pm Cross
                mModuleA_PmCross_target = fabs(_ctrlVal) * _ctrlVal;
                mModuleA_PmCross_base = mModuleA_PmCross;
                mModuleA_PmCross_diff = mModuleA_PmCross_target - mModuleA_PmCross_base;

                mModuleA_SmootherMask |= 0x0004;
                mModuleA_PmCross_ramp = 0.f;
                break;

            case OscCtrlID::PMFEEDBACK:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Osc A - PM FB: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 11: PM Feedback
                mModuleA_PmFeedback_target = _ctrlVal;
                mModuleA_PmFeedback_base = mModuleA_PmFeedback;
                mModuleA_PmFeedback_diff = mModuleA_PmFeedback_target - mModuleA_PmFeedback_base;

                mModuleA_SmootherMask |= 0x0400;
                mModuleA_PmFeedback_ramp = 0.f;
                break;

            case OscCtrlID::PMSELF_ENV:
                _ctrlVal = _ctrlVal / 127.f;
#ifdef PRINT_PARAMVALUES
                printf("Osc A - PM Self Env A: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 14: PM Self Env A Amount
                mModuleA_PmSelf_EnvA_Amnt_target = _ctrlVal;
                mModuleA_PmSelf_EnvA_Amnt_base = mModuleA_PmSelf_EnvA_Amnt;
                mModuleA_PmSelf_EnvA_Amnt_diff = mModuleA_PmSelf_EnvA_Amnt_target - mModuleA_PmSelf_EnvA_Amnt_base;

                mModuleA_SmootherMask |= 0x2000;
                mModuleA_PmSelf_EnvA_Amnt_ramp = 0.f;
                break;

            case OscCtrlID::PMCROSS_ENV:
                _ctrlVal = _ctrlVal / 127.f;
#ifdef PRINT_PARAMVALUES
                printf("Osc A - PM Cross Env B: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 15: PM Cross Env B Amount
                mModuleA_PmCross_EnvB_Amnt_target = _ctrlVal;
                mModuleA_PmCross_EnvB_Amnt_base = mModuleA_PmCross_EnvB_Amnt;
                mModuleA_PmCross_EnvB_Amnt_diff = mModuleA_PmCross_EnvB_Amnt_target - mModuleA_PmCross_EnvB_Amnt_base;

                mModuleA_SmootherMask |= 0x4000;
                mModuleA_PmCross_EnvB_Amnt_ramp = 0.f;
                break;

            case OscCtrlID::PMFEEDBACK_ENV:
                _ctrlVal = _ctrlVal / 127.f;
#ifdef PRINT_PARAMVALUES
                printf("Osc A - PM Feedback Env C: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 16: PM Feedback Env C Amount
                mModuleA_PmFeedback_EnvC_Amnt_target = _ctrlVal;
                mModuleA_PmFeedback_EnvC_Amnt_base = mModuleA_PmFeedback_EnvC_Amnt;
                mModuleA_PmFeedback_EnvC_Amnt_diff = mModuleA_PmFeedback_EnvC_Amnt_target - mModuleA_PmFeedback_EnvC_Amnt_base;

                mModuleA_SmootherMask |= 0x8000;
                mModuleA_PmFeedback_EnvC_Amnt_ramp = 0.f;
                break;

            case OscCtrlID::KEYTRACKING:
                if (_ctrlVal > 105)
                {
                    _ctrlVal = 105;
                }
#ifdef PRINT_PARAMVALUES
                printf("Osc A: Key Tracking: %f\n", _ctrlVal);
#endif
                mModuleA_KeyTracking = _ctrlVal / 100.f;
                mModuleA_OscFreq = setOscFreq(mPitch, mModuleA_KeyTracking, mModuleA_PitchOffset);
                mModuleA_PhaseInc = setPhaseInc(mModuleA_OscFreq, mModuleA_Fluct, mModuleA_RandVal);
                break;

            case OscCtrlID::PHASE:
                _ctrlVal = (_ctrlVal / 126.f) - 0.5f;

                if (_ctrlVal > 0.5f)
                {
                    _ctrlVal = 0.5f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Osc A - Phase: %f\n", _ctrlVal);
#endif
                mModuleA_ResetPhase = _ctrlVal;
                break;

            case OscCtrlID::PMSELFSHAPER:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Osc A - Shaper PM Self: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 2: Pm Self Shaper
                mModuleA_PmSelfShaper_target = _ctrlVal;
                mModuleA_PmSelfShaper_base = mModuleA_PmSelfShaper;
                mModuleA_PmSelfShaper_diff = mModuleA_PmSelfShaper_target - mModuleA_PmSelfShaper_base;

                mModuleA_SmootherMask |= 0x0002;
                mModuleA_PmSelfShaper_ramp = 0.f;
                break;

            case OscCtrlID::PMCROSSSHAPER:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Osc A - Shaper PM B: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 4: Pm Cross Shaper
                mModuleA_PmCrossShaper_target = _ctrlVal;
                mModuleA_PmCrossShaper_base = mModuleA_PmCrossShaper;
                mModuleA_PmCrossShaper_diff = mModuleA_PmCrossShaper_target - mModuleA_PmCrossShaper_base;

                mModuleA_SmootherMask |= 0x0008;
                mModuleA_PmCrossShaper_ramp = 0.f;
                break;

            case OscCtrlID::CHIRPFREQ:
                _ctrlVal = (_ctrlVal + 160.f) / 2.f;

                if (_ctrlVal > 140.f)
                {
                    _ctrlVal = 140.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Osc A - Chirp Freq: %f\n", _ctrlVal);
#endif
                _ctrlVal = NlToolbox::Conversion::pitch2freq(_ctrlVal - 1.5f);

                pModuleA_ChirpFilter->setFrequency(_ctrlVal);
                break;
        }
        break;

        /********************** Shaper A Controls **********************/
        case InstrID::SHAPER_A:

        switch (_ctrlID)
        {
            case ShaperCtrlID::DRIVE:

                _ctrlVal = (_ctrlVal/ 127.f) * 25.f;
#ifdef PRINT_PARAMVALUES
                printf("Shaper A - Drive: %f\n", _ctrlVal);
#endif
                _ctrlVal = NlToolbox::Conversion::db2af(_ctrlVal);
                _ctrlVal *= 0.18f;
                _ctrlVal += -0.18f;

                // Initialize Smoother ID 5: Drive
                mModuleA_Drive_target = _ctrlVal;
                mModuleA_Drive_base = mModuleA_Drive;
                mModuleA_Drive_diff = mModuleA_Drive_target - mModuleA_Drive_base;

                mModuleA_SmootherMask |= 0x0010;
                mModuleA_Drive_ramp = 0.f;
                break;

            case ShaperCtrlID::FOLD:
                _ctrlVal /= 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Shaper A - Fold: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 6: Fold
                mModuleA_Fold_target = _ctrlVal;
                mModuleA_Fold_base = mModuleA_Fold;
                mModuleA_Fold_diff = mModuleA_Fold_target - mModuleA_Fold_base;

                mModuleA_SmootherMask |= 0x0020;
                mModuleA_Fold_ramp = 0.f;
                break;

            case ShaperCtrlID::ASYM:
                _ctrlVal /= 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Shaper A - Asym: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 7: Asym
                mModuleA_Asym_target = _ctrlVal;
                mModuleA_Asym_base = mModuleA_Asym;
                mModuleA_Asym_diff = mModuleA_Asym_target - mModuleA_Asym_base;

                mModuleA_SmootherMask |= 0x0040;
                mModuleA_Asym_ramp = 0.f;
                break;

            case ShaperCtrlID::SHAPER_MIX:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Shaper A - Main Mix: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 8: Shaper Mix
                mModuleA_ShaperMix_target = _ctrlVal;
                mModuleA_ShaperMix_base = mModuleA_ShaperMix;
                mModuleA_ShaperMix_diff = mModuleA_ShaperMix_target - mModuleA_ShaperMix_base;

                mModuleA_SmootherMask |= 0x0080;
                mModuleA_ShaperMix_ramp = 0.f;
                break;

            case ShaperCtrlID::FEEDBACK_MIX:
                _ctrlVal = _ctrlVal / 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Shaper A - Feedback Mix: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 10: Feedback Mix Level
                mModuleA_FeedbackMix_target = _ctrlVal * _ctrlVal * _ctrlVal;
                mModuleA_FeedbackMix_base = mModuleA_FeedbackMix;
                mModuleA_FeedbackMix_diff = mModuleA_FeedbackMix_target - mModuleA_FeedbackMix_base;

                mModuleA_SmootherMask |= 0x0200;
                mModuleA_FeedbackMix_ramp = 0.f;
                break;

            case ShaperCtrlID::RING_MOD:
                _ctrlVal = _ctrlVal / 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Shaper A - Ring Modulation: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 9: Ring Modulation
                mModuleA_RingMod_target = _ctrlVal;
                mModuleA_RingMod_base = mModuleA_RingMod;
                mModuleA_RingMod_diff = mModuleA_RingMod_target - mModuleA_RingMod_base;

                mModuleA_SmootherMask |= 0x0100;
                mModuleA_RingMod_ramp = 0.f;
                break;

            case ShaperCtrlID::DRIVE_ENV:
                _ctrlVal = _ctrlVal / 127.f;
#ifdef PRINT_PARAMVALUES
                printf("Shaper A - Drive Env A Amount: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 17: Drive Env A Amount
                mModuleA_Drive_EnvA_Amnt_target = _ctrlVal;
                mModuleA_Drive_EnvA_Amnt_base = mModuleA_Drive_EnvA_Amnt;
                mModuleA_Drive_EnvA_Amnt_diff = mModuleA_Drive_EnvA_Amnt_target - mModuleA_Drive_EnvA_Amnt_base;

                mModuleA_SmootherMask |= 0x00010000;
                mModuleA_Drive_EnvA_Amnt_ramp = 0.f;
                break;


            case ShaperCtrlID::FBMIX_ENV:
                _ctrlVal = _ctrlVal / 127.f;
#ifdef PRINT_PARAMVALUES
                printf("Shaper A - Feedback Mix Env C Amount: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 18: Feedback Mis Env C Amount
                mModuleA_FeedbackMix_EnvC_Amnt_target = _ctrlVal;
                mModuleA_FeedbackMix_EnvC_Amnt_base = mModuleA_FeedbackMix_EnvC_Amnt;
                mModuleA_FeedbackMix_EnvC_Amnt_diff = mModuleA_FeedbackMix_EnvC_Amnt_target - mModuleA_FeedbackMix_EnvC_Amnt_base;

                mModuleA_SmootherMask |= 0x00020000;
                mModuleA_FeedbackMix_EnvC_Amnt_ramp = 0.f;
                break;
        }
        break;

        /********************** Oscillator B Controls **********************/
        case InstrID::OSC_B:

        switch (_ctrlID)
        {
            case OscCtrlID::OFFSETPITCH:
                _ctrlVal -= 20;
#ifdef PRINT_PARAMVALUES
                printf("Osc B - Pitch Offset: %f\n", _ctrlVal);
#endif
                mModuleB_PitchOffset = _ctrlVal;
                mModuleB_OscFreq = setOscFreq(mPitch, mModuleB_KeyTracking, mModuleB_PitchOffset);
                mModuleB_PhaseInc = setPhaseInc(mModuleB_OscFreq, mModuleB_Fluct, mModuleB_RandVal);
                break;

            case OscCtrlID::FLUCT:
                _ctrlVal /= 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Osc B - Fluct: %f\n", _ctrlVal);
#endif
                mModuleB_Fluct = (_ctrlVal * _ctrlVal * 0.95f);
                break;

            case OscCtrlID::PMSELF:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Osc B - PM Self: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 1: Pm Self
                mModuleB_PmSelf_target = fabs(_ctrlVal) * _ctrlVal * 0.5f;
                mModuleB_PmSelf_base = mModuleB_PmSelf;
                mModuleB_PmSelf_diff = mModuleB_PmSelf_target - mModuleB_PmSelf_base;

                mModuleB_SmootherMask |= 0x0001;
                mModuleB_PmSelf_ramp = 0.f;
                break;

            case OscCtrlID::PMCROSS:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Osc B - PM A: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 3: Pm Cross
                mModuleB_PmCross_target = fabs(_ctrlVal) * _ctrlVal;
                mModuleB_PmCross_base = mModuleB_PmCross;
                mModuleB_PmCross_diff = mModuleB_PmCross_target - mModuleB_PmCross_base;

                mModuleB_SmootherMask |= 0x0004;
                mModuleB_PmCross_ramp = 0.f;
                break;

            case OscCtrlID::PMFEEDBACK:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Osc B - PM FB: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 11: PM Feedback
                mModuleB_PmFeedback_target = _ctrlVal;
                mModuleB_PmFeedback_diff = mModuleB_PmFeedback_target - mModuleB_PmFeedback;

                mModuleB_SmootherMask |= 0x0400;
                mModuleB_PmFeedback_ramp = 0.f;
                break;

            case OscCtrlID::PMSELF_ENV:
                _ctrlVal = _ctrlVal / 127.f;
#ifdef PRINT_PARAMVALUES
                printf("Osc B - PM Self Env B: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 14: PM Self Env B Amount
                mModuleB_PmSelf_EnvB_Amnt_target = _ctrlVal;
                mModuleB_PmSelf_EnvB_Amnt_base = mModuleB_PmSelf_EnvB_Amnt;
                mModuleB_PmSelf_EnvB_Amnt_diff = mModuleB_PmSelf_EnvB_Amnt_target - mModuleB_PmSelf_EnvB_Amnt_base;

                mModuleB_SmootherMask |= 0x2000;
                mModuleB_PmSelf_EnvB_Amnt_ramp = 0.f;
                break;

            case OscCtrlID::PMCROSS_ENV:
                _ctrlVal = _ctrlVal / 127.f;
#ifdef PRINT_PARAMVALUES
                printf("Osc B - PM Cross Env A: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 15: PM Cross Env A Amount
                mModuleB_PmCross_EnvA_Amnt_target = _ctrlVal;
                mModuleB_PmCross_EnvA_Amnt_base = mModuleB_PmCross_EnvA_Amnt;
                mModuleB_PmCross_EnvA_Amnt_diff = mModuleB_PmCross_EnvA_Amnt_target - mModuleB_PmCross_EnvA_Amnt_base;

                mModuleB_SmootherMask |= 0x4000;
                mModuleB_PmCross_EnvA_Amnt_ramp = 0.f;
                break;

            case OscCtrlID::PMFEEDBACK_ENV:
                _ctrlVal = _ctrlVal / 127.f;
#ifdef PRINT_PARAMVALUES
                printf("Osc B - PM Feedback Env C: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 16: PM Feedback Env C Amount
                mModuleB_PmFeedback_EnvC_Amnt_target = _ctrlVal;
                mModuleB_PmFeedback_EnvC_Amnt_base = mModuleB_PmFeedback_EnvC_Amnt;
                mModuleB_PmFeedback_EnvC_Amnt_diff = mModuleB_PmFeedback_EnvC_Amnt_target - mModuleB_PmFeedback_EnvC_Amnt_base;

                mModuleB_SmootherMask |= 0x8000;
                mModuleB_PmFeedback_EnvC_Amnt_ramp = 0.f;
                break;

            case OscCtrlID::KEYTRACKING:
                if (_ctrlVal > 105)
                {
                    _ctrlVal = 105;
                }
#ifdef PRINT_PARAMVALUES
                printf("Osc B - Key Tracking: %f\n", _ctrlVal);
#endif
                mModuleB_KeyTracking = _ctrlVal / 100.f;
                mModuleB_OscFreq = setOscFreq(mPitch, mModuleB_KeyTracking, mModuleB_PitchOffset);
                mModuleB_PhaseInc = setPhaseInc(mModuleB_OscFreq, mModuleB_Fluct, mModuleB_RandVal);
                break;

            case OscCtrlID::PHASE:
                _ctrlVal = (_ctrlVal / 126.f) - 0.5f;

                if (_ctrlVal > 0.5f)
                {
                    _ctrlVal = 0.5f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Osc B - Phase: %f\n", _ctrlVal);
#endif
                mModuleB_ResetPhase = _ctrlVal;
                break;

            case OscCtrlID::PMSELFSHAPER:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Osc B - Shaper PM Self: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 2: Pm Self Shaper
                mModuleB_PmSelfShaper_target = _ctrlVal;
                mModuleB_PmSelfShaper_base = mModuleB_PmSelfShaper;
                mModuleB_PmSelfShaper_diff = mModuleB_PmSelfShaper_target - mModuleB_PmSelfShaper_base;

                mModuleB_SmootherMask |= 0x0002;
                mModuleB_PmSelfShaper_ramp = 0.f;
                break;

            case OscCtrlID::PMCROSSSHAPER:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Osc B - Shaper PM A: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 4: Pm Cross Shaper
                mModuleB_PmCrossShaper_target = _ctrlVal;
                mModuleB_PmCrossShaper_base = mModuleB_PmCrossShaper;
                mModuleB_PmCrossShaper_diff = mModuleB_PmCrossShaper_target - mModuleB_PmCrossShaper_base;

                mModuleB_SmootherMask |= 0x0008;
                mModuleB_PmCrossShaper_ramp = 0.f;
                break;


            case OscCtrlID::CHIRPFREQ:
                _ctrlVal = (_ctrlVal + 160.f) / 2.f;

                if (_ctrlVal > 140.f)
                {
                    _ctrlVal = 140.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Osc B - Chirp Freq: %f\n", _ctrlVal);
#endif
                _ctrlVal = NlToolbox::Conversion::pitch2freq(_ctrlVal - 1.5f);

                pModuleB_ChirpFilter->setFrequency(_ctrlVal);
                break;
        }
        break;

        /********************** Shaper B Controls **********************/
        case InstrID::SHAPER_B:

        switch (_ctrlID)
        {
            case ShaperCtrlID::DRIVE:
                _ctrlVal = (_ctrlVal/ 127.f) * 25.f;
#ifdef PRINT_PARAMVALUES
                printf("Shaper B - Drive: %f\n", _ctrlVal);
#endif
                _ctrlVal = NlToolbox::Conversion::db2af(_ctrlVal);
                _ctrlVal *= 0.18f;
                _ctrlVal += -0.18f;

                // Initialize Smoother ID 5: Drive
                mModuleB_Drive_target = _ctrlVal;
                mModuleB_Drive_base = mModuleB_Drive;
                mModuleB_Drive_diff = mModuleB_Drive_target - mModuleB_Drive_base;

                mModuleB_SmootherMask |= 0x0010;
                mModuleB_Drive_ramp = 0.f;
                break;

            case ShaperCtrlID::FOLD:
                _ctrlVal /= 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Shaper B - Fold: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 6: Fold
                mModuleB_Fold_target = _ctrlVal;
                mModuleB_Fold_base = mModuleB_Fold;
                mModuleB_Fold_diff = mModuleB_Fold_target - mModuleB_Fold_base;

                mModuleB_SmootherMask |= 0x0020;
                mModuleB_Fold_ramp = 0.f;
                break;

            case ShaperCtrlID::ASYM:
                _ctrlVal /= 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Shaper B - Asym: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 7: Asym
                mModuleB_Asym_target = _ctrlVal;
                mModuleB_Asym_base = mModuleB_Asym;
                mModuleB_Asym_diff = mModuleB_Asym_target - mModuleB_Asym_base;

                mModuleB_SmootherMask |= 0x0040;
                mModuleB_Asym_ramp = 0.f;
                break;

            case ShaperCtrlID::SHAPER_MIX:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Shaper B - Main Mix: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 8: Shaper Mix
                mModuleB_ShaperMix_target = _ctrlVal;
                mModuleB_ShaperMix_base = mModuleB_ShaperMix;
                mModuleB_ShaperMix_diff = mModuleB_ShaperMix_target - mModuleB_ShaperMix_base;

                mModuleB_SmootherMask |= 0x0080;
                mModuleB_ShaperMix_ramp = 0.f;
                break;

            case ShaperCtrlID::FEEDBACK_MIX:
                _ctrlVal = _ctrlVal / 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Shaper B - Feedback Mix: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 10: Feedback Mix Level
                mModuleB_FeedbackMix_target = _ctrlVal * _ctrlVal * _ctrlVal;
                mModuleB_FeedbackMix_base = mModuleB_FeedbackMix;
                mModuleB_FeedbackMix_diff = mModuleB_FeedbackMix_target - mModuleB_FeedbackMix_base;

                mModuleB_SmootherMask |= 0x0200;
                mModuleB_FeedbackMix_ramp = 0.f;
                break;

            case ShaperCtrlID::RING_MOD:
                _ctrlVal /= 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }
#ifdef PRINT_PARAMVALUES
                printf("Shaper B - Ring Modulation: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 9: Ring Modulatino
                mModuleB_RingMod_target = _ctrlVal;
                mModuleB_RingMod_base = mModuleB_RingMod;
                mModuleB_RingMod_diff = mModuleB_RingMod_target - mModuleB_RingMod_base;

                mModuleB_SmootherMask |= 0x0100;
                mModuleB_RingMod_ramp = 0.f;
                break;

            case ShaperCtrlID::DRIVE_ENV:
                _ctrlVal = _ctrlVal / 127.f;
#ifdef PRINT_PARAMVALUES
                printf("Shaper B - Drive Env B Amount: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 17: Drive Env B Amount
                mModuleB_Drive_EnvB_Amnt_target = _ctrlVal;
                mModuleB_Drive_EnvB_Amnt_base = mModuleB_Drive_EnvB_Amnt;
                mModuleB_Drive_EnvB_Amnt_diff = mModuleB_Drive_EnvB_Amnt_target - mModuleB_Drive_EnvB_Amnt_base;

                mModuleB_SmootherMask |= 0x00010000;
                mModuleB_Drive_EnvB_Amnt_ramp = 0.f;
                break;


            case ShaperCtrlID::FBMIX_ENV:
                _ctrlVal = _ctrlVal / 127.f;
#ifdef PRINT_PARAMVALUES
                printf("Shaper B - Feedback Mix Env C Amount: %f\n", _ctrlVal);
#endif
                // Initialize Smoother ID 18: Feedback Mix Env C Amount
                mModuleB_FeedbackMix_EnvC_Amnt_target = _ctrlVal;
                mModuleB_FeedbackMix_EnvC_Amnt_base = mModuleB_FeedbackMix_EnvC_Amnt;
                mModuleB_FeedbackMix_EnvC_Amnt_diff = mModuleB_FeedbackMix_EnvC_Amnt_target - mModuleB_FeedbackMix_EnvC_Amnt_base;

                mModuleB_SmootherMask |= 0x00020000;
                mModuleB_FeedbackMix_EnvC_Amnt_ramp = 0.f;
                break;

        }
        break;
    }
}



/*****************************************************************************/
/** @brief    applies the smoothers of the Soundgenertors A module,
 *            if the corresponding bit of the mask is set to 1
******************************************************************************/

inline void Soundgenerator::applyASmoother()
{
    //*************************** ID 1: Pm Self *****************************//
    if (mModuleA_PmSelf_ramp < 1.0)
    {
        mModuleA_PmSelf_ramp += SMOOTHER_INC;

        if (mModuleA_PmSelf_ramp > 1.0)
        {
            mModuleA_PmSelf = mModuleA_PmSelf_target;
            mModuleA_SmootherMask &= 0xFFFE;       // switch first bit to 0
        }
        else
        {
            mModuleA_PmSelf = mModuleA_PmSelf_base + mModuleA_PmSelf_diff * mModuleA_PmSelf_ramp;
        }
    }


    //*********************** ID 2: PmSelf Shaper ***************************//
    if (mModuleA_PmSelfShaper_ramp < 1.0)
    {
        mModuleA_PmSelfShaper_ramp += SMOOTHER_INC;

        if (mModuleA_PmSelfShaper_ramp > 1.0)
        {
            mModuleA_PmSelfShaper = mModuleA_PmSelfShaper_target;
            mModuleA_SmootherMask &= 0xFFFD;       // switch second bit to 0
        }
        else
        {
            mModuleA_PmSelfShaper = mModuleA_PmSelfShaper_base + mModuleA_PmSelfShaper_diff * mModuleA_PmSelfShaper_ramp;
        }
    }


    //*************************** ID 3: Pm Cross ****************************//
    if (mModuleA_PmCross_ramp < 1.0)
    {
        mModuleA_PmCross_ramp += SMOOTHER_INC;

        if (mModuleA_PmCross_ramp > 1.0)
        {
            mModuleA_PmCross = mModuleA_PmCross_target;
            mModuleA_SmootherMask &= 0xFFFB;       // switch third bit to 0
        }
        else
        {
            mModuleA_PmCross = mModuleA_PmCross_base + mModuleA_PmCross_diff * mModuleA_PmCross_ramp;
        }
    }


    //********************** ID 4: Pm Cross Shaper **************************//
    if (mModuleA_PmCrossShaper_ramp < 1.0)
    {
        mModuleA_PmCrossShaper_ramp += SMOOTHER_INC;

        if (mModuleA_PmCrossShaper_ramp > 1.0)
        {
            mModuleA_PmCrossShaper = mModuleA_PmCrossShaper_target;
            mModuleA_SmootherMask &= 0xFFF7;       // switch fourth bit to 0
        }
        else
        {
            mModuleA_PmCrossShaper = mModuleA_PmCrossShaper_base + mModuleA_PmCrossShaper_diff * mModuleA_PmCrossShaper_ramp;
        }
    }


    //**************************** ID 5: Drive ******************************//
    if (mModuleA_Drive_ramp < 1.0)
    {
        mModuleA_Drive_ramp += SMOOTHER_INC;

        if (mModuleA_Drive_ramp > 1.0)
        {
            mModuleA_Drive = mModuleA_Drive_target;
            mModuleA_SmootherMask &= 0xFFEF;       // switch fifth bit to 0
        }
        else
        {
            mModuleA_Drive = mModuleA_Drive_base + mModuleA_Drive_diff * mModuleA_Drive_ramp;
        }
    }


    //**************************** ID 6: Fold *******************************//
    if (mModuleA_Fold_ramp < 1.0)
    {
        mModuleA_Fold_ramp += SMOOTHER_INC;

        if (mModuleA_Fold_ramp > 1.0)
        {
            mModuleA_Fold = mModuleA_Fold_target;
            mModuleA_SmootherMask &= 0xFFDF;       // switch sixth bit to 0
        }
        else
        {
            mModuleA_Fold = mModuleA_Fold_base + mModuleA_Fold_diff * mModuleA_Fold_ramp;
        }
    }


    //**************************** ID 7: Asym *******************************//
    if (mModuleA_Asym_ramp < 1.0)
    {
        mModuleA_Asym_ramp += SMOOTHER_INC;

        if (mModuleA_Asym_ramp > 1.0)
        {
            mModuleA_Asym = mModuleA_Asym_target;
            mModuleA_SmootherMask &= 0xFFBF;       // switch seventh bit to 0
        }
        else
        {
            mModuleA_Asym = mModuleA_Asym_base + mModuleA_Asym_diff * mModuleA_Asym_ramp;
        }
    }


    //************************* ID 8: Shaper Mix ****************************//
    if (mModuleA_ShaperMix_ramp < 1.0)
    {
        mModuleA_ShaperMix_ramp += SMOOTHER_INC;

        if (mModuleA_ShaperMix_ramp > 1.0)
        {
            mModuleA_ShaperMix = mModuleA_ShaperMix_target;
            mModuleA_SmootherMask &= 0xFF7F;       // switch eigth bit to 0
        }
        else
        {
            mModuleA_ShaperMix = mModuleA_ShaperMix_base + mModuleA_ShaperMix_diff * mModuleA_ShaperMix_ramp;
        }
    }


    //*********************** ID 9: Ring Modulation *************************//
    if (mModuleA_RingMod_ramp < 1.0)
    {
        mModuleA_RingMod_ramp += SMOOTHER_INC;

        if (mModuleA_RingMod_ramp > 1.0)
        {
            mModuleA_RingMod = mModuleA_RingMod_target;
            mModuleA_SmootherMask &= 0xFEFF;       // switch ninth bit to 0
        }
        else
        {
            mModuleA_RingMod = mModuleA_RingMod_base + mModuleA_RingMod_diff * mModuleA_RingMod_ramp;
        }
    }


    //************************ ID 10: Feedback Mix **************************//
    if (mModuleA_FeedbackMix_ramp < 1.0)
    {
        mModuleA_FeedbackMix_ramp += SMOOTHER_INC;

        if (mModuleA_FeedbackMix_ramp > 1.0)
        {
            mModuleA_FeedbackMix = mModuleA_FeedbackMix_target;
            mModuleA_SmootherMask &= 0xFDFF;       // switch 10th bit to 0
        }
        else
        {
            mModuleA_FeedbackMix = mModuleA_FeedbackMix_base
                    + mModuleA_FeedbackMix_diff * mModuleA_FeedbackMix_ramp;
        }
    }


    //************************** ID 11: Feedback ****************************//
    if (mModuleA_PmFeedback_ramp < 1.0)
    {
        mModuleA_PmFeedback_ramp += SMOOTHER_INC;

        if (mModuleA_PmFeedback_ramp > 1.0)
        {
            mModuleA_PmFeedback = mModuleA_PmFeedback_target;
            mModuleA_SmootherMask &= 0xFBFF;       // switch 11th bit to 0
        }
        else
        {
            mModuleA_PmFeedback = mModuleA_PmFeedback_base + mModuleA_PmFeedback_diff * mModuleA_PmFeedback_ramp;
        }
    }


    //************************ ID 14: PM Self Env A *************************//
    if (mModuleA_PmSelf_EnvA_Amnt_ramp < 1.0)
    {
        mModuleA_PmSelf_EnvA_Amnt_ramp += SMOOTHER_INC;

        if (mModuleA_PmSelf_EnvA_Amnt_ramp > 1.0)
        {
            mModuleA_PmSelf_EnvA_Amnt = mModuleA_PmSelf_EnvA_Amnt_target;
            mModuleA_SmootherMask &= 0xDFFF;       // switch 14th bit to 0
        }
        else
        {
            mModuleA_PmSelf_EnvA_Amnt = mModuleA_PmSelf_EnvA_Amnt_base + mModuleA_PmSelf_EnvA_Amnt_diff * mModuleA_PmSelf_EnvA_Amnt_ramp;
        }
    }


    //************************ ID 15: PM Cross Env B *************************//
    if (mModuleA_PmCross_EnvB_Amnt_ramp < 1.0)
    {
        mModuleA_PmCross_EnvB_Amnt_ramp += SMOOTHER_INC;

        if (mModuleA_PmCross_EnvB_Amnt_ramp > 1.0)
        {
            mModuleA_PmCross_EnvB_Amnt = mModuleA_PmCross_EnvB_Amnt_target;
            mModuleA_SmootherMask &= 0xBFFF;       // switch 15th bit to 0
        }
        else
        {
            mModuleA_PmCross_EnvB_Amnt = mModuleA_PmCross_EnvB_Amnt_base + mModuleA_PmCross_EnvB_Amnt_diff * mModuleA_PmCross_EnvB_Amnt_ramp;
        }
    }


    //********************** ID 16: PM Feedback Env C ************************//
    if (mModuleA_PmFeedback_EnvC_Amnt_ramp < 1.0)
    {
        mModuleA_PmFeedback_EnvC_Amnt_ramp += SMOOTHER_INC;

        if (mModuleA_PmFeedback_EnvC_Amnt_ramp > 1.0)
        {
            mModuleA_PmFeedback_EnvC_Amnt = mModuleA_PmFeedback_EnvC_Amnt_target;
            mModuleA_SmootherMask &= 0x7FFF;       // switch 16th bit to 0
        }
        else
        {
            mModuleA_PmFeedback_EnvC_Amnt = mModuleA_PmFeedback_EnvC_Amnt_base + mModuleA_PmFeedback_EnvC_Amnt_diff * mModuleA_PmFeedback_EnvC_Amnt_ramp;
        }
    }


    //************************* ID 17: Drive Env A ***************************//
    if (mModuleA_Drive_EnvA_Amnt_ramp < 1.0)
    {
        mModuleA_Drive_EnvA_Amnt_ramp += SMOOTHER_INC;

        if (mModuleA_Drive_EnvA_Amnt_ramp > 1.0)
        {
            mModuleA_Drive_EnvA_Amnt = mModuleA_Drive_EnvA_Amnt_target;
            mModuleA_SmootherMask &= 0xFFFEFFFF;       // switch 17th bit to 0
        }
        else
        {
            mModuleA_Drive_EnvA_Amnt = mModuleA_Drive_EnvA_Amnt_base + mModuleA_Drive_EnvA_Amnt_diff * mModuleA_Drive_EnvA_Amnt_ramp;
        }
    }


    //********************* ID 18: Feedback Mix Env C ************************//
    if (mModuleA_FeedbackMix_EnvC_Amnt_ramp < 1.0)
    {
        mModuleA_FeedbackMix_EnvC_Amnt_ramp += SMOOTHER_INC;

        if (mModuleA_FeedbackMix_EnvC_Amnt_ramp > 1.0)
        {
            mModuleA_FeedbackMix_EnvC_Amnt = mModuleA_FeedbackMix_EnvC_Amnt_target;
            mModuleA_SmootherMask &= 0xFFFDFFFF;       // switch 18th bit to 0
        }
        else
        {
            mModuleA_FeedbackMix_EnvC_Amnt = mModuleA_FeedbackMix_EnvC_Amnt_base + mModuleA_FeedbackMix_EnvC_Amnt_diff * mModuleA_FeedbackMix_EnvC_Amnt_ramp;
        }
    }
}


/*****************************************************************************/
/** @brief    applies the smoothers of the Soundgenertors B module,
 *            depending if the corresponding bit of the mask is set to 1
******************************************************************************/

inline void Soundgenerator::applyBSmoother()
{
    //*************************** ID 1: Pm Self *****************************//
    if (mModuleB_PmSelf_ramp < 1.0)
    {
        mModuleB_PmSelf_ramp += SMOOTHER_INC;

        if (mModuleB_PmSelf_ramp > 1.0)
        {
            mModuleB_PmSelf = mModuleB_PmSelf_target;
            mModuleB_SmootherMask &= 0xFFFE;       // switch first bit to 0
        }
        else
        {
            mModuleB_PmSelf = mModuleB_PmSelf_base + mModuleB_PmSelf_diff * mModuleB_PmSelf_ramp;
        }
    }


    //*********************** ID 2: PmSelf Shaper ***************************//
    if (mModuleB_PmSelfShaper_ramp < 1.0)
    {
        mModuleB_PmSelfShaper_ramp += SMOOTHER_INC;

        if (mModuleB_PmSelfShaper_ramp > 1.0)
        {
            mModuleB_PmSelfShaper = mModuleB_PmSelfShaper_target;
            mModuleB_SmootherMask &= 0xFFFD;       // switch second bit to 0
        }
        else
        {
            mModuleB_PmSelfShaper = mModuleB_PmSelfShaper_base + mModuleB_PmSelfShaper_diff * mModuleB_PmSelfShaper_ramp;
        }
    }


    //*************************** ID 3: Pm Cross ****************************//
    if (mModuleB_PmCross_ramp < 1.0)
    {
        mModuleB_PmCross_ramp += SMOOTHER_INC;

        if (mModuleB_PmCross_ramp > 1.0)
        {
            mModuleB_PmCross = mModuleB_PmCross_target;
            mModuleB_SmootherMask &= 0xFFFB;       // switch third bit to 0
        }
        else
        {
            mModuleB_PmCross = mModuleB_PmCross_base + mModuleB_PmCross_diff * mModuleB_PmCross_ramp;
        }
    }


    //********************** ID 4: Pm Cross Shaper **************************//
    if (mModuleB_PmCrossShaper_ramp < 1.0)
    {
        mModuleB_PmCrossShaper_ramp += SMOOTHER_INC;

        if (mModuleB_PmCrossShaper_ramp > 1.0)
        {
            mModuleB_PmCrossShaper = mModuleB_PmCrossShaper_target;
            mModuleB_SmootherMask &= 0xFFF7;       // switch fourth bit to 0
        }
        else
        {
            mModuleB_PmCrossShaper = mModuleB_PmCrossShaper_base + mModuleB_PmCrossShaper_diff * mModuleB_PmCrossShaper_ramp;
        }
    }


    //**************************** ID 5: Drive ******************************//
    if (mModuleB_Drive_ramp < 1.0)
    {
        mModuleB_Drive_ramp += SMOOTHER_INC;

        if (mModuleB_Drive_ramp > 1.0)
        {
            mModuleB_Drive = mModuleB_Drive_target;
            mModuleB_SmootherMask &= 0xFFEF;       // switch fifth bit to 0
        }
        else
        {
            mModuleB_Drive = mModuleB_Drive_base + mModuleB_Drive_diff * mModuleB_Drive_ramp;
        }
    }


    //**************************** ID 6: Fold *******************************//
    if (mModuleB_Fold_ramp < 1.0)
    {
        mModuleB_Fold_ramp += SMOOTHER_INC;

        if (mModuleB_Fold_ramp > 1.0)
        {
            mModuleB_Fold = mModuleB_Fold_target;
            mModuleB_SmootherMask &= 0xFFDF;       // switch sixth bit to 0
        }
        else
        {
            mModuleB_Fold = mModuleB_Fold_base + mModuleB_Fold_diff * mModuleB_Fold_ramp;
        }
    }


    //**************************** ID 7: Asym *******************************//
    if (mModuleB_Asym_ramp < 1.0)
    {
        mModuleB_Asym_ramp += SMOOTHER_INC;

        if (mModuleB_Asym_ramp > 1.0)
        {
            mModuleB_Asym = mModuleB_Asym_target;
            mModuleB_SmootherMask &= 0xFFBF;       // switch seventh bit to 0
        }
        else
        {
            mModuleB_Asym = mModuleB_Asym_base + mModuleB_Asym_diff * mModuleB_Asym_ramp;
        }
    }


    //************************* ID 8: Shaper Mix ****************************//
    if (mModuleB_ShaperMix_ramp < 1.0)
    {
        mModuleB_ShaperMix_ramp += SMOOTHER_INC;

        if (mModuleB_ShaperMix_ramp > 1.0)
        {
            mModuleB_ShaperMix = mModuleB_ShaperMix_target;
            mModuleB_SmootherMask &= 0xFF7F;       // switch eigth bit to 0
        }
        else
        {
            mModuleB_ShaperMix = mModuleB_ShaperMix_base + mModuleB_ShaperMix_diff * mModuleB_ShaperMix_ramp;
        }
    }


    //*********************** ID 9: Ring Modulation *************************//
    if (mModuleB_RingMod_ramp < 1.0)
    {
        mModuleB_RingMod_ramp += SMOOTHER_INC;

        if (mModuleB_RingMod_ramp > 1.0)
        {
            mModuleB_RingMod = mModuleB_RingMod_target;
            mModuleB_SmootherMask &= 0xFEFF;       // switch ninth bit to 0
        }
        else
        {
            mModuleB_RingMod = mModuleB_RingMod_base + mModuleB_RingMod_diff * mModuleB_RingMod_ramp;
        }
    }


    //************************ ID 10: Feedback Mix **************************//
    if (mModuleB_FeedbackMix_ramp < 1.0)
    {
        mModuleB_FeedbackMix_ramp += SMOOTHER_INC;

        if (mModuleB_FeedbackMix_ramp > 1.0)
        {
            mModuleB_FeedbackMix = mModuleB_FeedbackMix_target;
            mModuleB_SmootherMask &= 0xFDFF;       // switch tenth bit to 0
        }
        else
        {
            mModuleB_FeedbackMix = mModuleB_FeedbackMix_base
                    + mModuleB_FeedbackMix_diff * mModuleB_FeedbackMix_ramp;
        }
    }


    //************************** ID 11: Feedback ****************************//
    if (mModuleB_PmFeedback_ramp < 1.0)
    {
        mModuleB_PmFeedback_ramp += SMOOTHER_INC;

        if (mModuleB_PmFeedback_ramp > 1.0)
        {
            mModuleB_PmFeedback = mModuleB_PmFeedback_target;
            mModuleB_SmootherMask &= 0xFBFF;       // switch 11th bit to 0
        }
        else
        {
            mModuleB_PmFeedback = mModuleB_PmFeedback_base + mModuleB_PmFeedback_diff * mModuleB_PmFeedback_ramp;
        }
    }


    //************************ ID 14: PM Self Env B *************************//
    if (mModuleB_PmSelf_EnvB_Amnt_ramp < 1.0)
    {
        mModuleB_PmSelf_EnvB_Amnt_ramp += SMOOTHER_INC;

        if (mModuleB_PmSelf_EnvB_Amnt_ramp > 1.0)
        {
            mModuleB_PmSelf_EnvB_Amnt = mModuleB_PmSelf_EnvB_Amnt_target;
            mModuleB_SmootherMask &= 0xDFFF;       // switch 14th bit to 0
        }
        else
        {
            mModuleB_PmSelf_EnvB_Amnt = mModuleB_PmSelf_EnvB_Amnt_base + mModuleB_PmSelf_EnvB_Amnt_diff * mModuleB_PmSelf_EnvB_Amnt_ramp;
        }
    }


    //************************ ID 15: PM Cross Env A *************************//
    if (mModuleB_PmCross_EnvA_Amnt_ramp < 1.0)
    {
        mModuleB_PmCross_EnvA_Amnt_ramp += SMOOTHER_INC;

        if (mModuleB_PmCross_EnvA_Amnt_ramp > 1.0)
        {
            mModuleB_PmCross_EnvA_Amnt = mModuleB_PmCross_EnvA_Amnt_target;
            mModuleB_SmootherMask &= 0xBFFF;       // switch 15th bit to 0
        }
        else
        {
            mModuleB_PmCross_EnvA_Amnt = mModuleB_PmCross_EnvA_Amnt_base + mModuleB_PmCross_EnvA_Amnt_diff * mModuleB_PmCross_EnvA_Amnt_ramp;
        }
    }


    //********************** ID 16: PM Feedback Env C ************************//
    if (mModuleB_PmFeedback_EnvC_Amnt_ramp < 1.0)
    {
        mModuleB_PmFeedback_EnvC_Amnt_ramp += SMOOTHER_INC;

        if (mModuleB_PmFeedback_EnvC_Amnt_ramp > 1.0)
        {
            mModuleB_PmFeedback_EnvC_Amnt = mModuleB_PmFeedback_EnvC_Amnt_target;
            mModuleB_SmootherMask &= 0x7FFF;       // switch 16th bit to 0
        }
        else
        {
            mModuleB_PmFeedback_EnvC_Amnt = mModuleB_PmFeedback_EnvC_Amnt_base + mModuleB_PmFeedback_EnvC_Amnt_diff * mModuleB_PmFeedback_EnvC_Amnt_ramp;
        }
    }


    //************************* ID 17: Drive Env B ***************************//
    if (mModuleB_Drive_EnvB_Amnt_ramp < 1.0)
    {
        mModuleB_Drive_EnvB_Amnt_ramp += SMOOTHER_INC;

        if (mModuleB_Drive_EnvB_Amnt_ramp > 1.0)
        {
            mModuleB_Drive_EnvB_Amnt = mModuleB_Drive_EnvB_Amnt_target;
            mModuleB_SmootherMask &= 0xFFFEFFFF;       // switch 17th bit to 0
        }
        else
        {
            mModuleB_Drive_EnvB_Amnt = mModuleB_Drive_EnvB_Amnt_base + mModuleB_Drive_EnvB_Amnt_diff * mModuleB_Drive_EnvB_Amnt_ramp;
        }
    }


    //********************* ID 18: Feedback Mix Env C ************************//
    if (mModuleB_FeedbackMix_EnvC_Amnt_ramp < 1.0)
    {
        mModuleB_FeedbackMix_EnvC_Amnt_ramp += SMOOTHER_INC;

        if (mModuleB_FeedbackMix_EnvC_Amnt_ramp > 1.0)
        {
            mModuleB_FeedbackMix_EnvC_Amnt = mModuleB_FeedbackMix_EnvC_Amnt_target;
            mModuleB_SmootherMask &= 0xFFFDFFFF;       // switch 18th bit to 0
        }
        else
        {
            mModuleB_FeedbackMix_EnvC_Amnt = mModuleB_FeedbackMix_EnvC_Amnt_base + mModuleB_FeedbackMix_EnvC_Amnt_diff * mModuleB_FeedbackMix_EnvC_Amnt_ramp;
        }
    }
}



/******************************************************************************/
/** @brief    frequency calculation depending on the incoming pitch,
 *            key tracking amount and pitch offset
 *  @param    incoming pitch - 60ct
 *  @param    key tracking amount
 *  @param    pitch offset
 *  @return   frequency in Hz
*******************************************************************************/

inline float Soundgenerator::setOscFreq(float _pitch, float _keyTracking, float _pitchOffset)
{
    _pitch = _pitch * _keyTracking + _pitchOffset;

    if (_pitch < 0.f)
    {
        if (_pitch > -20.f)
        {
            _pitch = ((300.f/13.f) * _pitch) / ((280.f/13.f) + _pitch);
        }

        else if (_pitch < -20.f)
        {
            _pitch = -300.f;
        }
    }

    return NlToolbox::Conversion::pitch2freq(_pitch);
}



/******************************************************************************/
/** @brief      Sets the Phase increment for the Oscillator, dependant on the
 *              following incoming parameters
 *  @param      Oscillator Frequency
 *  @param      Fluctuation Amount
 *  @param      Random Value previously set by active voice number (seed)
 *  @return     Phase increment
*******************************************************************************/

inline float Soundgenerator::setPhaseInc(float _oscFreq, float _fluctAmnt, int32_t& _randVal)
{
    _randVal = _randVal * 1103515245 + 12345;

    float modFreq = static_cast<float>(_randVal) * 4.5657f * pow(10.f, -10.f);
    modFreq = modFreq * _fluctAmnt * _oscFreq;

    return (_oscFreq + modFreq) / SAMPLERATE;
}
