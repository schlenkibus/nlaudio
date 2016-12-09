/******************************************************************************/
/** @file		biquadfilters.h
    @date		2016-06-28
    @version	0.2
    @author		Anton Schmied[2016-03-18]
    @brief		An implementation of Biquad Filters
                (Lowpass, Highpass, Low Shelf, High Shelf)
                as used in the C15 and implemented in Reaktor
*******************************************************************************/

#pragma once
#include "nltoolbox.h"

enum class BiquadFilterType             // Enum Class for filter types
{
    LOWPASS,
    HIGHPASS,
    LOWSHELF,
    HIGHSHELF
};

class BiquadFilters                     // Biquad Filters Class
{
public:

    BiquadFilters();                                // Deafult Constructor

    BiquadFilters(uint32_t _sampleRate,              	// Parameterized Constructor
                  float _cutFreq,
                  float _shelfAmp,
                  float _resonance,
                  BiquadFilterType _filterType);

    ~BiquadFilters(){}                              // Class Destructor


    void setCutFreq(float _cutFreq);
    void setShelfAmp(float _shelfAmp);
    void setResonance(float _resonance);
    void setFiltertype(BiquadFilterType _filterType);
    void setFilterParams(unsigned char _ctrlId, float _ctrlVal);

    float applyFilter(float _currSample);


private:

    void calcCoeff();
    inline void setAlpha();
    void resetStateVariables();

    float mSampleRate;						// samplerate

    float mShelfAmp;						// normalized shelf amplification
    float mResonance;						// filter resonance

    float mOmegaCos;                        // cosine of the warped frequency
    float mOmegaSin;            			// sine of the warped frequency
    float mAlpha;							// a product of omega_sin and resonance
    float mBeta;							// helper variable when the shelf amp is set

    float mInStateVar1;          			// first state variable of the incoming sample
    float mInStateVar2;          			// second state variable of the incoming sample
    float mOutStateVar1;         			// first state variable of the processed sample
    float mOutStateVar2;         			// second state variable of the processed sample

    float mB0, mB1, mB2, mA0, mA1, mA2;		// Filter Coefficients

    BiquadFilterType mFilterType;        	// Filtertype
    uint32_t mFilterCounter;                 // counter variable if the module is used with all filtertypes
											// (0 = lowpass, 1 = highpass, 2 = lowshelf, 3 = highshelf)

    enum CtrlId: unsigned char          	// Enum class for control IDs (Korg Nano Kontrol I)
    {
        CUTFREQ    = 0x12,
        SHELFAMP   = 0x06,
        RESONANCE  = 0x13,
        FILTERTYPE = 0x1B
    };
};
