#pragma once

#include "smoother.h"
#include "onepolefilters.h"

#include <cmath>
#include <array>

class Echo
{
public:
    Echo(int _sRate,                    //Constructor with default values
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

    float applyEcho(float currSample, unsigned int chInd)        //Signal Processing
    {
        //apply Smoothers
        dry = drySmoother.smooth();
        wet = wetSmoother.smooth();
        fbLocal = fbLocalSmoother.smooth();
        fbCross = fbCrossSmoother.smooth();

        float output = 0.f;

        if (chInd == 0)          //Channel L
        {
            output = currSample + (channelDelayL * fbLocal) + (channelDelayR * fbCross);

            //smooth delay time
            lp2hz(chInd);

            //apply delay
            output = delay(output, chInd);

            //apply 1-pole lowpass
            output = lowpass.applyFilter(output, chInd);

            //apply 1-pole highpass
            channelDelayL = highpass.applyFilter(output, chInd);

        }
        else if (chInd == 1)     //Channel R
        {
            output = currSample + (channelDelayR * fbLocal) + (channelDelayL * fbCross);

            //smooth delay time
            lp2hz(chInd);

            //apply delay
            output = delay(output, chInd);

            //apply 1-pole lowpass
            output = lowpass.applyFilter(output, chInd);

            //apply 1-pole highpass
            channelDelayR = highpass.applyFilter(output, chInd);
        }

        // apply effect amount
        output = xFade(currSample, output);

        return output;
    }
    
    void setMix(float _mix)          //set MIX amount
    {
        float mix_square = _mix * _mix;
        wet = (mix_square + mix_square) - mix_square * mix_square;
        dry = (1.f - mix_square + 1.f - mix_square) - (1.f - mix_square) * (1.f - mix_square);      //dry = 1.f - 4.f * mix_square + mix_square * mix_square;

        drySmoother.initSmoother(dry);
        wetSmoother.initSmoother(wet);
    }

    void setHiCut(float _hiCut)             //set HICUT
    {
        lowpass.setCutFreq(_hiCut);
    }
    
    void setTime(float _time)               //set Delay Time for L/R-Channels depending on Stereo Amount
    {
        delayTime = _time;;
        setChannelDelays();
    }

    void setStereo(float _stereoAmnt)             //set Stereo Amount
    {
        stereoAmnt = _stereoAmnt;
        setChannelDelays();
    }

    inline void setChannelDelays()
    {
        delayTimeL = (1.f + stereoAmnt * 0.0101f) * delayTime;
        delayTimeR = (1.f - stereoAmnt * 0.0101f) * delayTime;
    }


    void setFeedback(float _feedback)             //set Feedback Amount
    {
        fbAmnt = _feedback;
        calcFB();
    }

    void setCrossFeedback(float _crossFeedback)   //set Cross Feedback Amount
    {
        xfbAmnt = _crossFeedback;
        calcFB();
    }

private:
    
    /*Echo Controls*/
    float sRate;
    float stereoAmnt;
    float delayTime;
    float fbAmnt;
    float xfbAmnt;

    /*Resulting Parameters*/
    float wet;
    float dry;
    float fbLocal;
    float fbCross;
    float delayTimeL;
    float delayTimeR;
    float delayTimeL_lp;
    float delayTimeR_lp;

    std::array<float,132072> sampleBufferL;
    std::array<float,132072> sampleBufferR;

    //Filter objects
    OnePoleFilters lowpass;
    OnePoleFilters highpass;

    // Smoother objects
    Smoother drySmoother;
    Smoother wetSmoother;
    Smoother fbLocalSmoother;
    Smoother fbCrossSmoother;

    float channelDelayL;
    float channelDelayR;

    // lp2Hz delays
    float inCh1Delay;
    float inCh2Delay;

    float delay(float sampleIN, unsigned int chInd)                                  //Delay component -- might also move!
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

            if (ind_tm1 < 0.f)  {ind_tm1 += sampleBufferL.size();}        //Wrap
            if (ind_t0  < 0.f)  {ind_t0  += sampleBufferL.size();}
            if (ind_tp1 < 0.f)  {ind_tp1 += sampleBufferL.size();}
            if (ind_tp2 < 0.f)  {ind_tp2 += sampleBufferL.size();}

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

            if (ind_tm1 < 0.f)  {ind_tm1 += sampleBufferR.size();}        //Wrap
            if (ind_t0  < 0.f)  {ind_t0  += sampleBufferR.size();}
            if (ind_tp1 < 0.f)  {ind_tp1 += sampleBufferR.size();}
            if (ind_tp2 < 0.f)  {ind_tp2 += sampleBufferR.size();}

            sampleOUT = interpolRT(delaySamplesR_fract, sampleBufferR[ind_tm1], sampleBufferR[ind_t0], sampleBufferR[ind_tp1], sampleBufferR[ind_tp2]);

            indxR += 1;
            if (indxR >= sampleBufferR.size())
                indxR = 0;
        }

        return sampleOUT;
    }

    void calcFB()                                           //Calculate feedback amounts and initialize Smoothers
    {
        fbLocal = fbAmnt * (1.f - xfbAmnt);
        fbCross = fbAmnt * xfbAmnt;

        fbLocalSmoother.initSmoother(fbLocal);
        fbCrossSmoother.initSmoother(fbCross);
    }

    void lp2hz(unsigned int chInd)     //noch mal mit dem Cabinet hp1  vergleichen!!!
    {
        float w0 = 2.f * (2.f * M_PI / sRate);                //Normalize 2Hz

        if(w0 > 1.9f)                                       //Clip
            w0 = 1.9f;


        if (chInd == 0)                              //process
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

    float interpolRT(float fract, float sample_tm1, float sample_t0, float sample_tp1, float sample_tp2)
    {
        float fract_square = fract * fract;
        float fract_cub = fract_square * fract;

        float a = 0.5f * (sample_tp1 - sample_tm1);
        float b = 0.5f * (sample_tp2 - sample_t0);
        float c = sample_t0 - sample_tp1;

        return sample_t0 + fract * a + fract_cub * (a + b + 2.f*c) - fract_square * (2.f*a + b + 3.f*c);
    }

    inline float xFade(float sampleIn1, float sampleIn2)            //xFade Function, input1 - raw input, input2 - processed input
    {
        return ((sampleIn1 * dry) + (sampleIn2 * wet));
    }
    
};
