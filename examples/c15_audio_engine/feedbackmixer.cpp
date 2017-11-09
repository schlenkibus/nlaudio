/******************************************************************************/
/** @file		feedbackmixer.cpp
    @date		2017-06-07
    @version	0.0
    @author		Anton Schmied[2017-06-07]
    @brief		Feedbackmixer Class member and method definitions
*******************************************************************************/

#include "feedbackmixer.h"

/******************************************************************************/
/** Feedback Mixer Default Constructor
 * @brief    initialization of the modules local variabels with default values
*******************************************************************************/

FeedbackMixer::FeedbackMixer()
{
    mCombLevel = 0.008f;
    mSVFilterLevel = 0.008f;
    mEffectsLevel = 0.000064f;
    mReverbLevel = 0.f;

    mDrive = NlToolbox::Conversion::db2af(5.1f) * 0.25f;
    mFold = 0.5f;
    mAsym = 0.f;

    mMainLevel = 0.650281f;
    mKeyTracking = 0.f;

    mShaperStateVar = 0.f;

    pHighpass = new OnePoleFilters(SAMPLERATE, 20.f, 0.f, OnePoleFilterType::HIGHPASS);

    mSmootherMask = 0x0000;
    mCombLevel_ramp = 1.f;
    mSVFilterLevel_ramp = 1.f;
    mEffectsLevel_ramp = 1.f;
    mReverbLevel_ramp = 1.f;
    mDrive_ramp = 1.f;
    mFold_ramp = 1.f;
    mAsym_ramp = 1.f;
    mLevel_ramp = 1.f;
}



/******************************************************************************/
/** Feedback Mixer Parameterized Constructor
 * @brief    initialization of the modules local variabels with custom
 *           parameters
*******************************************************************************/

FeedbackMixer::FeedbackMixer(float _CombLevel,
                             float _SVFilterLevel,
                             float _EffectsLevel,
                             float _ReverbLevel,
                             float _drive,
                             float _fold,
                             float _asym,
                             float _mainLevel,
                             float _keyTracking)
{
    mCombLevel = _CombLevel;
    mSVFilterLevel = _SVFilterLevel;
    mEffectsLevel = _EffectsLevel * fabs(_EffectsLevel);
    mReverbLevel = _ReverbLevel;

    mDrive = NlToolbox::Conversion::db2af(_drive) * 0.25f;
    mFold = _fold;
    mAsym = _asym;

    mMainLevel = _mainLevel * _mainLevel;
    mKeyTracking = _keyTracking;

    mShaperStateVar = 0.f;

    pHighpass = new OnePoleFilters(SAMPLERATE, 20.f, 0.f, OnePoleFilterType::HIGHPASS);

    mSmootherMask = 0x0000;
    mCombLevel_ramp = 1.f;
    mSVFilterLevel_ramp = 1.f;
    mEffectsLevel_ramp = 1.f;
    mReverbLevel_ramp = 1.f;
    mDrive_ramp = 1.f;
    mFold_ramp = 1.f;
    mAsym_ramp = 1.f;
    mLevel_ramp = 1.f;
}



/******************************************************************************/
/** Feedback Mixer Destructor
 * @brief
*******************************************************************************/

FeedbackMixer::~FeedbackMixer()
{
    delete pHighpass;
}


/******************************************************************************/
/** @brief    main function which ...
*******************************************************************************/

void FeedbackMixer::applyFeedbackMixer(float _CombSample, float _SVFilterSample, float _FeedbackSample)
{
    //****************************** Smoothing *******************************//

    if(mSmootherMask)
    {
        applySmoothers();
    }


    //**************************** Sample Mixer ******************************//

    mFeedbackOut = _FeedbackSample * mEffectsLevel + _CombSample * mCombLevel + _SVFilterSample * mSVFilterLevel;


    //******************************* Filter *********************************//

    mFeedbackOut = pHighpass->applyFilter(mFeedbackOut);


    //******************************* Shaper *********************************//

    mFeedbackOut = mDrive * mFeedbackOut;

    float ctrlSample = mFeedbackOut;

    mFeedbackOut = NlToolbox::Math::sinP3(mFeedbackOut);
    mFeedbackOut = NlToolbox::Others::threeRanges(mFeedbackOut, ctrlSample, mFold);

    float squareSample = mFeedbackOut * mFeedbackOut;
    squareSample = squareSample - mShaperStateVar;
    mShaperStateVar = squareSample * 0.00427428f + mShaperStateVar + DNC_CONST;

    mFeedbackOut = NlToolbox::Others::parAsym(mFeedbackOut, squareSample, mAsym);

    mFeedbackOut = mMainLevel * mFeedbackOut;
}



/*****************************************************************************/
/** @brief    calculates the Highpass Cut Frequency and the effect on the
 *            Main Level if Keytracking is > 0
******************************************************************************/

void FeedbackMixer::setPitchInfluence(float _keyPitch)
{
    mPitch = _keyPitch;
    mPitchInfluence = NlToolbox::Conversion::db2af(mKeyTracking * _keyPitch);

    mMainLevel = mLevel_current * mPitchInfluence;

    pHighpass->setCutFreq(NlToolbox::Conversion::pitch2freq(_keyPitch + 12.f));
}



/*****************************************************************************/
/** @brief    interface method which converts and scales the incoming midi
 *            values and passes these to the respective methods or parameters
 *  @param    midi control ID -> enum CtrlID (feedbackmixer.h)
 *  @param    midi control value [0 ... 127]
******************************************************************************/

void FeedbackMixer::setFeedbackMixerParams(unsigned char _ctrlID, float _ctrlVal)
{
    switch (_ctrlID)
    {
        case CtrlID::COMB_LEVEL:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("Feedback Mixer - Comb Level: %f\n", _ctrlVal);

            // Set Smoothing Ramp
            mCombLevel_target = _ctrlVal;
            mCombLevel_base = mCombLevel;
            mCombLevel_diff = mCombLevel_target - mCombLevel_base;
            mCombLevel_ramp = 0.f;

            mSmootherMask |= 0x0001;        // ID: 1
            break;

        case CtrlID::SVFILTER_LEVEL:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("Feedback Mixer - SV Filter Level: %f\n", _ctrlVal);

            // Set Smoothing Ramp
            mSVFilterLevel_target = _ctrlVal;
            mSVFilterLevel_base = mSVFilterLevel;
            mSVFilterLevel_diff = mSVFilterLevel_target - mSVFilterLevel_base;
            mSVFilterLevel_ramp = 0.f;

            mSmootherMask |= 0x0002;        // ID: 2
            break;

        case CtrlID::EFFECTS_LEVEL:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("Feedback Mixer - Effects Level: %f\n", _ctrlVal);

            // Set Smoothing Ramp
            mEffectsLevel_target = _ctrlVal * fabs(_ctrlVal);
            mEffectsLevel_base = mEffectsLevel;
            mEffectsLevel_diff = mEffectsLevel_target - mEffectsLevel_base;
            mEffectsLevel_ramp = 0.f;

            mSmootherMask |= 0x0004;        // ID: 3
            break;

        case CtrlID::REVERB_LEVEL:
            _ctrlVal = (_ctrlVal / 126.f);

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("Feedback Mixer - Reverb Level: %f\n", _ctrlVal);

            // Set Smoothing Ramp
            mReverbLevel_target = _ctrlVal;
            mReverbLevel_base = mReverbLevel;
            mReverbLevel_diff = mReverbLevel_target - mReverbLevel_base;
            mReverbLevel_ramp = 0.f;

            mSmootherMask |= 0x0008;        // ID: 4
            break;

        case CtrlID::DRIVE:
            _ctrlVal = (_ctrlVal / 127.f) * 50.f;

            printf("Feedback Mixer - Drive: %f\n", _ctrlVal);

            // Set Smoothing Ramp
            mDrive_target = NlToolbox::Conversion::db2af(_ctrlVal) * 0.25f;

            mDrive_base = mDrive;
            mDrive_diff = mDrive_target - mDrive_base;
            mDrive_ramp = 0.0;

            mSmootherMask|= 0x0010;         //ID: 5
            break;

        case CtrlID::FOLD:
            _ctrlVal = (_ctrlVal / 127.f);

            printf("Feedback Mixer - Fold: %f\n", _ctrlVal);

            // Set Smoothing Ramp
            mFold_target = _ctrlVal;

            mFold_base = mFold;
            mFold_diff = mFold_target - mFold_base;
            mFold_ramp = 0.0;

            mSmootherMask|= 0x0020;         //ID: 6
            break;

        case CtrlID::ASYM:
            _ctrlVal = (_ctrlVal / 127.f);

            printf("Feedback Mixer - Asym: %f\n", _ctrlVal);

            // Set Smoothing Ramp
            mAsym_target = _ctrlVal;

            mAsym_base = mAsym;
            mAsym_diff = mAsym_target - mAsym_base;
            mAsym_ramp = 0.0;

            mSmootherMask|= 0x0040;         //ID: 7
            break;

        case CtrlID::LEVEL:
            _ctrlVal = (_ctrlVal / 63.f);

            if (_ctrlVal > 2.f)
            {
                _ctrlVal = 2.f;
            }

            printf("Feedback Mixer: Level: %f\n", NlToolbox::Conversion::af2db(_ctrlVal * _ctrlVal));

            // Set Smoothing Ramp
            mLevel_target = _ctrlVal * _ctrlVal;

            mLevel_base = mLevel_current;
            mLevel_diff = mLevel_target - mLevel_base;
            mLevel_ramp = 0.0;

            mSmootherMask|= 0x0080;         //ID: 8
            break;

        case CtrlID::KEY_TRACKING:

            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("Feedback Mixer: Key Tracking: %f\n", _ctrlVal);

            mPitchInfluence = NlToolbox::Conversion::db2af(_ctrlVal * mPitch);
            mMainLevel = mLevel_current * mPitchInfluence;
            break;
    }
}



/******************************************************************************/
/** @brief    calls the smoothing functions f0r each sample
*******************************************************************************/

void FeedbackMixer::applySmoothers()
{
    //********************* ID 1: Comb Level Smoother ************************//
    if (mCombLevel_ramp < 1.f)
    {
        mCombLevel_ramp += SMOOTHER_INC;

        if (mCombLevel_ramp > 1.0)
        {
            mCombLevel = mCombLevel_target;
            mSmootherMask &= 0xFFFE;        // switch first bit to 0
        }
        else
        {
            mCombLevel = mCombLevel_base + mCombLevel_diff * mCombLevel_ramp;
        }
    }

    //****************** ID 2: SV Filter Level Smoother **********************//
    if (mSVFilterLevel_ramp < 1.0)
    {
        mSVFilterLevel_ramp += SMOOTHER_INC;

        if (mSVFilterLevel_ramp > 1.f)
        {
            mSVFilterLevel = mSVFilterLevel_target;
            mSmootherMask &= 0xFFFD;        // switch second bit to 0
        }
        else
        {
            mSVFilterLevel = mSVFilterLevel_base + mSVFilterLevel_diff * mSVFilterLevel_ramp;
        }
    }

    //******************* ID 3: Effects Level Smoother ***********************//
    if (mEffectsLevel_ramp < 1.0)
    {
        mEffectsLevel_ramp += SMOOTHER_INC;

        if (mEffectsLevel_ramp > 1.0)
        {
            mEffectsLevel = mEffectsLevel_target;
            mSmootherMask &= 0xFFFB;      // switch third bit to 0
        }
        else
        {
            mEffectsLevel = mEffectsLevel_base + mEffectsLevel_diff * mEffectsLevel_ramp;
        }
    }

    //******************** ID 4: Reverb Level Smoother ***********************//
    if (mReverbLevel_ramp < 1.0)
    {
        mReverbLevel_ramp += SMOOTHER_INC;

        if (mReverbLevel_ramp > 1.0)
        {
            mReverbLevel = mReverbLevel_target;
            mSmootherMask &= 0xFFF7;      // switch fourth bit to 0
        }
        else
        {
            mReverbLevel = mReverbLevel_base + mReverbLevel_diff * mReverbLevel_ramp;
        }
    }

    //*********************** ID 5: Drive Smoother ***************************//
    if (mDrive_ramp < 1.0)
    {
        mDrive_ramp += SMOOTHER_INC;

        if (mDrive_ramp > 1.0)
        {
            mDrive = mDrive_target;
            mSmootherMask &= 0xFFEF;      // switch fifth bit to 0
        }
        else
        {
            mDrive = mDrive_base + mDrive_diff * mDrive_ramp;
        }
    }

    //************************ ID 6: Fold Smoother ***************************//
    if (mFold_ramp < 1.0)
    {
        mFold_ramp += SMOOTHER_INC;

        if (mFold_ramp > 1.0)
        {
            mFold = mFold_target;
            mSmootherMask &= 0xFFDF;      // switch sixth bit to 0
        }
        else
        {
            mFold = mFold_base + mFold_diff * mFold_ramp;
        }
    }

    //************************ ID 7: Asym Smoother ***************************//
    if (mAsym_ramp < 1.0)
    {
        mAsym_ramp += SMOOTHER_INC;

        if (mAsym_ramp > 1.0)
        {
            mAsym = mAsym_target;
            mSmootherMask &= 0xFFBF;      // switch seventh bit to 0
        }
        else
        {
            mAsym = mAsym_base + mAsym_diff * mAsym_ramp;
        }
    }

    //************************ ID 8: Level Smoother **************************//
    if (mLevel_ramp < 1.0)
    {
        mLevel_ramp += SMOOTHER_INC;

        if (mLevel_ramp > 1.0)
        {
            mLevel_current = mLevel_target;
            mSmootherMask &= 0xFF7F;      // switch eighth bit to 0
        }
        else
        {
            mLevel_current = mLevel_base + mLevel_diff * mLevel_ramp;
        }

        mMainLevel = mLevel_current * mPitchInfluence;
    }
}
