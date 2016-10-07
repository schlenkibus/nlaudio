/******************************************************************************/
/** @file		cabinet.h
    @date		2016-07-01
    @version	0.2
    @author		Anton Schmied[2016-03-18]
    @brief		An implementation of the Cabinet Effect
                as used in the C15 and implemented in Reaktor
*******************************************************************************/

#pragma once

#include "math.h"
#include "nltoolbox.h"
#include "biquadfilters.h"
#include "tiltfilters.h"
#include "smoother.h"

class Cabinet                               // Cabinet Class
{
public:

    Cabinet();                              // Default Constructor

    Cabinet(int _sampleRate,                // Parameterized Constructor
            float _drive,
            float _tilt,
            float _hiCut,
            float _loCut,
            float _fold,
            float _asym,
            float _cabLvl,
            float _mix);

    ~Cabinet(){}                            // Class Destructor


    inline void setFold(float _fold);
    inline void setAsym(float _asym);
    void setTilt(float _tilt);
    inline void setSaturation(float _tilt);

    void setMix(float _mix);
    void setCabLvl(float _cabLvl);
    void setDrive(float _drive);
    void setHiCut(float _hiCut);
    void setLoCut(float _loCut);

    void setCabinetParams(float _ctrlVal, unsigned char _ctrlTag);

    float applyCab(float _currSample);
    float sineShaper(float _currSample);


private:

    float mSampleRate;             // samplerate

    float mDrive;                  // drive level [0 .. 50] dB, default 20dB
    float mCabLvl;                 // cabinet level [-50 .. 0] dB, default -14dB
    float mMix;                    // mix amount [0 .. 1], default 0.0
    float mFold;                   // fold amount [0 .. 1], default 0.25
    float mAsym;                   // asym amount [0 .. 1], default 0.1

    float mSaturation;             // saturation amount
    float mWet;                    // wet amount
    float mDry;                    // dry amount

    BiquadFilters mHighpass;       // first highpass
    BiquadFilters mLowpass1;       // first lowpass
    BiquadFilters mLowpass2;       // second lowpass
    TiltFilters mLowshelf1;        // first lowshelf
    TiltFilters mLowshelf2;        // second lowshelf
    NlToolbox::Filters::Highpass30Hz mHighpass30Hz;        // 1-Pole 30Hz Highpass for Smoothing within the sineShaper function

    Smoother mDrySmoother;         // dry level smoother
    Smoother mWetSmoother;         // wet level smoother
    Smoother mDriveSmoother;       // drive level smoother

    enum CtrlId: unsigned char    // Enum class for control IDs (Korg Nano Kontrol I)
    {
        // Enum class for control IDs (Korg Nano Kontrol I)

//        HICUT    = 0x3D,
//        LOCUT    = 0x3E,
//        MIX      = 0x32,
//        CABLEVEL = 0x37,
//        DRIVE    = 0x36,
//        TILT     = 0x35,
//        FOLD     = 0x33,
//        ASYM     = 0x34

        // Enum class for control IDs (ReMote 61)

        HICUT    = 0x2E,
        LOCUT    = 0x2F,
        MIX      = 0x29,
        CABLEVEL = 0x30,
        DRIVE    = 0x2D,
        TILT     = 0x2C,
        FOLD     = 0x2A,
        ASYM     = 0x2B

    };
};
