/******************************************************************************/
/** @file		cabinet.cpp
    @date		2017-05-11
    @version	1.0
    @author		Anton Schmied[2016-03-18]
    @brief		Cabinet Class member and method definitions
*******************************************************************************/

#include "cabinet.h"

/******************************************************************************/
/** Cabinet Default Constructor
 * @brief    initialization of the modules local variabels with default values
 *           SampleRate:            48 kHz
 *           Drive:                 20 dB
 *           Tilt:                  -12 dB
 *           HiCut Frequency:       4700 Hz     Lowpass Filters
 *           LoCut Frequency:       61 Hz       Highpass Filters
 *           Fold:                  0.25
 *           Asym:                  0.1
 *           Cabinet Level:         -14 dB
 *           Mix:                   0.f
*******************************************************************************/

Cabinet::Cabinet()
{
    mDrive = NlToolbox::Conversion::db2af(20.f);
    mWet = 0.f;
    mDry = 1.f;
    mFold = 0.25;
    mAsym = 0.1f;
    mCabLvl = NlToolbox::Conversion::db2af(-14.f);

    mSaturation = NlToolbox::Conversion::db2af(0.5f * -12.f);
    mSaturationConst = (0.1588f / mSaturation);

    pHighpass = new BiquadFilters(61.f, 0.f, 0.5, BiquadFilterType::HIGHPASS);
    pLowpass_1 = new BiquadFilters(4700.f, 0.f, 0.5, BiquadFilterType::LOWPASS);
    pLowpass_2 = new BiquadFilters(4700.f * 1.333f, 0.f, 0.5, BiquadFilterType::LOWPASS);
    pLowshelf_1 = new TiltFilters(1200.f, -12.f, 2.f, 0.5f, TiltFilterType::LOWSHELF);
    pLowshelf_2 = new TiltFilters(1200.f, -12.f * (-1.f), 2.f, 0.5f, TiltFilterType::LOWSHELF);
    pHighpass30Hz = new NlToolbox::Filters::Highpass30Hz(SAMPLERATE);

    mSmootherMask = 0x0000;
    mDry_ramp = 1.0f;
    mWet_ramp = 1.0f;
    mDrive_ramp = 1.0f;
}

/******************************************************************************/
/** Cabinet Parameterized Constructor
 * @brief    initialization of the modules local variabels with custom
 *           parameters
*******************************************************************************/

Cabinet::Cabinet(float _drive,
                 float _tilt,
                 float _hiCut,
                 float _loCut,
                 float _fold,
                 float _asym,
                 float _cabLvl,
                 float _mix)
{
    mDrive = NlToolbox::Conversion::db2af(_drive);
    mWet = NlToolbox::Conversion::db2af(_cabLvl) * _mix;
    mDry = 1.f - _mix;
    mFold = _fold;
    mAsym = _asym;

    mSaturation = NlToolbox::Conversion::db2af(0.5f * _tilt);
    mSaturationConst = (0.1588f / mSaturation);

    pHighpass = new BiquadFilters(_loCut, 0.f, 0.5, BiquadFilterType::HIGHPASS);
    pLowpass_1 = new BiquadFilters(_hiCut, 0.f, 0.5, BiquadFilterType::LOWPASS);
    pLowpass_2 = new BiquadFilters(_hiCut * 1.333f, 0.f, 0.5, BiquadFilterType::LOWPASS);
    pLowshelf_1 = new TiltFilters(1200.f, _tilt, 2.f, 0.5f, TiltFilterType::LOWSHELF);
    pLowshelf_2 = new TiltFilters(1200.f, _tilt * (-1.f), 2.f, 0.5f, TiltFilterType::LOWSHELF);
    pHighpass30Hz = new NlToolbox::Filters::Highpass30Hz(SAMPLERATE);

    mSmootherMask = 0x0000;
    mDry_ramp = 1.0f;
    mWet_ramp = 1.0f;
    mDrive_ramp = 1.0f;
}



/******************************************************************************/
/** Cabinet Destructor
 * @brief   Destroys the created Filter Objects
*******************************************************************************/

Cabinet::~Cabinet()
{
    delete pHighpass;
    delete pLowpass_1;
    delete pLowpass_2;
    delete pLowshelf_1;
    delete pLowshelf_2;
    delete pHighpass30Hz;
}


/*****************************************************************************/
/** @brief    interface method which converts and scales the incoming midi
 *            values and passes these to the respective methods
 *  @param    midi control value [0 ... 127]
 *  @param    midi control ID -> enum CtrlID (cabinet.h)
******************************************************************************/

void Cabinet::setCabinetParams(unsigned char _ctrlId, float _ctrlVal)
{
    switch(_ctrlId)
    {
        case CtrlId::HICUT:
            _ctrlVal = (_ctrlVal * 80.f) / 127.f + 60.f;            //Midi to Pitch [60 .. 140]
            printf("Cabinet - HiCut: %f\n", _ctrlVal);

            _ctrlVal = pow(2.f, (_ctrlVal - 69.f) / 12) * 440.f;    //Pitch to Freq [261Hz .. 26580Hz]

            pLowpass_1->setCutFreq(_ctrlVal);
            pLowpass_2->setCutFreq(_ctrlVal * 1.333f);
            break;

        case CtrlId::LOCUT:
            _ctrlVal = (_ctrlVal * 80.f) / 127.f + 20.f;            //Midi to Pitch [20 ..100]
            printf("Cabinet - LoCut: %f\n", _ctrlVal);

            _ctrlVal = pow(2.f, (_ctrlVal - 69.f) / 12) * 440.f;    //Pitch to Freq [26Hz .. 2637Hz]

            pHighpass->setCutFreq(_ctrlVal);
            break;

        case CtrlId::MIX:
            _ctrlVal = _ctrlVal / 127.f;                            //Midi to [0 .. 1]
            printf("Cabinet - Mix: %f\n", _ctrlVal);

            // Initialize Smoother ID 1: Dry
            mDry_target = 1.f - _ctrlVal;
            mDry_base = mDry;
            mDry_diff = mDry_target - mDry_base;

            mSmootherMask |= 0x0001;
            mDry_ramp = 0.f;

            // Initialize Smoother ID 2: Wet
            mWet_target = _ctrlVal * mCabLvl;
            mWet_base = mWet;
            mWet_diff = mWet_target - mWet_base;

            mSmootherMask |= 0x0002;
            mWet_ramp = 0.f;
            break;

        case CtrlId::CABLEVEL:
            _ctrlVal = (_ctrlVal - 127.f) * (50.f / 127.f);         //Midi to [-50db .. 0dB]
            printf("Cabinet - Cab Lvl: %f\n", _ctrlVal);

            mCabLvl = NlToolbox::Conversion::db2af(_ctrlVal);

            // Initialize Smoother ID 2: Wet
            mWet_target = (1.f - mDry_target) * mCabLvl;

            mWet_base = mWet;
            mWet_diff = mWet_target - mWet_base;

            mSmootherMask |= 0x0002;
            mWet_ramp = 0.0;
            break;

        case CtrlId::DRIVE:
            _ctrlVal = _ctrlVal * (50.f / 127.f);                     //Midi to [0dB .. 50dB]
            printf("Cabinet - Drive: %f\n", _ctrlVal);

            // Initialize Smoother ID 3: Drive
            mDrive_target = NlToolbox::Conversion::db2af(_ctrlVal);

            mDrive_base = mDrive;
            mDrive_diff = mDrive_target - mDrive_base;

            mSmootherMask |= 0x0004;
            mDrive_ramp = 0.0;
            break;

        case CtrlId::TILT:
            _ctrlVal = (_ctrlVal - 63.5f) * (50.f / 63.5f);           //Midi to [-50dB .. 50dB]
            printf("Cabinet - Tilt: %f\n", _ctrlVal);

            pLowshelf_1->setTilt(_ctrlVal);
            pLowshelf_2->setTilt(_ctrlVal * (-1.f));

            mSaturation = NlToolbox::Conversion::db2af(0.5f * _ctrlVal);
            mSaturationConst = (0.1588f / mSaturation);
            break;

        case CtrlId::FOLD:
            _ctrlVal = _ctrlVal / 127.f;                              //Midi to [0 .. 1]
            printf("Cabinet - Fold: %f\n", _ctrlVal);

            mFold = _ctrlVal;
            break;

        case CtrlId::ASYM:
            _ctrlVal = _ctrlVal / 127.f;                              //Midi to [0 .. 1]
            printf("Cabinet - Asym: %f\n", _ctrlVal);

            mAsym = _ctrlVal;
            break;
    }
}



/*****************************************************************************/
/** @brief    applies the cabinet effect to the incoming sample
 *  @param    raw Sample
******************************************************************************/

void Cabinet::applyCab(float _rawSample)
{
    //****************************** Smoothing ******************************//
    if (mSmootherMask)
    {
        applyCabSmoother();
    }


    //******************************** Drive ********************************//
    float processedSample = _rawSample * mDrive;


    //*************************** Biquad Highpass ***************************//
    processedSample = pHighpass->applyFilter(processedSample);


    //************************** 1st Tilt Lowshelf **************************//
    processedSample = pLowshelf_1->applyFilter(processedSample);


    //******************************* Shaper ********************************//
    processedSample *= mSaturationConst;
    float ctrlSample = processedSample;

    processedSample = NlToolbox::Math::sinP3(processedSample);
    processedSample = NlToolbox::Others::threeRanges(processedSample, ctrlSample, mFold);

    float processedSample_square = pHighpass30Hz->applyFilter(processedSample * processedSample);

    processedSample = NlToolbox::Others::parAsym(processedSample, processedSample_square, mAsym);
    processedSample *= mSaturation;


    //************************* 2nd Tilt Lowshelf ***************************//
    processedSample = pLowshelf_2->applyFilter(processedSample);


    //************************* 2 Biquad Lowpass ****************************//
    processedSample = pLowpass_1->applyFilter(processedSample);
    processedSample = pLowpass_2->applyFilter(processedSample);


    //**************************** Crossfade ********************************//
    mCabinetOut = NlToolbox::Crossfades::crossFade(_rawSample, processedSample, mDry, mWet);
}



/*****************************************************************************/
/** @brief    applies the smoothers of the cabinet module, depending if the
 *            corresponding bit of the mask is set to 1
******************************************************************************/

inline void Cabinet::applyCabSmoother()
{
    //************************ ID 1: Dry Smoother ***************************//
    if (mDry_ramp < 1.0)
    {
        mDry_ramp += SMOOTHER_INC;

        if (mDry_ramp > 1.0)
        {
            mDry = mDry_target;
            mSmootherMask &= 0xFFFE;       // switch first bit to 0
        }
        else
        {
            mDry = mDry_base + mDry_diff * mDry_ramp;
        }
    }


    //************************ ID 2: Wet Smoother ***************************//
    if (mWet_ramp < 1.0)
    {
        mWet_ramp += SMOOTHER_INC;

        if (mWet_ramp > 1.0)
        {
            mWet = mWet_target;
            mSmootherMask &= 0xFFFD;       // switch second bit to 0
        }
        else
        {
            mWet = mWet_base + mWet_diff * mWet_ramp;
        }
    }


    //*********************** ID 3: Drive Smoother **************************//
    if (mDrive_ramp < 1.0)
    {
        mDrive_ramp += SMOOTHER_INC;

        if (mDrive_ramp > 1.0)
        {
            mDrive = mDrive_target;
            mSmootherMask &= 0xFFFB;       // switch third bit to 0
        }
        else
        {
            mDrive = mDrive_base + mDrive_diff * mDrive_ramp;
        }
    }
}
