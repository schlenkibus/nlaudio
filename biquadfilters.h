/**
 * @file       biquadfilters.h
 * @date       2016-03-18
 * @brief      this class is an implementation of Biquad Filters (LP, HP, LS, HS) as found in Reaktor
 * @author     Anton Schmied [date of file creation 2016-03-18]
*/

#pragma once
#include "tools.h"

/** Enum Class for filter types */
enum class BiquadFiltertype
{
    lowpass,
    highpass,
    lowshelf,
    highshelf
};

class BiquadFilters
{
public:
	/** BiquadFilters Constructor with default values */
    BiquadFilters(int _sRate,
                  float _cutFreq = 22000.f,
                  float _shelfAmp = 0.f,
                  float _resonance = 0.5f,
                  BiquadFiltertype _filtertype = BiquadFiltertype::lowpass)
        : sRate(static_cast<float>(_sRate))
        , inCh1Delay1(0.f)
        , inCh1Delay2(0.f)
        , outCh1Delay1(0.f)
        , outCh1Delay2(0.f)
        , inCh2Delay1(0.f)
        , inCh2Delay2(0.f)
        , outCh2Delay1(0.f)
        , outCh2Delay2(0.f)
        , b0(0.f)                   
        , b1(0.f)
        , b2(0.f)
        , a0(0.f)
        , a1(0.f)
        , a2(0.f)
    {
        setCutFreq(_cutFreq);
        setShelfAmp(_shelfAmp);
        setResonance(_resonance);
        setFiltertype(_filtertype);
    }

    ~BiquadFilters(){}

	/** @brief    sets cut frequency
     *  @param    cut frequency in Hz
    */
    void setCutFreq(float _cutFreq)
    {
        if (_cutFreq < (sRate / 24576.f))					//clipping check
        {
            _cutFreq = sRate / 24576.f;
        }

        if (_cutFreq > (sRate / 2.125f))
        {
            _cutFreq = sRate / 2.125f;
        }

        float omega = _cutFreq * (2.f * M_PI / sRate);     //Frequency to Omega (Warp)

        omega_cos = Nl::cos(omega);                        //alternative to cos(omega) -> tools.h 
        omega_sin = Nl::sin(omega);                        //alternative to sin(omega) -> tools.h

        setAlpha();
        calcCoeff();
    }

	/** @brief    sets shelf amplification
     *  @param    shelf amplification in dB
    */
    void setShelfAmp(float _shelfAmp)
    {
        shelfAmp = pow(1.059f, _shelfAmp);          		//alternative to pow(10, (_shelfAmp / 40.f))
        beta = 2.f * sqrt(shelfAmp);

        calcCoeff();
    }

	/** @brief    sets resonance
     *  @param    resonance
    */
    void setResonance(float _resonance)
    {
        resonance = _resonance;

        if (resonance > 0.999f)                     		//clipping check
        {
            resonance = 0.999f;
        }

        else if (resonance < -0.999f)
        {
            resonance = -0.999f;
        }

        setAlpha();
        calcCoeff();
    }

	/** @brief    sets filter type
     *  @param    filter type BiquadFiltertype
    */
    void setFiltertype(BiquadFiltertype _filtertype)
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
            output += b2 * inCh1Delay2;

            output += a1 * outCh1Delay1;
            output += a2 * outCh1Delay2;

            inCh1Delay2 = inCh1Delay1;
            inCh1Delay1 = currSample;

            outCh1Delay2 = outCh1Delay1;
            outCh1Delay1 = output;
        }

        else if (chInd == 1)
        {
            output += b0 * currSample;
            output += b1 * inCh2Delay1;
            output += b2 * inCh2Delay2;

            output += a1 * outCh2Delay1;
            output += a2 * outCh2Delay2;

            inCh2Delay2 = inCh2Delay1;
            inCh2Delay1 = currSample;

            outCh2Delay2 = outCh2Delay1;
            outCh2Delay1 = output;
        }
        return output;
    }

    /** @brief    interface method which converts and scales the incoming midi values and passes these to the respective methods
     *  @param    midi control value [0 ... 127]
     *  @param    midi control adress
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

        case CtrlID::Resonance:
            ctrlVal = (ctrlVal - 64.f) / 32.f;                              //Midi to [-1 .. 1]
            setResonance(ctrlVal);
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
                    setFiltertype(BiquadFiltertype::lowpass);
                    printf("lowpass on\n");
                    break;

                    case 1:
                    setFiltertype(BiquadFiltertype::highpass);
                    printf("highpass on\n");
                    break;

                    case 2:
                    setFiltertype(BiquadFiltertype::lowshelf);
                    printf("lowshelf on\n");
                    break;

                    case 3:
                    setFiltertype(BiquadFiltertype::highshelf);
                    printf("highshelf on\n");
                    break;
                }
            }
            break;
        }
    }

private:

    float omega_cos;			/**< cosine of the warped frequency */
    float omega_sin;			/**< sine of the warped frequency */
    float alpha;				/**< a product of omega_sin and resonance */
    float beta;					/**< helper variable when the shelf amp is set */
    float shelfAmp;				/**< normalized shelf amplification */
    float resonance;			/**< filter resonance */
    float sRate;				/**< samplerate */

    float inCh1Delay1;          /**< Channel 1 Input Delay 1 */
    float inCh1Delay2;			/**< Channel 1 Input Delay 2 */
    float outCh1Delay1;			/**< Channel 1 Output Delay 1 */	
    float outCh1Delay2;			/**< Channel 1 Output Delay 2 */ 

    float inCh2Delay1;          /**< Channel 2 Input Delay 1 */
    float inCh2Delay2;			/**< Channel 2 Input Delay 2 */
    float outCh2Delay1;			/**< Channel 2 Output Delay 1 */
    float outCh2Delay2;			/**< Channel 2 Output Delay 2 */

    float b0, b1, b2, a0, a1, a2;         /**< Filter Coefficients */

    BiquadFiltertype filtertype;

    enum CtrlID: unsigned char  /**< enums for control IDs Korg Nano Kontrol I*/
    {
        Cutfreq    = 0x12,
        Shelfamp   = 0x06,
        Resonance  = 0x13,
        Filtertype = 0x1B
    };

    /** @brief    calculates the coefficients depending on the chosen filter type
    */
    void calcCoeff() 
    {
        float coeff;
        switch(filtertype)
        {
            case BiquadFiltertype::lowpass:

            a0 = 1.f + alpha;
            a1 = omega_cos * -2.f;
            a2 = 1.f - alpha;
            b0 = (1.f - omega_cos) / 2.f;
            b1 = 1.f - omega_cos;
            b2 = b0;
            break;

            case BiquadFiltertype::highpass:

            a0 = 1.f + alpha;
            a1 = omega_cos * -2.f;
            a2 = 1.f - alpha;
            b0 = (1.f + omega_cos) / 2.f;
            b1 = (1.f + omega_cos) * -1.f;
            b2 = b0;
            break;

            case BiquadFiltertype::lowshelf:
            coeff = beta * alpha;

            a0 = (shelfAmp + 1.f) + (omega_cos * (shelfAmp - 1.f)) + coeff;
            a1 = ((shelfAmp - 1.f) + (omega_cos * (shelfAmp + 1.f))) * -2.f;
            a2 = (shelfAmp + 1.f) + (omega_cos * (shelfAmp - 1.f)) - coeff;
            b0 = ((shelfAmp + 1.f) - (omega_cos * (shelfAmp - 1.f)) + coeff) * shelfAmp;
            b1 = ((shelfAmp - 1.f) - (omega_cos * (shelfAmp + 1.f))) * 2.f * shelfAmp;
            b2 = ((shelfAmp + 1.f) - (omega_cos * (shelfAmp - 1.f)) - coeff) * shelfAmp;
            break;

            case BiquadFiltertype::highshelf:
            coeff = beta * alpha;

            a0 = (shelfAmp + 1.f) - (omega_cos * (shelfAmp - 1.f)) + coeff;
            a1 = ((shelfAmp - 1.f) - (omega_cos * (shelfAmp + 1.f))) * 2.f;
            a2 = (shelfAmp + 1.f) - (omega_cos * (shelfAmp - 1.f)) - coeff;
            b0 = ((shelfAmp + 1.f) + (omega_cos * (shelfAmp - 1.f)) + coeff) * shelfAmp;
            b1 = ((shelfAmp - 1.f) + (omega_cos * (shelfAmp + 1.f))) * -2.f * shelfAmp;
            b2 = ((shelfAmp + 1.f) + (omega_cos * (shelfAmp - 1.f)) - coeff) * shelfAmp;
            break;
        }

        a1 /= (-1.f * a0);              //normalize
        a2 /= (-1.f * a0);
        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
    }
	
    /** @brief    alpha calculation
    */
    inline void setAlpha()
    {
        alpha = omega_sin * (1.f - resonance);
    }
	
    /** @brief    resetes the delays if the filter type is changed
    */
    void resetDelays()
    {
        inCh1Delay1 = 0.f;          //Channel 1
        inCh1Delay2 = 0.f;
        outCh1Delay1 = 0.f;
        outCh1Delay2 = 0.f;

        inCh2Delay1 = 0.f;          //Channel 2
        inCh2Delay2 = 0.f;
        outCh2Delay1 = 0.f;
        outCh2Delay2 = 0.f;
    }
};
