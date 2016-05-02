/**
 * @file       onepolefilters.h
 * @date       2016-03-18
 * @brief      this class is an implementation of 1-Pole Filters (LP, HP, LS, HS) as found in Reaktor
 * @author     Anton Schmied [date of file creation 2016-03-18]
*/

#pragma once
#include "tools.h"

/** Enum Class for filter types */
enum class OnePoleFiltertype
{
    lowpass,
    highpass,
    lowshelf,
    highshelf
};

class OnePoleFilters{
    public:
	/** OnePoleFilters Constructor with default values */
    OnePoleFilters(int _sRate,
                   float _cutFreq = 22000.f,
                   float _shelfAmp = 0.f,
                   OnePoleFiltertype _filtertype = OnePoleFiltertype::lowpass)
        : sRate(static_cast<float>(_sRate))
        , inCh1Delay1(0.f)
        , outCh1Delay1(0.f)
        , inCh2Delay1(0.f)
        , outCh2Delay1(0.f)
    {
        setCutFreq(_cutFreq);
        setShelfAmp(_shelfAmp);
        setFiltertype(_filtertype);
    }

    ~OnePoleFilters(){}

	/** @brief    sets cut frequency
     *  @param    cut frequency in Hz
    */
    void setCutFreq(float _cutFreq)
    {      
        switch(filtertype)
        {
            case OnePoleFiltertype::lowshelf:
            _cutFreq /= shelfAmp;
            break;

            case OnePoleFiltertype::highshelf:
            _cutFreq *= shelfAmp;
            break;

            case OnePoleFiltertype::highpass:
            break;

            case OnePoleFiltertype::lowpass:
            break;
        }

        if (_cutFreq < (sRate / 24000.f))							//clipping check
        {
            _cutFreq = sRate / 24000.f;
        }

        if (_cutFreq > (sRate / 2.18f))
        {
            _cutFreq = sRate / 2.18f;
        }

        _cutFreq *= (M_PI / sRate);                //Frequency warp
        omega_tan = Nl::tan(_cutFreq);             //alternative to tan(cutFreq) -> tools.h;

        calcCoeff();
    }

	/** @brief    sets shelf amplification
     *  @param    shelf amplification in dB
    */
    void setShelfAmp(float _shelfAmp)
    {
        shelfAmp = pow(1.059f, _shelfAmp);         //alternative to pow(10, (_shelfAmp / 40.f));
        shelfAmp_square = shelfAmp * shelfAmp;

        calcCoeff();
    }

	/** @brief    sets filter type
     *  @param    filter type <OnePoleFiltertype>
    */
    void setFiltertype(OnePoleFiltertype _filtertype)
    {
        filtertype = _filtertype;
        resetDelays();
        calcCoeff();
    }

	/** @brief    applies the specified filter to the incoming sample depending on the channel
     *  @param    raw Sample
     *  @param    channel index, 0 - Left, 1 - Right
     *  @return   processed sample
    */
    float applyFilter(float currSample, unsigned int chInd)
    {
        float output = 0.f;

        if(chInd == 0)
        {
            output += b0 * currSample;
            output += b1 * inCh1Delay1;
            output += a1 * outCh1Delay1;

            inCh1Delay1 = currSample;
            outCh1Delay1 = output;
        }

        else if (chInd == 1)
        {
            output += b0 * currSample;
            output += b1 * inCh2Delay1;
            output += a1 * outCh2Delay1;

            inCh2Delay1 = currSample;
            outCh2Delay1 = output;
        }

        return output;
    }

    /** @brief    interface method which converts and scales the incoming midi values and passes these to the respective methods
     *  @param    midi control value [0 ... 127]
     *  @param    midi control ID
    */
    void setFilterParams(float ctrlVal, unsigned char ctrlID)
    {
        switch (ctrlID)
        {
        case CtrlID::Cutfreq:
            ctrlVal = 20.f * pow(2.f, ctrlVal / 12.75f);                    //Midi to Freq [20Hz .. 19930Hz]
            setCutFreq(ctrlVal);
            break;

        case CtrlID::Shelfamp:
            ctrlVal = ((ctrlVal - 64.f) * 12.f) / 64.f;                     //Midi to [-12dB .. 12dB] linear
            setShelfAmp(ctrlVal);
            break;

        case CtrlID::Filtertype:
            static int counter = 0;

            if (static_cast<int>(ctrlVal) > 0)
            {
                ++counter;
                if (counter > 3) { counter = 0;}
                switch(counter)
                {
                    case 0:
                    setFiltertype(OnePoleFiltertype::lowpass);
                    printf("lowpass on\n");
                    break;

                    case 1:
                    setFiltertype(OnePoleFiltertype::highpass);
                    printf("highpass on\n");
                    break;

                    case 2:
                    setFiltertype(OnePoleFiltertype::lowshelf);
                    printf("lowshelf on\n");
                    break;

                    case 3:
                    setFiltertype(OnePoleFiltertype::highshelf);
                    printf("highshelf on\n");
                    break;
                }
            }
            break;
        }
    }

private:

    float omega_tan;					/**< tangent of the warped frequency */
    float shelfAmp;						/**< normalized shelf amplification */
    float shelfAmp_square;				/**< helper variable when the shelf amp is set */
    float sRate;						/**< samplerate */

    float inCh1Delay1;					/**< Channel 1 Input Delay 1 */
    float outCh1Delay1;					/**< Channel 1 Output Delay 1 */
    float inCh2Delay1;					/**< Channel 2 Input Delay 1 */
    float outCh2Delay1;					/**< Channel 2 Output Delay 1 */

    float b0, b1, a1;               	/**< Filter Coefficients */

    OnePoleFiltertype filtertype;

    enum CtrlID: unsigned char          /**< Enum class for control IDs Korg Nano Kontrol I*/
    {
        Cutfreq    = 0x12,
        Shelfamp   = 0x06,
        Filtertype = 0x1B
    };

    /** @brief    calculates the coefficients depending on the chosen filter type
    */
    void calcCoeff()
    {
        switch(filtertype)
        {
            case OnePoleFiltertype::lowpass:
            a1 = (1.f - omega_tan) / (1.f + omega_tan);
            b0 = omega_tan / (1.f + omega_tan);
            b1 = omega_tan / (1.f + omega_tan);
            break;

            case OnePoleFiltertype::highpass:
            a1 = (1.f - omega_tan) / (1.f + omega_tan);
            b0 = 1.f / (1.f + omega_tan);
            b1 = (1.f / (1.f + omega_tan)) * -1.f;
            break;

            case OnePoleFiltertype::lowshelf:
            a1 = (1.f - omega_tan) / (1.f + omega_tan);
            b0 = ((omega_tan / (1.f + omega_tan)) * (shelfAmp_square + -1.f)) + 1.f;
            b1 = ((omega_tan / (1.f + omega_tan)) * (shelfAmp_square + -1.f)) - a1;
            break;

            case OnePoleFiltertype::highshelf:
            a1 = (1.f - omega_tan) / (1.f + omega_tan);
            b0 = ((shelfAmp_square + -1.f) / (1.f + omega_tan)) + 1.f;
            b1 = (((shelfAmp_square + -1.f) / (1.f + omega_tan)) + a1) * -1.f;
            break;
        }
    }

    /** @brief    resetes the delays if the filter type is changed
    */	
    void resetDelays()
    {
        inCh1Delay1 = 0.f;          //Channel 1
        outCh1Delay1 = 0.f;

        inCh2Delay1 = 0.f;          //Channel 2
        outCh2Delay1 = 0.f;
    }
};


