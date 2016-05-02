/**
 * @file       tiltfilters.h
 * @date       2016-03-18
 * @brief      this class is an implementation of Biquad Tilt Filters (LP, HP, LS, HS) as applied in the cabinet effect
 * @author     Anton Schmied [date of file creation 2016-03-18]
*/

#pragma once
#include "tools.h"

/** Enum Class for filter types */
enum class TiltFiltertype
{
    lowpass,
    highpass,
    lowshelf,
    highshelf
};

class TiltFilters
{
public:
	/** TiltFilters Constructor with default values */
    TiltFilters(int _sRate,                          
                float _cutFreq = 22000.f,
                float _tilt = 0.f,
                float _slopeWidth = 2.f,
                float _resonance = 0.5f,
                TiltFiltertype _filtertype = TiltFiltertype::lowpass)
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
        setTilt(_tilt);
        setResonance(_resonance);
        setSlopeWidth(_slopeWidth);
        setFiltertype(_filtertype);
    }

    ~TiltFilters(){}

	/** @brief    sets cut frequency
     *  @param    cut frequency in Hz
    */
    void setCutFreq(float _cutFreq)
    {
        if (_cutFreq < (sRate / 24576.f))                   //Frequency clipping
        {
            _cutFreq = sRate / 24576.f;
        }

        else if (_cutFreq > (sRate / 2.125f))
        {
            _cutFreq = sRate / 2.125f;
        }

        float omega = _cutFreq * (2.f * M_PI / sRate);      //Freqnecy to omega (warp)

        omega_sin = Nl::sin(omega);                         //alternative to sin(omega) -> tools.h
        omega_cos = Nl::cos(omega);                         //alternative to cos(omega) -> tools.h

        setAlpha();
        calcCoeff();
    }

	/** @brief    sets tilt amount, somewhat similar to shelf amplification
     *  @param    tilt in dB
    */
    void setTilt(float _tilt)
    {
        tilt = pow(1.059f, _tilt);                          //alterative to pow(10, (tilt/ 40.f))
        beta = 2.f * sqrt(tilt);

        setAlpha();
        calcCoeff();
    }

	/** @brief    sets resonance
     *  @param    resonance
    */
    void setResonance(float _resonance)
    {
        resonance = _resonance;

        if (resonance > 0.999f)                             //Resonance clipping
        {
            resonance = 0.999f;
        }

        setAlpha();
        calcCoeff();
    }

	/** @brief    sets slope width
     *  @param    slope width
    */
    void setSlopeWidth(float _slopeWidth)
    {
        slopeWidth = _slopeWidth < 1.f                      //min clip check
        ? 1.f
        : _slopeWidth;

        setAlpha();
        calcCoeff();
    }

	/** @brief    sets filter type
     *  @param    filter type <TiltFiltertype>
    */
    void setFiltertype(TiltFiltertype _filtertype)
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
    float applyFilter(float currSample, unsigned int channelIndex)
    {
        float output = 0.f;

        if(channelIndex == 0)
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

        else if (channelIndex == 1)
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

        case CtrlID::Tilt:
            ctrlVal = ((ctrlVal - 64.f) * 12.f) / 64.f;                     //Midi to [-12dB .. 12dB] linear
            setTilt(ctrlVal);
            break;

         case CtrlID::Resonance:
            ctrlVal = (ctrlVal - 64.f) / 32.f;                              //Midi to [-1 .. 1]
            setResonance(ctrlVal);
            break;

        case CtrlID::Slopewidth:
            ctrlVal = (ctrlVal / 127.f * 4.f) + 1.f;                        //Midi to [1 .. 5]
            setSlopeWidth(ctrlVal);
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
                    setFiltertype(TiltFiltertype::lowpass);
                    printf("lowpass on\n");
                    break;

                    case 1:
                    setFiltertype(TiltFiltertype::highpass);
                    printf("highpass on\n");
                    break;

                    case 2:
                    setFiltertype(TiltFiltertype::lowshelf);
                    printf("lowshelf on\n");
                    break;

                    case 3:
                    setFiltertype(TiltFiltertype::highshelf);
                    printf("highshelf on\n");
                    break;
                }
            }
            break;
        }
    }

private:

    float omega_cos;				/**< cosine of the warped frequency */
    float omega_sin;				/**< sine of the warped frequency */
    float alpha;					/**< a product of omega_sin and resonance */
    float beta;						/**< helper variable when tilt is set */
    float tilt;						/**< normalized tilt amount */
    float resonance;				/**< filter resonance */
    float slopeWidth;				/**< slope width */
    float sRate;					/**< samplerate */

    float inCh1Delay1;         		/**< Channel 1 Input Delay 1 */
    float inCh1Delay2;				/**< Channel 1 Input Delay 2 */
    float outCh1Delay1;				/**< Channel 1 Output Delay 1 */
    float outCh1Delay2;				/**< Channel 1 Output Delay 2 */

    float inCh2Delay1;          	/**< Channel 2 Input Delay 1 */
    float inCh2Delay2;				/**< Channel 2 Input Delay 2 */
    float outCh2Delay1;				/**< Channel 2 Output Delay 1 */
    float outCh2Delay2;				/**< Channel 2 Output Delay 2 */

    float b0, b1, b2, a0, a1, a2;	/**< Filter Coefficients */

    TiltFiltertype filtertype;

    enum CtrlID: unsigned char      /**< enum class for control IDs KORG Nano Kontrol*/
    {
        Cutfreq    = 0x12,
        Tilt       = 0x06,
        Resonance  = 0x13,
        Slopewidth = 0x08,
        Filtertype = 0x1B
    };


    /** @brief    calculates the coefficients depending on the chosen filter type
    */
    void calcCoeff()
    {
        float coeff;
        switch(filtertype)
        {
            case TiltFiltertype::lowpass:

            a0 = 1.f + alpha;
            a1 = omega_cos * -2.f;
            a2 = 1.f - alpha;
            b0 = (1 - omega_cos) / 2.f;
            b1 = 1.f - omega_cos;
            b2 = b0;
            break;

            case TiltFiltertype::highpass:

            a0 = 1.f + alpha;
            a1 = omega_cos * -2.f;
            a2 = 1.f - alpha;
            b0 = (1.f + omega_cos) / 2.f;
            b1 = (1.f + omega_cos) * -1.f;
            b2 = b0;
            break;

            case TiltFiltertype::lowshelf:
            coeff = beta * alpha;

            a0 = (tilt + 1.f) + (omega_cos * (tilt - 1.f)) + coeff;
            a1 = ((tilt - 1.f) + (omega_cos * (tilt + 1.f))) * -2.f;
            a2 = (tilt + 1.f) + (omega_cos * (tilt - 1.f)) - coeff;
            b0 = ((tilt + 1.f) - (omega_cos * (tilt - 1.f)) + coeff) * tilt;
            b1 = ((tilt - 1.f) - (omega_cos * (tilt + 1.f))) * 2.f * tilt;
            b2 = ((tilt + 1.f) - (omega_cos * (tilt - 1.f)) - coeff) * tilt;
            break;

            case TiltFiltertype::highshelf:
            coeff = beta * alpha;

            a0 = (tilt + 1.f) - (omega_cos * (tilt - 1.f)) + coeff;
            a1 = ((tilt - 1.f) - (omega_cos * (tilt + 1.f))) * 2.f;
            a2 = (tilt + 1.f) - (omega_cos * (tilt - 1.f)) - coeff;
            b0 = ((tilt + 1.f) + (omega_cos * (tilt - 1.f)) + coeff) * tilt;
            b1 = ((tilt - 1.f) + (omega_cos * (tilt + 1.f))) * -2.f * tilt;
            b2 = ((tilt + 1.f) + (omega_cos * (tilt - 1.f)) - coeff) * tilt;
            break;
        }

        a1 /= (-1.f * a0);          //normalize
        a2 /= (-1.f * a0);
        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
    }

    /** @brief    alpha calculation
    */
    void setAlpha()
    {
        alpha = (tilt + (1.f / tilt)) * (slopeWidth - 1.f);
        alpha = sqrt(alpha + 2.f) * omega_sin * (1.f - resonance);
    }

    /** @brief    resetes the delays if the filter type is changed
    */
    void resetDelays()
    {
        inCh1Delay1 = 0.f;          //channel 1
        inCh1Delay2 = 0.f;
        outCh1Delay1 = 0.f;
        outCh1Delay2 = 0.f;

        inCh2Delay1 = 0.f;          //Channel 2
        inCh2Delay2 = 0.f;
        outCh2Delay1 = 0.f;
        outCh2Delay2 = 0.f;
    }
};
