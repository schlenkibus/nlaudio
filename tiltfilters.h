/******************************************************************************/
/** @file		tiltfilters.h
    @date		2016-06-28
    @version	0.2
    @author		Anton Schmied [2016-03-18]
    @brief		An implementation of Biquad Tilt Filters
                (Lowpass, Highpass, Low Shelf, High Shelf)
                as used in the C15 and implemented in Reaktor
*******************************************************************************/

#pragma once
#include "nltoolbox.h"

enum class TiltFilterType                       // Enum Class for filter Types
{
    LOWPASS,
    HIGHPASS,
    LOWSHELF,
    HIGHSHELF
};

class TiltFilters                               // Tilt Filters Class
{
public:

    TiltFilters();                                  // Deafult Constructor

    TiltFilters(uint32_t _sampleRate,                    // Parameterized Constructor
                float _cutFreq,
                float _tilt,
                float _slopeWidth,
                float _resonance,
                TiltFilterType _filterType);

    ~TiltFilters(){}                                // Class Destructor


    void setCutFreq(float _cutFreq);
    void setTilt(float _tilt);
    void setResonance(float _resonance);
    void setSlopeWidth(float _slopeWidth);
    void setFilterType(TiltFilterType _filterType);
    void setFilterParams(unsigned char _ctrlId, float _ctrlVal);

    float applyFilter(float _currSample);


private:

    void calcCoeff();
    void setAlpha();
    void resetStateVariables();

    float mSampleRate;                   // samplerate

    float mResonance;                    // filter resonance
    float mSlopeWidth;                   // slope width
    float mTilt;                         // normalized tilt amount

    float mOmegaCos;                     // cosine of the warped frequency
    float mOmegaSin;                     // sine of the warped frequency
    float mAlpha;                        // a product of omega_sin and resonance
    float mBeta;                         // helper variable when tilt is set

    float mInStateVar1;                  // first state variable of the incoming sample
    float mInStateVar2;                  // second state variable of the incoming sample
    float mOutStateVar1;                 // first state variable of the processed sample
    float mOutStateVar2;                 // second state variable of the processed sample

    float mB0, mB1, mB2, mA0, mA1, mA2;  // Filter Coefficients

    TiltFilterType mFilterType;          // FilterType
    uint32_t mFilterCounter;              // counter variable if the module is used with all filterTypes (0 = lowpass, 1 = highpass, 2 = lowshelf, 3 = highshelf)

    enum CtrlId: unsigned char           // enum class for control IDs (KORG Nano Kontrol I)
    {
        CUTFREQ    = 0x12,
        TILT       = 0x06,
        RESONANCE  = 0x13,
        SLOPEWIDTH = 0x08,
        FILTERTYPE = 0x1B
    };
};
