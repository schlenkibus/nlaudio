/******************************************************************************/
/** @file		onepolefilters.h
    @date		2016-06-28
    @version	0.2
    @author		Anton Schmied[2016-03-18]
    @brief		An implementation of 1-Pole Filters
                (Lowpass, Highpass, Low Shelf, High Shelf)
                as used in the C15 and implemented in Reaktor
*******************************************************************************/

#pragma once
#include "toolbox.h"

enum class OnePoleFilterType            // Enum Class for filter Types
{
    LOWPASS,
    HIGHPASS,
    LOWSHELF,
    HIGHSHELF
};

class OnePoleFilters                // 1-Pole Filters Class
{
public:

    OnePoleFilters();                               // Default Constructor

    OnePoleFilters(int _sampleRate,                 // Parameterized Constructor
                   float _cutFreq,
                   float _shelfAmp,
                   OnePoleFilterType _filterType);

    ~OnePoleFilters(){}                             // Class Destructor


    void setCutFreq(float _cutFreq);
    void setShelfAmp(float _shelfAmp);
    void setFilterType(OnePoleFilterType _filterType);
    void setFilterParams(float _ctrlVal, unsigned char _ctrlId);

    float applyFilter(float _currSample);


private:

    void calcCoeff();
    void resetStateVariables();

    float mSampleRate;                              // samplerate
    float mShelfAmp;                                // normalized shelf amplification

    float mOmegaTan;                                // tangent of the warped frequency
    float mShelfAmpSquare;                          // helper variable when the shelf amp is set

    float mInStateVar;                              // state variable of the incoming sample
    float mOutStateVar;                             // state variable of the processed sample

    float mB0, mB1, mA1;                            // Filter Coefficients

    OnePoleFilterType mFilterType;                  // FilterType
    unsigned int mFilterCounter;                    // counter variable if the module is used with all filterTypes
                                                    // (0 = lowpass, 1 = highpass, 2 = lowshelf, 3 = highshelf)

    enum CtrlId: unsigned char                      // Enum for control IDs (Korg Nano Kontrol I)
    {
        CUTFREQ    = 0x12,
        SHELFAMP   = 0x06,
        FILTERTYPE = 0x1B
    };
};


