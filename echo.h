/**
 * @file       echo.h
 * @date       2016-03-18
 * @brief      this class is an implementation of the echo effect as found in Reaktor
 * @author     Anton Schmied [date of file creation 2016-03-18]
*/

#pragma once

#include "smoother.h"
#include "onepolefilters.h"
#include <array>

class Echo
{
public:
	/** Echo Constructor with default values */
    Echo(int _sRate,
         float _time = 0.375f,
         float _stereo = 0.f,
         float _feedback = 0.5f,
         float _crossFeedback = 0.f,
         float _hiCut = 4700.f,
         float _mix = 0.f)
        :sRate(static_cast<float>(_sRate))
        ,fbAmnt(_feedback)
        ,xfbAmnt(_crossFeedback)
        ,lowpass(_sRate, _hiCut, 0.f, OnePoleFiltertype::lowpass)
        ,highpass(_sRate, 50.f, 0.f, OnePoleFiltertype::highpass)
        ,drySmoother(_sRate, 0.032f)
        ,wetSmoother(_sRate, 0.032f)
        ,fbLocalSmoother(_sRate, 0.032f)
        ,fbCrossSmoother(_sRate, 0.032f)
        ,channelDelayL(0.f)
        ,channelDelayR(0.f)
        ,inCh1Delay(0.f)
        ,inCh2Delay(0.f)
    {
        setTime(_time);
        setStereo(_stereo);
        calcFB();
        setMix(_mix);
    }
    
    ~Echo(){}

	/** @brief    processes the incoming sample depending on the channel
     *  @param    raw Sample
     *  @param    channel index, 0 - Left, 1 - Right
     *  @return   processed sample
    */
    float applyEcho(float currSample, unsigned int chInd)
    {
        dry = drySmoother.smooth();									//apply Smoothers
        wet = wetSmoother.smooth();
        fbLocal = fbLocalSmoother.smooth();
        fbCross = fbCrossSmoother.smooth();

        float output = 0.f;

        if (chInd == 0)			//Channel L
        {
            output = currSample + (channelDelayL * fbLocal) + (channelDelayR * fbCross);

            lp2hz(chInd);											//smooth delay time

            output = delay(output, chInd);							//apply delay

            output = lowpass.applyFilter(output, chInd);			//apply 1-pole lowpass

            channelDelayL = highpass.applyFilter(output, chInd);	//apply 1-pole highpass

        }
        else if (chInd == 1)     //Channel R
        {
            output = currSample + (channelDelayR * fbLocal) + (channelDelayL * fbCross);

            lp2hz(chInd);											//smooth delay time

            output = delay(output, chInd);							//apply delay

            output = lowpass.applyFilter(output, chInd);			//apply 1-pole lowpass

            channelDelayR = highpass.applyFilter(output, chInd); 	//apply 1-pole highpass
        }

        output = xFade(currSample, output);							// apply effect amount

        return output;
    }

    /** @brief    sets mix amount, calculate dry and wet amounts
     *  @param    mix amount [0 .. 1]
    */   
    void setMix(float _mix)
    {
        float mix_square = _mix * _mix;
        wet = (mix_square + mix_square) - mix_square * mix_square;
        dry = (1.f - mix_square + 1.f - mix_square) - (1.f - mix_square) * (1.f - mix_square);      //dry = 1.f - 4.f * mix_square + mix_square * mix_square;

        drySmoother.initSmoother(dry);
        wetSmoother.initSmoother(wet);
    }

    /** @brief    sets hiCut for the lowpass fiter
     *  @param    hiCut frequnecy in Hz
    */
    void setHiCut(float _hiCut)
    {
        lowpass.setCutFreq(_hiCut);
    }
 
    /** @brief    sets Delay Time
     *  @param    delay time in ms
    */ 
    void setTime(float _time)
    {
        delayTime = _time;;
        setChannelDelays();
    }

    /** @brief    sets Stereo Amount
     *  @param    stereo amount [-1 .. 1]
    */ 
    void setStereo(float _stereoAmnt)
    {
        stereoAmnt = _stereoAmnt;
        setChannelDelays();
    }

    /** @brief    sets Delay Time for L/R-Channels depending on Stereo Amount
    */ 
    inline void setChannelDelays()
    {
        delayTimeL = (1.f + stereoAmnt * 0.0101f) * delayTime;
        delayTimeR = (1.f - stereoAmnt * 0.0101f) * delayTime;
    }

	/** @brief    sets Feedback Amount
     *  @param    feedback [0 .. 1]
    */ 
    void setFeedback(float _feedback)   
    {
        fbAmnt = _feedback;
        calcFB();
    }

	/** @brief    sets Cross Feedback Amount
     *  @param    cross feedback [0 .. 1]
    */ 
    void setCrossFeedback(float _crossFeedback)
    {
        xfbAmnt = _crossFeedback;
        calcFB();
    }

    void setEchoParams(float ctrlVal, unsigned char ctrlTag)
    {
        switch (ctrlTag)
        {
        case CtrlID::Hicut:
            ctrlVal = (ctrlVal * 80.f) / 127.f + 60.f;          // Pitch Values for better testing
            printf("HiCut: %f\n", ctrlVal);

            ctrlVal = pow(2.f, (ctrlVal - 69.f) / 12) * 440.f;

            setHiCut(ctrlVal);
            break;

        case CtrlID::Mix:
            ctrlVal = ctrlVal / 127.f;
            printf("Mix: %f\n", ctrlVal);
            setMix(ctrlVal);
            break;

        case CtrlID::Dtime:
            ctrlVal = ctrlVal / 127.f;
            ctrlVal = ctrlVal * ctrlVal * 1.5f;
            printf("Time: %f\n", ctrlVal * 1000.f);
            setTime(ctrlVal);
            break;

        case CtrlID::Stereo:
            ctrlVal = (ctrlVal - 63.5f) * (33.f / 63.5f);
            printf("Stereo: %f\n", ctrlVal);
            setStereo(ctrlVal);
            break;

        case CtrlID::Feedback:
            ctrlVal = ctrlVal / 127.f;
            printf("Feedback: %f\n", ctrlVal);
            setFeedback(ctrlVal);
            break;

        case CtrlID::Xfeedback:
            ctrlVal = ctrlVal / 127.f;
            printf("Feedback: %f\n", ctrlVal);
            setCrossFeedback(ctrlVal);
        }
    }

private:
    
    float sRate;					/**< samplerate */
    float stereoAmnt;				/**< stereo amount */
    float delayTime;				/**< delay time */
    float fbAmnt;					/**< feedback amount - external*/
    float xfbAmnt;					/**< cross feedback amount - external*/

    float wet;						/**< wet amount */
    float dry;						/**< dry amount */
    float fbLocal;					/**< local feedback amount - internal*/
    float fbCross;					/**< cross feedback amount - internal*/
    float delayTimeL;				/**< left channel delay time */
    float delayTimeR;				/**< right channel delay time */
    float delayTimeL_lp;			/**< left channel delay time processed through 2Hz-lowpass */
    float delayTimeR_lp;			/**< right channel delay time processed through 2Hz-lowpass */

    std::array<float,131072> sampleBufferL;		/**< sample buffer for the left channel */
    std::array<float,131072> sampleBufferR;		/**< sample buffer for the right channel */

    OnePoleFilters lowpass;			/**< lowpass filter*/
    OnePoleFilters highpass;		/**< highpass filter */

    Smoother drySmoother;			/**< dry amount smoother */	
    Smoother wetSmoother;			/**< wet smount smoother */
    Smoother fbLocalSmoother;		/**< local feedback smoother */
    Smoother fbCrossSmoother;		/**< cross feedback smoother */	

    float channelDelayL;			/**<  delayed sample processed through the 1-Pole highpass - left channel*/
    float channelDelayR;			/**<  delayed sample processed through the 1-Pole highpass - right channel*/	

    float inCh1Delay;				/**< internal 2Hz lowpass Channel 1 Input Delay */
    float inCh2Delay;				/**< internal 2Hz lowpass Channel 2 Input Delay */

    enum CtrlID: unsigned char  /**< Enum class for control IDs Korg Nano Kontrol I*/
    {
        Hicut     = 0x12,
        Mix       = 0x04,
        Dtime     = 0x05,
        Stereo    = 0x06,
        Feedback  = 0x08,
        Xfeedback = 0x09
    };

    /** @brief    main delay function, writes to delay buffer, reads from delay buffer, 
	 *  		  interpolates 4 neighbouring values, all dependant on the channel index
     *  @param    raw sample
	 *  @param 	  channel index
	 *  @return	  processed sample
    */
    float delay(float sampleIN, unsigned int chInd)                              
    {
        float sampleOUT = 0.f;

        static unsigned int indxL = 0;
        static unsigned int indxR = 0;

        if (chInd == 0)
        {
            float delaySamplesL = delayTimeL_lp * sRate;

            float delaySamplesL_int = round(delaySamplesL - 0.5f);
            float delaySamplesL_fract = delaySamplesL - delaySamplesL_int;

            sampleBufferL[indxL] = sampleIN;                              //Write

            int ind_tm1 = delaySamplesL_int - 1;
            if (ind_tm1 < 0)
            {
                ind_tm1 = 0;
            }
            int ind_t0  = delaySamplesL_int;
            int ind_tp1 = delaySamplesL_int + 1;
            int ind_tp2 = delaySamplesL_int + 2;

            ind_tm1 = indxL - ind_tm1;
            ind_t0  = indxL - ind_t0;
            ind_tp1 = indxL - ind_tp1;
            ind_tp2 = indxL - ind_tp2;

            ind_tm1 &= (sampleBufferL.size() - 1);                         //Wrap with a mask sampleBuffer.size()-1
            ind_t0  &= (sampleBufferL.size() - 1);
            ind_tp1 &= (sampleBufferL.size() - 1);
            ind_tp2 &= (sampleBufferL.size() - 1);
#if 0
            if (ind_tm1 < 0.f)  {ind_tm1 += sampleBufferL.size();}        //Wrap as found in Reaktor
            if (ind_t0  < 0.f)  {ind_t0  += sampleBufferL.size();}
            if (ind_tp1 < 0.f)  {ind_tp1 += sampleBufferL.size();}
            if (ind_tp2 < 0.f)  {ind_tp2 += sampleBufferL.size();}
#endif
            sampleOUT = interpolRT(delaySamplesL_fract, sampleBufferL[ind_tm1], sampleBufferL[ind_t0], sampleBufferL[ind_tp1], sampleBufferL[ind_tp2]);

            ++indxL;
            if (indxL >= sampleBufferL.size())
                indxL = 0;

        }
        else if (chInd == 1)
        {
            float delaySamplesR = delayTimeR_lp * sRate;

            float delaySamplesR_int = round(delaySamplesR - 0.5f);
            float delaySamplesR_fract = delaySamplesR - delaySamplesR_int;

            sampleBufferR[indxR] = sampleIN;                              //Write

            int ind_tm1 = delaySamplesR_int - 1;
            if (ind_tm1 < 0)
            {
                ind_tm1 = 0;
            }
            int ind_t0  = delaySamplesR_int;
            int ind_tp1 = delaySamplesR_int + 1;
            int ind_tp2 = delaySamplesR_int + 2;

            ind_tm1 = indxR - ind_tm1;
            ind_t0  = indxR - ind_t0;
            ind_tp1 = indxR - ind_tp1;
            ind_tp2 = indxR - ind_tp2;

            ind_tm1 &= (sampleBufferR.size() - 1);                          //Wrap with a mask sampleBuffer.size()-1
            ind_t0  &= (sampleBufferR.size() - 1);
            ind_tp1 &= (sampleBufferR.size() - 1);
            ind_tp2 &= (sampleBufferR.size() - 1);
#if 0
            if (ind_tm1 < 0.f)  {ind_tm1 += sampleBufferR.size();}        //Wrap as found in Reaktor
            if (ind_t0  < 0.f)  {ind_t0  += sampleBufferR.size();}
            if (ind_tp1 < 0.f)  {ind_tp1 += sampleBufferR.size();}
            if (ind_tp2 < 0.f)  {ind_tp2 += sampleBufferR.size();}
#endif
            sampleOUT = interpolRT(delaySamplesR_fract, sampleBufferR[ind_tm1], sampleBufferR[ind_t0], sampleBufferR[ind_tp1], sampleBufferR[ind_tp2]);

            indxR += 1;
            if (indxR >= sampleBufferR.size())
                indxR = 0;
        }

        return sampleOUT;
    }

    /** @brief    calculates feedback amount and initializes smoothers
    */
    void calcFB()
    {
        fbLocal = fbAmnt * (1.f - xfbAmnt);
        fbCross = fbAmnt * xfbAmnt;

        fbLocalSmoother.initSmoother(fbLocal);
        fbCrossSmoother.initSmoother(fbCross);
    }

	/** @brief    internal first order 2Hz lowpass to smooth delay times when changed
     *  @param    channel index
    */
    void lp2hz(unsigned int chInd)
    {
        float w0 = 2.f * (2.f * M_PI / sRate);              //normalize 2Hz

        if(w0 > 1.9f)                                       //Clip
            w0 = 1.9f;


        if (chInd == 0)                              		//process
        {
            delayTimeL_lp = delayTimeL - inCh1Delay;
            delayTimeL_lp = delayTimeL_lp * w0 + inCh1Delay;

            inCh1Delay = delayTimeL_lp;
        }

        else if (chInd == 1)
        {
            delayTimeR_lp = delayTimeR - inCh2Delay;
            delayTimeR_lp = delayTimeR_lp * w0 + inCh2Delay;

            inCh2Delay = delayTimeR_lp;
        }
    }

	/** @brief    interpolation of 4 neighbouring samples from the delay buffers
     *  @param    fractional part of the number of the delay samples
	 *  @param	  sample from delay buffer with index n-1
	 *  @param	  sample from delay buffer with index n
	 *  @param	  sample from delay buffer with index n+1
	 *  @param	  sample from delay buffer with index n+2
	 *  @return	  interpolated value of the four samples
    */
    float interpolRT(float fract, float sample_tm1, float sample_t0, float sample_tp1, float sample_tp2)
    {
        float fract_square = fract * fract;
        float fract_cub = fract_square * fract;

        float a = 0.5f * (sample_tp1 - sample_tm1);
        float b = 0.5f * (sample_tp2 - sample_t0);
        float c = sample_t0 - sample_tp1;

        return sample_t0 + fract * a + fract_cub * (a + b + 2.f*c) - fract_square * (2.f*a + b + 3.f*c);
    }

    /** @brief    calculation of dry and wet amounts depending on mix amount
     *  @param    raw Sample
     *  @param    processed sample
     *  @return   a mix of both samples
    */
	inline float xFade(float sampleIn1, float sampleIn2)
    {
        return ((sampleIn1 * dry) + (sampleIn2 * wet));
    }
    
};
