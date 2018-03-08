/******************************************************************************/
/** @file		voicemanager.h
    @date		2016-07-26
    @version	0.1
    @author		Anton Schmied[2016-07-26]
    @brief		..

    @todo       Soft Clip: Saturation and Volume
                Parameterized Contructor
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "nlglobaldefines.h"

#include "paramengine.h"

#include "biquadfilters.h"
#include "test_envelopes.h"
#include "soundgenerator.h"
#include "combfilter.h"
#include "statevariablefilter.h"
#include "feedbackmixer.h"
#include "outputmixer.h"
#include "cabinet.h"
#include "gapfilter.h"
#include "flanger.h"
#include "echo.h"
#include "reverb.h"

#include <vector>
#include <array>

class VoiceManager{
public:
    VoiceManager();                 // Default Constructor

    ~VoiceManager();                // Destructor

    float mainOut_L, mainOut_R;     // fully processed samples

    void voiceLoop();
    void evalMidiEvents(unsigned char _instrID, unsigned char _ctrlID, float _ctrlVal);
    void evalTCDEvents(unsigned char _status, unsigned char _data_0, unsigned char _data_1);

    void flushAllBuffer();
private:

    //************************ Fadepoint Lowpass *********************************//
    //**************** Fadepoint Lowpass// Raised Cosine *************************//
    BiquadFilters* pFadepointLowpass;
    bool mFlushNow;
    float mFadepoint;
    uint32_t mFadepointCounter;

    std::array<float, FADE_SAMPLES > mRaisedCosineTable;


    //*************************** Envelopes ***************************************//

    Test_Envelopes* pTest_Envelopes[NUM_VOICES];


    //******************** Sound Generating Modules *******************************//

    Soundgenerator* pSoundGenerator[NUM_VOICES];
    CombFilter* pCombFilter[NUM_VOICES];
    StateVariableFilter* pSVFilter[NUM_VOICES];
    FeedbackMixer* pFeedbackMixer[NUM_VOICES];


    //***************************** Mixers ****************************************//

    Outputmixer* pOutputMixer;


    //**************************** Effects ****************************************//

    Flanger* pFlanger;
    Cabinet* pCabinet_L;
    Cabinet* pCabinet_R;
    GapFilter* pGapFilter;
    Echo* pEcho;
    Reverb* pReverb;


    //************************ Voice Allocation ***********************************//

    void vallocInit();
    void vallocProcess(uint32_t _keyDirection, float _pitch, float _velocity);

    int32_t vVoiceState[NUM_VOICES] = {};                      // which voices are active? 1 - on, 0 - off

    uint32_t vOldestAssigned;
    uint32_t vYoungestAssigned;
    uint32_t vNextAssigned[NUM_VOICES] = {};                    // array with the next elements per voice
    uint32_t vPreviousAssigned[NUM_VOICES] = {};

    uint32_t vNumAssigned;

    uint32_t vOldestReleased;                                   // index of the earliest disabled voice (full use: earliest turned on)
    uint32_t vYoungestReleased;                                 // index of the last disabled voice (full use: last turned on)
    uint32_t vNextReleased[NUM_VOICES] = {};



    //************************* Instrument IDs ************************************//

    enum InstrID: unsigned char
    {
#ifdef REMOTE61                         // novation ReMOTE61
        KEYUP_0             = 0x80,
        KEYUP_1             = 0x81,
        KEYUP_2             = 0x82,
        KEYUP_3             = 0x83,
        KEYUP_4             = 0x84,
        KEYUP_5             = 0x85,
        KEYUP_6             = 0x86,
        KEYUP_7             = 0x87,
        KEYUP_8             = 0x88,
        KEYUP_9             = 0x89,
        KEYUP_10            = 0x8A,
        KEYUP_11            = 0x8B,
        KEYUP_12            = 0x8C,
        KEYUP_13            = 0x8D,
        KEYUP_14            = 0x8E,
        KEYUP_15            = 0x8F,

        KEYDOWN_0           = 0x90,
        KEYDOWN_1           = 0x91,
        KEYDOWN_2           = 0x92,
        KEYDOWN_3           = 0x93,
        KEYDOWN_4           = 0x94,
        KEYDOWN_5           = 0x95,
        KEYDOWN_6           = 0x96,
        KEYDOWN_7           = 0x97,
        KEYDOWN_8           = 0x98,
        KEYDOWN_9           = 0x99,
        KEYDOWN_10          = 0x9A,
        KEYDOWN_11          = 0x9B,
        KEYDOWN_12          = 0x9C,
        KEYDOWN_13          = 0x9D,
        KEYDOWN_14          = 0x9E,
        KEYDOWN_15          = 0x9F,

        ENEVELOPES_PARAM    = 0xB0,

        OSC_A_PARAM         = 0xB1,
        OSC_B_PARAM         = 0xB2,
        SHAPER_A_PARAM      = 0xB3,
        SHAPER_B_PARAM      = 0xB4,

        COMBFILTER_PARAM    = 0xB5,
        STATEVARIABLE_PARAM = 0xB6,

        FEEDBACKMIXER_PARAM = 0xB7,
        OUTPUTMIXER_PARAM   = 0xB8,

        FLANGER_PARAM       = 0xB9,
        CABINET_PARAM       = 0xBA,
        GAP_PARAM           = 0xBB,
        ECHO_PARAM          = 0xBC,
        REVERB_PARAM        = 0xBD,

        MASTER_PARAM        = 0xBE,

        PARAM_ENGINE        = 0xF

#else
        KEYUP_0,
        KEYUP_1,
        KEYUP_2,
        KEYUP_3,
        KEYUP_4,
        KEYUP_5,
        KEYUP_6,
        KEYUP_7,
        KEYUP_8,
        KEYUP_9,
        KEYUP_10,
        KEYUP_11,
        KEYUP_12,
        KEYUP_13,
        KEYUP_14,
        KEYUP_15,

        KEYDOWN_0,
        KEYDOWN_1,
        KEYDOWN_2,
        KEYDOWN_3,
        KEYDOWN_4,
        KEYDOWN_5,
        KEYDOWN_6,
        KEYDOWN_7,
        KEYDOWN_8,
        KEYDOWN_9,
        KEYDOWN_10,
        KEYDOWN_11,
        KEYDOWN_12,
        KEYDOWN_13,
        KEYDOWN_14,
        KEYDOWN_15,

        ENEVELOPES_PARAM,

        OSC_A_PARAM,
        OSC_B_PARAM,
        SHAPER_A_PARAM,
        SHAPER_B_PARAM,

        COMBFILTER_PARAM,
        STATEVARIABLE_PARAM,

        FEEDBACKMIXER_PARAM,
        OUTPUTMIXER_PARAM,

        FLANGER_PARAM,
        CABINET_PARAM,
        GAP_PARAM,
        ECHO_PARAM,
        REVERB_PARAM,

        MASTER_PARAM
#endif
    };
};




