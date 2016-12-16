/******************************************************************************/
/** @file		outputmixer.cpp
    @date		2016-10-20
    @version	0.2
    @author		Anton Schmied[2016-10-20]
    @brief		Outputmixer Class member and method definitions
*******************************************************************************/

#include "outputmixer.h"

/******************************************************************************/
/** Outputmixer Default Constructor
 * @brief    initialization of the modules local variabels with default values
*******************************************************************************/

Outputmixer::Outputmixer()
    : mLeftHighpass(48000, NlToolbox::Conversion::pitch2freq(8.f), 0.f, OnePoleFilterType::HIGHPASS)
    , mRightHighpass(48000, NlToolbox::Conversion::pitch2freq(8.f), 0.f, OnePoleFilterType::HIGHPASS)
#ifdef SMOOTHEROBJ
    //-------------------------- Smoother
    , mALevelSmoother(48000.f, 0.032f)
    , mAPanSmoother(48000.f, 0.032f)
    , mBLevelSmoother(48000.f, 0.032f)
    , mBPanSmoother(48000.f, 0.032f)
    , mCombLevelSmoother(48000.f, 0.032f)
    , mCombPanSmoother(48000.f, 0.032f)
    , mSVFilterLevelSmoother(48000.f, 0.032f)
    , mSVFilterPanSmoother(48000.f, 0.032f)
    , mDriveSmoother(48000.f, 0.032f)
    , mFoldSmoother(48000.f, 0.032f)
    , mAsymSmoother(48000.f, 0.032f)
    , mMainLevelSmoother(48000.f, 0.032f)
    , mKeypanSmoother(48000.f, 0.032f)
#else
    , mOMSmootherMask(0x0000)
    , mInc(5.f / (static_cast<float>(48000.f) * 0.032))
#endif
{
    setALevel(0.f);
    setAPan(0.f);
    setBLevel(0.f);
    setBPan(0.f);
    setCombLevel(0.f);
    setCombPan(0.f);
    setSVFilterLevel(0.f);
    setSVFilterPan(0.f);

    setDrive(0.5f);
    setFold(0.5f);
    setAsym(0.5f);

    setMainLevel(0.f);
    setKeyPan(0.f);
}


/******************************************************************************/
/** @brief    calls the smoothing functions fpr each sample
*******************************************************************************/

inline void Outputmixer::applySmoothers()
{
#ifdef SMOOTHEROBJ
    mALevel = mALevelSmoother.smooth();
    mAPan = mAPanSmoother.smooth();
    mBLevel = mBLevelSmoother.smooth();
    mBPan = mBPanSmoother.smooth();
    mCombLevel = mCombLevelSmoother.smooth();
    mCombPan = mCombPanSmoother.smooth();
    mSVFilterLevel = mSVFilterLevelSmoother.smooth();
    mSVFilterPan = mSVFilterPanSmoother.smooth();

    mMainLevel = mMainLevelSmoother.smooth();

    mKeypan = mKeypanSmoother.smooth();
#else
    // 0: A Level Smoother
    if (mALevel_ramp < 1.0)
    {
        mALevel_ramp += mInc;

        if (mALevel_ramp > 1.0)
        {
            mALevel = mALevel_target;
            mOMSmootherMask &= 0xFFFE;      // switch first bit to 0
        }
        else
        {
            mALevel = mALevel_base + mALevel_diff * mALevel_ramp;
        }
    }

    // 1: A Pan Smoother
    if (mAPan_ramp < 1.0)
    {
        mAPan_ramp += mInc;

        if (mAPan_ramp > 1.0)
        {
            mAPan = mAPan_target;
            mOMSmootherMask &= 0xFFFD;      // switch second bit to 0
        }
        else
        {
            mAPan = mAPan_base + mAPan_diff * mAPan_ramp;
        }
    }

    // 2: B Level Smoother
    if (mBLevel_ramp < 1.0)
    {
        mBLevel_ramp += mInc;

        if (mBLevel_ramp > 1.0)
        {
            mBLevel = mBLevel_target;
            mOMSmootherMask &= 0xFFFB;      // switch third bit to 0
        }
        else
        {
            mBLevel = mBLevel_base + mBLevel_diff * mBLevel_ramp;
        }
    }

    // 3: B Pan Smoother
    if (mBPan_ramp < 1.0)
    {
        mBPan_ramp += mInc;

        if (mBPan_ramp > 1.0)
        {
            mBPan = mBPan_target;
            mOMSmootherMask &= 0xFFF7;      // switch fourth bit to 0
        }
        else
        {
            mBPan = mBPan_base + mBPan_diff * mBPan_ramp;
        }
    }

    // 4: Comb Level Smoother
    if (mCombLevel_ramp < 1.0)
    {
        mCombLevel_ramp += mInc;

        if (mCombLevel_ramp > 1.0)
        {
            mCombLevel = mCombLevel_target;
            mOMSmootherMask &= 0xFFEF;      // switch fifth bit to 0
        }
        else
        {
            mCombLevel = mCombLevel_base + mCombLevel_diff * mCombLevel_ramp;
        }
    }

    // 5: Comb Pan Smoother
    if (mCombPan_ramp < 1.0)
    {
        mCombPan_ramp += mInc;

        if (mCombPan_ramp > 1.0)
        {
            mCombPan = mCombPan_target;
            mOMSmootherMask &= 0xFFDF;      // switch sixth bit to 0
        }
        else
        {
            mCombPan = mCombPan_base + mCombPan_diff * mCombPan_ramp;
        }
    }

    // 6: SV Filter Level Smoother
    if (mSVFilterLevel_ramp < 1.0)
    {
        mSVFilterLevel_ramp += mInc;

        if (mSVFilterLevel_ramp > 1.0)
        {
            mSVFilterLevel = mSVFilterLevel_target;
            mOMSmootherMask &= 0xFFBF;      // switch seventh bit to 0
        }
        else
        {
            mSVFilterLevel = mSVFilterLevel_base + mSVFilterLevel_diff * mSVFilterLevel_ramp;
        }
    }

    // 7: SV Filter Pan Smoother
    if (mSVFilterPan_ramp < 1.0)
    {
        mSVFilterPan_ramp += mInc;

        if (mSVFilterPan_ramp > 1.0)
        {
            mSVFilterPan = mSVFilterPan_target;
            mOMSmootherMask &= 0xFF7F;      // switch eighth bit to 0
        }
        else
        {
            mSVFilterPan = mSVFilterPan_base + mSVFilterPan_diff * mSVFilterPan_ramp;
        }
    }

    // 8: Drive Smoother
    if (mDrive_ramp < 1.0)
    {
        mDrive_ramp += mInc;

        if (mDrive_ramp > 1.0)
        {
            mDrive = mDrive_target;
            mOMSmootherMask &= 0xFEFF;      // switch ninth bit to 0
        }
        else
        {
            mDrive = mDrive_base + mDrive_diff * mDrive_ramp;
        }
    }

    // 9: Fold Smoother
    if (mFold_ramp < 1.0)
    {
        mFold_ramp += mInc;

        if (mFold_ramp > 1.0)
        {
            mFold = mFold_target;
            mOMSmootherMask &= 0xFDFF;      // switch tenth bit to 0
        }
        else
        {
            mFold = mFold_base + mFold_diff * mFold_ramp;
        }
    }

    // 10: Asym Smoother
    if (mAsym_ramp < 1.0)
    {
        mAsym_ramp += mInc;

        if (mAsym_ramp > 1.0)
        {
            mAsym = mAsym_target;
            mOMSmootherMask &= 0xFBFF;      // switch 11th bit to 0
        }
        else
        {
            mAsym = mAsym_base + mAsym_diff * mAsym_ramp;
        }
    }

    // 11: Main Level Smoother
    if (mMainLevel_ramp < 1.0)
    {
        mMainLevel_ramp += mInc;

        if (mMainLevel_ramp > 1.0)
        {
            mMainLevel = mMainLevel_target;
            mOMSmootherMask &= 0xF7FF;      // switch 12th bit to 0
        }
        else
        {
            mMainLevel = mMainLevel_base + mMainLevel_diff * mMainLevel_ramp;
        }
    }

    // 13: Key Pan Smoother
    if (mKeypan_ramp < 1.0)
    {
        mKeypan_ramp += mInc;

        if (mKeypan_ramp > 1.0)
        {
            mKeypan = mKeypan_target;
            mOMSmootherMask &= 0xFFF;       // switch 13th bit to 0
        }
        else
        {
            mKeypan = mKeypan_base + mKeypan_diff * mKeypan_ramp;
        }
    }
#endif
}

/******************************************************************************/
/** @brief    main function which calculates the mix of the A and B samples
 *            depending ...
*******************************************************************************/

void Outputmixer::applyMixer(float _sampleA, float _sampleB, float _sampleComb, float _sampleSVFilter)
{
    static uint32_t voiceNumber = 0;

    if (voiceNumber == 0)
    {
        mSample_L = 0.f;
        mSample_R = 0.f;

        calcKeyPan();

#ifndef SMOOTHEROBJ
        if(mOMSmootherMask)
        {
            applySmoothers();
        }
#endif
    }

    // panning
    /// soll das wirklich jedes mal berechnet werden ... :/
    float pan_AR = (mPitchPanArray[voiceNumber] + mAPan) * mALevel;
    float pan_AL = (1.f - (mPitchPanArray[voiceNumber] + mAPan)) * mALevel;

    float pan_BR = (mPitchPanArray[voiceNumber] + mBPan) * mBLevel;
    float pan_BL = (1.f - (mPitchPanArray[voiceNumber] + mBPan)) * mBLevel;

    float main_R = (pan_AR * _sampleA) + (pan_BR * _sampleB);
    float main_L = (pan_AL * _sampleA) + (pan_BL * _sampleB);

    /*
    float pan_CombR = (mPitchPanArray[voiceNumber] + mCombPan) * mCombLevel;
    float pan_CombL = (1.f - (mPitchPanArray[voiceNumber] + mCombPan)) * mCombLevel;

    float pan_SVFilterR = (mPitchPanArray[voiceNumber] + mSVFilterPan) * mSVFilterLevel;
    float pan_SVFilterL = (1.f - (mPitchPanArray[voiceNumber] + mSVFilterPan)) * mSVFilterLevel;

    main_L = (pan_AL * _sampleA) + (pan_BL * _sampleB) + (pan_CombL * _sampleComb) + (pan_SVFilterL * _sampleSVFilter);
    main_R = (pan_AR * _sampleA) + (pan_BR * _sampleB) + (pan_CombR * _sampleComb) + (pan_SVFilterR * _sampleSVFilter);
    */

    float ctrlSample;
    float squareSample;

    // Shape Left Sample
    main_L = mDrive * main_L;
    ctrlSample = main_L;

    main_L = NlToolbox::Math::sinP3(main_L);
    main_L = NlToolbox::Others::threeRanges(main_L, ctrlSample, mFold);

    squareSample = main_L * main_L;
    main_L = NlToolbox::Others::parAsym(main_L, squareSample, mAsym);

    // Shape Right Sample
    main_R = mDrive * main_R;
    ctrlSample = main_R;

    main_R = NlToolbox::Math::sinP3(main_R);
    main_R = NlToolbox::Others::threeRanges(main_R, ctrlSample, mFold);

    squareSample = main_R * main_R;
    main_R = NlToolbox::Others::parAsym(main_R, squareSample, mAsym);

    // Combine the voices
    /// die velocity hier ist temporär, da wir noch keine envelopes haben ...
    mSample_L += (main_L * gVoiceVelocity[voiceNumber]);
    mSample_R += (main_R * gVoiceVelocity[voiceNumber]);

    voiceNumber++;

    if (voiceNumber == NUM_VOICES)
    {
        /// 1-Pole-HP, Frage ob dieser so richtig ist
        mSample_L = mLeftHighpass.applyFilter(mSample_L);
        mSample_R = mRightHighpass.applyFilter(mSample_R);

        // Level adjustment
        mSample_L *= mMainLevel;
        mSample_R *= mMainLevel;

        voiceNumber = 0;
    }
}



/*****************************************************************************/
/** @brief    interface method which converts and scales the incoming midi
 *            values and passes these to the respective methods or parameters
 *  @param    midi control ID -> enum CtrlID (ouputmixer.h)
 *  @param    midi control value [0 ... 127]
******************************************************************************/

void Outputmixer::setOutputmixerParams(unsigned char _ctrlID, float _ctrlVal)
{
    switch (_ctrlID)
    {
        case CtrlID::A_LEVEL:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("Output Mixer: A Level: %f\n", _ctrlVal);
            setALevel(_ctrlVal);
            break;

        case CtrlID::A_PAN:
            _ctrlVal = (_ctrlVal / .63f) - 100.f;

            if (_ctrlVal > 100.f)
            {
                _ctrlVal = 100.f;
            }

            printf("Output Mixer: A Pan: %f\n", _ctrlVal);
            setAPan(_ctrlVal);
            break;

        case CtrlID::B_LEVEL:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("Output Mixer: B Level: %f\n", _ctrlVal);
            setBLevel(_ctrlVal);
            break;

        case CtrlID::B_PAN:
            _ctrlVal = (_ctrlVal / .63f) - 100.f;

            if (_ctrlVal > 100.f)
            {
                _ctrlVal = 100.f;
            }

            printf("Output Mixer: B Pan: %f\n", _ctrlVal);
            setBPan(_ctrlVal);
            break;

        case CtrlID::COMB_LEVEL:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("Output Mixer: Comb Level: %f\n", _ctrlVal);
            setCombLevel(_ctrlVal);
            break;

        case CtrlID::COMB_PAN:
            _ctrlVal = (_ctrlVal / .63f) - 100.f;

            if (_ctrlVal > 100.f)
            {
                _ctrlVal = 100.f;
            }

            printf("Output Mixer: Comb Pan: %f\n", _ctrlVal);
            setCombPan(_ctrlVal);
            break;

        case CtrlID::SVFILTER_LEVEL:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("Output Mixer: SV Filter Level: %f\n", _ctrlVal);
            setSVFilterLevel(_ctrlVal);
            break;

        case CtrlID::SVFILTER_PAN:
            _ctrlVal = (_ctrlVal / .63f) - 100.f;

            if (_ctrlVal > 100.f)
            {
                _ctrlVal = 100.f;
            }

            printf("Output Mixer: SV Filter Pan: %f\n", _ctrlVal);
            setSVFilterPan(_ctrlVal);
            break;

        case CtrlID::DRIVE:
            _ctrlVal = (_ctrlVal / 127.f) * 25.f;

            printf("Output Mixer: Drive: %f\n", _ctrlVal);
            setDrive(_ctrlVal);
            break;

        case CtrlID::FOLD:
            _ctrlVal = (_ctrlVal / 127.f);

            printf("Output Mixer: Fold: %f\n", _ctrlVal);
            setFold(_ctrlVal);
            break;

        case CtrlID::ASYM:
            _ctrlVal = (_ctrlVal / 127.f);

            printf("Output Mixer: Asym: %f\n", _ctrlVal);
            setAsym(_ctrlVal);
            break;

        case CtrlID::MAIN_LEVEL:
            _ctrlVal = (_ctrlVal / 63.f);

            if (_ctrlVal > 2.f)
            {
                _ctrlVal = 2.f;
            }

            printf("Output Mixer: Main Level: %f\n", NlToolbox::Conversion::af2db(_ctrlVal * _ctrlVal));
            setMainLevel(_ctrlVal);
            break;

        case CtrlID::KEYPAN:
            _ctrlVal = (_ctrlVal / 127.f);

            printf("Output Mixer: Key Pan: %f\n", _ctrlVal);
            setKeyPan(_ctrlVal);
            break;
    }
}



/*****************************************************************************/
/** @brief    Sets the Mix Level of Module A and initializes the respective Smoother
 *  @param    _level
******************************************************************************/

void Outputmixer::setALevel(float _level)
{
#ifdef SMOOTHEROBJ
    mALevel = _level * 2.f;
    mALevelSmoother.initSmoother(mALevel);
#else
    mALevel_target = _level * 2.f;

    mALevel_base = mALevel;
    mALevel_diff = mALevel_target - mALevel_base;

    mOMSmootherMask |= 0x0001; //ID 1;
    mALevel_ramp = 0.0;
#endif
}



/*****************************************************************************/
/** @brief    Sets the Pan of Module A and initializes the respective Smoother
 *  @param    _pan
******************************************************************************/

void Outputmixer::setAPan(float _pan)
{
#ifdef SMOOTHEROBJ
    mAPan = (_pan * 0.005f) + 0.5f;
    mAPanSmoother.initSmoother(mAPan);
#else
    mAPan_target = (_pan * 0.005f) + 0.5f;

    mAPan_base = mAPan;
    mAPan_diff = mAPan_target - mAPan_base;

    mOMSmootherMask|= 0x0002; //ID 2;

    mAPan_ramp = 0.0;
#endif
}



/*****************************************************************************/
/** @brief    Sets the Mix Level of Module B and initializes the respective Smoother
 *  @param    _level
******************************************************************************/

void Outputmixer::setBLevel(float _level)
{
#ifdef SMOOTHEROBJ
    mBLevel = _level * 2.f;
    mBLevelSmoother.initSmoother(mBLevel);
#else
    mBLevel_target = _level * 2.f;

    mBLevel_base = mBLevel;
    mBLevel_diff = mBLevel_target - mBLevel_base;

    mOMSmootherMask|= 0x0004; //ID 3;
    mBLevel_ramp = 0.0;
#endif
}



/*****************************************************************************/
/** @brief    Sets the Pan of Module B and initializes the respective Smoother
 *  @param    _pan
******************************************************************************/

void Outputmixer::setBPan(float _pan)
{
#ifdef SMOOTHEROBJ
    mBPan = (_pan * 0.005f) + 0.5f;
    mBPanSmoother.initSmoother(mBPan);
#else
    mBPan_target = (_pan * 0.005f) + 0.5f;

    mBPan_base = mBPan;
    mBPan_diff = mBPan_target - mBPan_base;

    mOMSmootherMask|= 0x0008; //ID 4;
    mBPan_ramp = 0.0;
#endif
}



/*****************************************************************************/
/** @brief    Sets the Mix Level of the Comb Filter and initializes the respective Smoother
 *  @param    _level
******************************************************************************/

void Outputmixer::setCombLevel(float _level)
{
#ifdef SMOOTHEROBJ
    mCombLevel = _level;
    mCombLevelSmoother.initSmoother(mCombLevel);
#else
    mCombLevel_target = _level;

    mCombLevel_base = mCombLevel;
    mCombLevel_diff = mCombLevel_target - mCombLevel_base;

    mOMSmootherMask|= 0x0010; //ID 5;
    mCombLevel_ramp = 0.0;

#endif
}



/*****************************************************************************/
/** @brief    Sets the Pan of the Comb Filter and initializes the respective Smoother
 *  @param    _pan
******************************************************************************/

void Outputmixer::setCombPan(float _pan)
{
#ifdef SMOOTHEROBJ
    mCombPan = (_pan * 0.005f) + 0.5f;
    mCombPanSmoother.initSmoother(mCombPan);
#else
    mCombPan_target = (_pan * 0.005f) + 0.5f;

    mCombPan_base = mCombPan;
    mCombPan_diff = mCombPan_target - mCombPan_base;

    mOMSmootherMask|= 0x0020; //ID 6;
    mCombPan_ramp = 0.0;
#endif
}



/*****************************************************************************/
/** @brief    Sets the Mix Level of the State Varibale Filter and initializes the respective Smoother
 *  @param    _level
******************************************************************************/

void Outputmixer::setSVFilterLevel(float _level)
{
#ifdef SMOOTHEROBJ
    mSVFilterLevel = _level;
    mSVFilterLevelSmoother.initSmoother(mSVFilterLevel);
#else
    mSVFilterLevel_target = _level;

    mSVFilterLevel_base = mSVFilterLevel;
    mSVFilterLevel_diff = mSVFilterLevel_target - mSVFilterLevel_base;

    mOMSmootherMask|= 0x0040; //ID 7;
    mSVFilterLevel_ramp = 0.0;
#endif
}



/*****************************************************************************/
/** @brief    Sets the Pan of the State Variable Filter and initializes the respective Smoother
 *  @param    _pan
******************************************************************************/

void Outputmixer::setSVFilterPan(float _pan)
{
#ifdef SMOOTHEROBJ
    mSVFilterPan = (_pan * 0.005f) + 0.5f;
    mSVFilterPanSmoother.initSmoother(mSVFilterPan);
#else
    mSVFilterPan_target = (_pan * 0.005f) + 0.5f;

    mSVFilterPan_base = mSVFilterPan;
    mSVFilterPan_diff = mSVFilterPan_target - mSVFilterPan_base;

    mOMSmootherMask|= 0x0080; //ID 8;
    mSVFilterPan_ramp = 0.0;
#endif
}



/*****************************************************************************/
/** @brief    Sets the Drive Level and initializes the respective Smoother
 *  @param    _drive
******************************************************************************/

void Outputmixer::setDrive(float _drive)
{
#ifdef SMOOTHEROBJ
    mDrive = NlToolbox::Conversion::db2af(_drive) * 0.25f;
    mDriveSmoother.initSmoother(mDrive);
#else
    mDrive_target = NlToolbox::Conversion::db2af(_drive) * 0.25f;

    mDrive_base = mDrive;
    mDrive_diff = mDrive_target - mDrive_base;

    mOMSmootherMask|= 0x0100; //ID 9;
    mDrive_ramp = 0.0;
#endif
}



/*****************************************************************************/
/** @brief    Sets the Fold Amount and initializes the respective Smoother
 *  @param    _fold
******************************************************************************/

void Outputmixer::setFold(float _fold)
{
#ifdef SMOOTHEROBJ
    mFold = _fold;
    mFoldSmoother.initSmoother(mFold);
#else
    mFold_target = _fold;

    mFold_base = mFold;
    mFold_diff = mFold_target - mFold_base;

    mOMSmootherMask|= 0x0200; //ID 10;
    mFold_ramp = 0.0;
#endif
}



/*****************************************************************************/
/** @brief    Sets the Asymmetry Amount and initializes the respective Smoother
 *  @param    _asym
******************************************************************************/

void Outputmixer::setAsym(float _asym)
{
#ifdef SMOOTHEROBJ
    mAsym = _asym;
    mAsymSmoother.initSmoother(mAsym);
#else
    mAsym_target = _asym;

    mAsym_base = mAsym;
    mAsym_diff = mAsym_target - mAsym_base;

    mOMSmootherMask|= 0x0400; //ID 11;
    mAsym_ramp = 0.0;
#endif
}



/*****************************************************************************/
/** @brief    Sets the Main Level and initializes the respective Smoother
 *  @param    _asym
******************************************************************************/

void Outputmixer::setMainLevel(float _level)
{
#ifdef SMOOTHEROBJ
    mMainLevel = _level * _level * 0.64f;
    mMainLevelSmoother.initSmoother(mMainLevel);
#else
    mMainLevel_target = _level * _level * 0.64f;

    mMainLevel_base = mMainLevel;
    mMainLevel_diff = mMainLevel_target - mMainLevel_base;

    mOMSmootherMask|= 0x0800; //ID 12;
    mMainLevel_ramp = 0.0;
#endif
}



/*****************************************************************************/
/** @brief    Sets the Key Pan Amount and initializes the respective Smoother
 *  @param    _keyPan
******************************************************************************/

void Outputmixer::setKeyPan(float _keypan)
{
#ifdef SMOOTHEROBJ
    mKeypan = _keypan / 60.f;
    mKeypanSmoother.initSmoother(mKeypan);
#else
    mKeypan_target = _keypan / 60.f;

    mKeypan_base = mKeypan;
    mKeypan_diff = mKeypan_target - mKeypan_base;

    mOMSmootherMask|= 0x1000; //ID 13;
    mKeypan_ramp = 0.0;
#endif
}


/*****************************************************************************/
/** @brief    Calculates key panning values for alle voices depending on the
 *            assigned pitch to the voice
******************************************************************************/

void Outputmixer::calcKeyPan()
{
    for(uint32_t i = 0; i < NUM_VOICES; i++)
    {
        mPitchPanArray[i] = ((gKeyPitch[i] - 66.f) * mKeypan) + 0.f;       // hier fehl noch unison pan
    }
}
