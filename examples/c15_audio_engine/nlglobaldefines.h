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
const float WARPCONST_2PI = 6.28319f / SAMPLERATE;
const float WARPCONST_PI  = 3.14159f / SAMPLERATE;
const float SAMPLING_INTERVAL = 1.f / SAMPLERATE;

const float FREQCLIP_2HZ = SAMPLERATE / 24576.f;
const float FREQCLIP_12000HZ = SAMPLERATE / 4.f;
const float FREQCLIP_14700HZ = SAMPLERATE / 3.f;
const float FREQCLIP_22000HZ = SAMPLERATE / 2.125f;
const float FREQCLIP_22050HZ = SAMPLERATE / 2.f;

