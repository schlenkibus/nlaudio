/******************************************************************************/
/** @file		nlglobaldefines.h
    @date		2017-03-16
    @version	0.1
    @author		Anton Schmied[2017-03-16]
    @brief		A collection of global Defines and Setting
*******************************************************************************/

#pragma once
#include <stdint.h>

//********************** Controller define - temporal ************************//

#define REMOTE61
//#define NANOKONTROL_I

#define SINGLARRAY
//#define GLBLARRAY
//#define NOARRAY

//**************** PARAM IDs in the ParamSignalData Array ********************//

#define ENV_A 0
#define ENV_B 2
#define ENV_C 4
#define ENV_GATE 5

//******************** Number of Voices and Channels *************************//

#define NUM_VOICES 12
#define NUM_CHANNELS 2
#define NUM_SIGNALS 400


//*************************** Smoother Objects *******************************//

//#define SMOOTHEROBJ


//***************************** Constants ************************************//

#ifndef DNC_CONST
#define DNC_CONST 1.e-18f
#endif

#ifndef SAMPLERATE
#define SAMPLERATE 48000.f
#endif

// const const expr anstatt const float -> eher für Templates, brauch ich Templates??
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

extern float PARAMSIGNALDATA[NUM_VOICES][NUM_SIGNALS];
//external float DUAL_SIGNALDATA[NUM_CHANNELS][NUM_SIGNALS];


#define STACKSINGLE
#ifdef STACKSINGLE
/******************************************************************************/
/** Singleton Class for a global ParamSignalData - Stack Version
*******************************************************************************/

class ParamSignalData
{
private:
    ParamSignalData(){}           // private contructor to prevent multiple instancing
    float mSignalArray[NUM_VOICES][NUM_SIGNALS];
    uint32_t val;

public:

    static ParamSignalData& instance()
    {
        static ParamSignalData _instance;
        return _instance;
    }

    ~ParamSignalData() {}


    inline float getSignalValue(uint32_t _vn, uint32_t _paramID)
    {
        return mSignalArray[_vn][_paramID];
    }

    inline void setSignalValue(uint32_t _vn, uint32_t _paramID, float _val)
    {
        mSignalArray[_vn][_paramID] = _val;
    }
};
#endif


//#define HEAPSINGLE
#ifdef HEAPSINGLE
/******************************************************************************/
/** Singleton Class for a global ParamSignalData - Heap Version
*******************************************************************************/

class ParamSignalData
{
private:
    static ParamSignalData* instance;

    ParamSignalData(){}

    float mSignalArray[NUM_VOICES][NUM_SIGNALS];
public:

    inline void setSignalValue(uint32_t _vn, uint32_t _paramID, float _val)
    {
        mSignalArray[_vn][_paramID] = _val;
    }

    inline float getSignalValue(uint32_t _vn, uint32_t _paramID)
    {
        return mSignalArray[_vn][_paramID];
    }

    static ParamSignalData* getInstance()
    {
        if (instance == 0)
        {
            instance = new ParamSignalData();
        }
        return instance;
    }
};
#endif
