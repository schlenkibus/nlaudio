/******************************************************************************/
/** @file		cabinet.h
    @date		2017-05-11
    @version	1.0
    @author		Anton Schmied[2016-03-18]
    @brief		An implementation of the Cabinet Effect
                as used in the C15 and implemented in Reaktor
*******************************************************************************/

#pragma once

#include "nlglobaldefines.h"
#include "nltoolbox.h"
#include "biquadfilters.h"
#include "tiltfilters.h"


class Cabinet                               // Cabinet Class
{
public:

    Cabinet();                              // Default Constructor

    Cabinet(float _drive,                   // Parameterized Constructor
            float _tilt,
            float _hiCut,
            float _loCut,
            float _fold,
            float _asym,
            float _cabLvl,
            float _mix);

    ~Cabinet();                             // Class Destructor

    float mCabinetOut;                      // public processed sample

    void setCabinetParams(unsigned char _ctrlId, float _ctrlVal);       // sets the effect parameters and activates smoothers

    void applyCab(float _rawSample);        // main audio function, which processes the incoming sample

private:

    //******************************** Control Variabels ********************************//

    float mDrive;                  // drive level
    float mCabLvl;                 // cabinet level
    float mWet;                    // wet amount, dependant on mix and cabinet level
    float mDry;                    // dry amount, dependant on mix and cabinet level
    float mFold;                   // fold amount
    float mAsym;                   // asym amount

    float mSaturation;             // saturation amount
    float mSaturationConst;        // constant resulting from (0.1588f / mSaturation) - used in the SineShaper


    //********************************* Cabinet Filters *********************************//

    BiquadFilters* pHighpass;       // first highpass
    BiquadFilters* pLowpass_1;      // first lowpass
    BiquadFilters* pLowpass_2;      // second lowpass
    TiltFilters* pLowshelf_1;       // first lowshelf
    TiltFilters* pLowshelf_2;       // second lowshelf
    NlToolbox::Filters::Highpass30Hz* pHighpass30Hz;        // 1-Pole 30Hz Highpass for Smoothing within the sineShaper function


    //******************************** Smoothing Defines ********************************//

    inline void applyCabSmoother();

    uint32_t mSmootherMask;                 // Smoother Mask (ID 1: dry, ID 2: wet, ID 3: dry)

    // Mask ID: 1
    float mDry_base;
    float mDry_target;
    float mDry_diff;
    float mDry_ramp;

    // Mask ID: 2
    float mWet_base;
    float mWet_target;
    float mWet_diff;
    float mWet_ramp;

    // Mask ID: 3
    float mDrive_base;
    float mDrive_target;
    float mDrive_diff;
    float mDrive_ramp;


    //****************************** Control IDs ******************************//

    enum CtrlId: unsigned char
    {
#ifdef NANOKONTROL_I              // Korg Nano Kontrol I
        HICUT    = 0x3D,
        LOCUT    = 0x3E,
        MIX      = 0x32,
        CABLEVEL = 0x37,
        DRIVE    = 0x36,
        TILT     = 0x35,
        FOLD     = 0x33,
        ASYM     = 0x34
#endif
#ifdef REMOTE61                   // ReMote 61
        DRIVE    = 0x29,
        FOLD     = 0x2A,
        ASYM     = 0x2B,
        TILT     = 0x2C,
        HICUT    = 0x2D,
        LOCUT    = 0x2E,
        CABLEVEL = 0x2F,
        MIX      = 0x30
#endif
    };
};
