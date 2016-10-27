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
    //-------------------------- Smoother
    : mALevelSmoother(48000.f, 0.032f)
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
/** @brief    main function which calculates the mix of the A and B samples
 *            depending ...
*******************************************************************************/

void Outputmixer::applyOutputmixer()
{
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
    calcKeyPan();

    float a_R[NUM_VOICES] = {};
    float a_L[NUM_VOICES] = {};

    float b_R[NUM_VOICES] = {};
    float b_L[NUM_VOICES] = {};

    float main_L[NUM_VOICES] = {};
    float main_R[NUM_VOICES] = {};

    float ctrlSample = 0.f;
    float squareSample = 0.f;

    // Pan + Sine Sat for all voices
    for (int i = 0; i < NUM_VOICES; i++)
    {
        // Pan + Smooth .. .naja ... eher nur Pan!
        a_R[i] = (mKeyPanArray[i] + mAPan) * mALevel;
        a_L[i] = (mKeyPanArray[i] + (1.f - mAPan)) * mALevel;


        b_R[i] = (mKeyPanArray[i] + mBPan) * mBLevel;
        b_L[i] = (mKeyPanArray[i] + (1.f - mBPan)) * mBLevel;

        // apply pan to samples for each voice
        main_L[i] = (a_L[i] * gSoundGenOut_A[i]) + (b_L[i] * gSoundGenOut_B[i]);
        main_R[i] = (a_R[i] * gSoundGenOut_A[i]) + (b_R[i] * gSoundGenOut_B[i]);

        // Sine Sat L
        main_L[i] = mDrive * main_L[i];
        ctrlSample = main_L[i];

        main_L[i] = NlToolbox::Math::sinP3(main_L[i]);
        main_L[i] = NlToolbox::Others::threeRanges(main_L[i], ctrlSample, mFold);

        squareSample = main_L[i] * main_L[i];
        main_L[i] = NlToolbox::Others::parAsym(main_L[i], squareSample, mAsym);

        // Sine Sat R
        main_R[i] = mDrive * main_R[i];
        ctrlSample = main_R[i];

        main_R[i] = NlToolbox::Math::sinP3(main_R[i]);
        main_R[i] = NlToolbox::Others::threeRanges(main_R[i], ctrlSample, mFold);

        squareSample = main_R[i] * main_R[i];
        main_R[i] = NlToolbox::Others::parAsym(main_R[i], squareSample, mAsym);
    }

    mSample_L = 0.f;
    mSample_R = 0.f;

    // Voice combiner - Alle Stimmen "stumpf" addieren!!
    for (int i = 0; i < NUM_VOICES; i++)
    {
        mSample_L += main_L[i];
        mSample_R += main_R[i];
    }

    // Level adustment
    mSample_L *= mMainLevel;
    mSample_R *= mMainLevel;
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
    mALevel = _level;
    mALevelSmoother.initSmoother(mALevel);

}



/*****************************************************************************/
/** @brief    Sets the Pan of Module A and initializes the respective Smoother
 *  @param    _pan
******************************************************************************/

void Outputmixer::setAPan(float _pan)
{
    mAPan = (_pan * 0.005f) + 0.5f;
    mAPanSmoother.initSmoother(mAPan);
}



/*****************************************************************************/
/** @brief    Sets the Mix Level of Module B and initializes the respective Smoother
 *  @param    _level
******************************************************************************/

void Outputmixer::setBLevel(float _level)
{
    mBLevel = _level;
    mBLevelSmoother.initSmoother(mBLevel);
}



/*****************************************************************************/
/** @brief    Sets the Pan of Module B and initializes the respective Smoother
 *  @param    _pan
******************************************************************************/

void Outputmixer::setBPan(float _pan)
{
    mBPan = (_pan * 0.005f) + 0.5f;
    mBPanSmoother.initSmoother(mBPan);
}



/*****************************************************************************/
/** @brief    Sets the Mix Level of the Comb Filter and initializes the respective Smoother
 *  @param    _level
******************************************************************************/

void Outputmixer::setCombLevel(float _level)
{
    mCombLevel = _level;
    mCombLevelSmoother.initSmoother(mCombLevel);
}



/*****************************************************************************/
/** @brief    Sets the Pan of the Comb Filter and initializes the respective Smoother
 *  @param    _pan
******************************************************************************/

void Outputmixer::setCombPan(float _pan)
{
    mCombPan = (_pan * 0.005f) + 0.5f;
    mCombPanSmoother.initSmoother(mCombPan);
}



/*****************************************************************************/
/** @brief    Sets the Mix Level of the State Varibale Filter and initializes the respective Smoother
 *  @param    _level
******************************************************************************/

void Outputmixer::setSVFilterLevel(float _level)
{
    mSVFilterLevel = _level;
    mSVFilterLevelSmoother.initSmoother(mSVFilterLevel);
}



/*****************************************************************************/
/** @brief    Sets the Pan of the State Variable Filter and initializes the respective Smoother
 *  @param    _pan
******************************************************************************/

void Outputmixer::setSVFilterPan(float _pan)
{
    mSVFilterPan = (_pan * 0.005f) + 0.5f;
    mSVFilterPanSmoother.initSmoother(mSVFilterPan);
}



/*****************************************************************************/
/** @brief    Sets the Drive Level and initializes the respective Smoother
 *  @param    _drive
******************************************************************************/

void Outputmixer::setDrive(float _drive)
{
    mDrive = NlToolbox::Conversion::db2af(_drive) * 0.25f;
    mDriveSmoother.initSmoother(mDrive);
}



/*****************************************************************************/
/** @brief    Sets the Fold Amount and initializes the respective Smoother
 *  @param    _fold
******************************************************************************/

void Outputmixer::setFold(float _fold)
{
    mFold = _fold;
    mFoldSmoother.initSmoother(mFold);
}



/*****************************************************************************/
/** @brief    Sets the Asymmetry Amount and initializes the respective Smoother
 *  @param    _asym
******************************************************************************/

void Outputmixer::setAsym(float _asym)
{
    mAsym = _asym;
    mAsymSmoother.initSmoother(mAsym);
}



/*****************************************************************************/
/** @brief    Sets the Main Level and initializes the respective Smoother
 *  @param    _asym
******************************************************************************/

void Outputmixer::setMainLevel(float _level)
{
    mMainLevel = _level * _level * 0.64f;
    mMainLevelSmoother.initSmoother(mMainLevel);
}



/*****************************************************************************/
/** @brief    Sets the Key Pan Amount and initializes the respective Smoother
 *  @param    _keyPan
******************************************************************************/

void Outputmixer::setKeyPan(float _keypan)
{
    mKeypan = _keypan / 60.f;
    mKeypanSmoother.initSmoother(mKeypan);
}


/*****************************************************************************/
/** @brief    Calculates key panning values for alle voices depending on the
 *            assigned pitch to the voice
******************************************************************************/

void Outputmixer::calcKeyPan()
{
    for(int i = 0; i < NUM_VOICES; i++)
    {
        mKeyPanArray[i] = (gKeyPitch[i] * mKeypan) + 0.f;       // hier fehl noch unison pan
    }
}
