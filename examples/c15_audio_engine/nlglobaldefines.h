/******************************************************************************/
/** @file		nlglobaldefines.h
    @date		2017-03-16
    @version	0.1
    @author		Anton Schmied[2017-03-16]
    @brief		A collection of global Defines and Setting
*******************************************************************************/

#pragma once


//************************ Controller define - temporal ******************************//

#define REMOTE61
//#define NANOKONTROL_I


//*********************** Number of Voices and Channels ******************************//

#define NUM_VOICES 12
#define NUM_CHANNELS 2
#define NUM_SIGNALS 400


//****************************** Smoother Objects ************************************//

//#define SMOOTHEROBJ

//********************************* Constants ****************************************//

#ifndef DNC_CONST
#define DNC_CONST 1.e-18f
#endif

#ifndef SAMPLERATE
#define SAMPLERATE 48000.f
#endif

const float SMOOTHER_INC  = 5.f / (SAMPLERATE * 0.032f);
const float REVERB_SMOOTHER_INC = 1.f / (SAMPLERATE * 0.001f * 50.f);

const float CONST_PI = 3.14159f;
const float CONST_HALF_PI = 1.5708;
const float CONST_DOUBLE_PI = 6.28318f;
const float WARPCONST_2PI = 6.28319f / SAMPLERATE;
const float WARPCONST_PI  = 3.14159f / SAMPLERATE;
const float SAMPLE_INTERVAL = 1.f / SAMPLERATE;

const float FREQCLIP_MIN_1 = SAMPLERATE / 24000.f;
const float FREQCLIP_MIN_2 = SAMPLERATE / 24576.f;
const float FREQCLIP_MAX_1 = SAMPLERATE / 4.f;
const float FREQCLIP_MAX_2 = SAMPLERATE / 3.f;
const float FREQCLIP_MAX_3 = SAMPLERATE / 2.18f;
const float FREQCLIP_MAX_4 = SAMPLERATE / 2.125f;
const float FREQCLIP_MAX_5 = SAMPLERATE / 2.f;


//*************************** Param Engine ***********************************//
/****************************** Array Solution *********************************
 *
 *  float POLY_SIGNALDATA[M][N]
 *  M -> Signal ID
 *  N -> Voice Number
 *
*******************************************************************************/
#include <array>

extern float PARAMSIGNALDATA[NUM_VOICES][NUM_SIGNALS];
//float DUAL_SIGNALDATA[NUM_CHANNELS][NUM_SIGNALS];


/****************************** Array Solution *********************************
 *
 *  struct SIGNALDATA
 *  N -> Voice Number
 *
*******************************************************************************/
#if 0
struct SIGNALDATA
{
    //************************ Envelope Signals ******************************//
    float EnvA_Gain[NUM_VOICES];
    float EnvA_Gate[NUM_VOICES];
    float EnvB_Gain[NUM_VOICES];
    float EnvB_Gate[NUM_VOICES];
    float EnvC[NUM_VOICES];
    float EnvG[NUM_VOICES];

    //************************** Osc A Signals *******************************//
    float OscA_Freq[NUM_VOICES];
    float OscA_Fluct[NUM_VOICES];
    float OscA_PMs[NUM_VOICES];
    float OscA_PMs_Shp[NUM_VOICES];
    float OscA_PMb[NUM_VOICES];
    float OscA_PMb_Shp[NUM_VOICES];
    float OscA_PMfb[NUM_VOICES];
    float OscA_Phase[NUM_VOICES];
    float OscA_Retrig[NUM_VOICES];
    float OscA_ChirpFreq[NUM_VOICES];

    //************************ Shaper A Signals ******************************//
    float ShpA_Drive[NUM_VOICES];
    float ShpA_Fold[NUM_VOICES];
    float ShpA_Asym[NUM_VOICES];
    float ShpA_Curve[NUM_VOICES];
    float ShpA_Mix[NUM_VOICES];
    float ShpA_FB_Mix[NUM_VOICES];
    float ShpA_RingMod[NUM_VOICES];

}renderedSignals;
#endif
