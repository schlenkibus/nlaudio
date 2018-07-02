/******************************************************************************/
/** @file		outputmixer.cpp
    @date		2016-05-30
    @version	1.0
    @author		Anton Schmied[2016-10-20]
    @brief		Outputmixer Class member and method definitions
*******************************************************************************/

#include "outputmixer.h"

/******************************************************************************/
/** Outputmixer Default Constructor
 * @brief    initialization of the modules local variabels with default values
 *          A Level:            0.08
 *          A Pan:              0
 *          B Level:            0.08
 *          B Pan:              0
 *          Comb Level:         0.08
 *          Comb Pan:           0
 *          SV Filter Level:    0.08
 *          SV Filter Pan:      0
 *          Drive:              0
 *          Fold:               0.5
 *          Asym:               0
 *          Main Level:         0.5776 (== -4.47 dB)
 *          Key Pan:            0
*******************************************************************************/

Outputmixer::Outputmixer()
{
    //******************************* Outputs ********************************//
    mSample_L = 0.f;
    mSample_R = 0.f;

    //****************************** Controls ********************************//
    mALevel = 0.08f * 2.f;
    mAPan = (0.f * 0.005f) + 0.5f;
    mBLevel = 0.08f * 2.f;
    mBPan = (0.f * 0.005f) + 0.5f;
    mCombLevel = 0.08f * 2.f;
    mCombPan = (0.f * 0.005f) + 0.5f;
    mSVFilterLevel = 0.08f * 2.f;
    mSVFilterPan = (0.f * 0.005f) + 0.5f;

    mDrive = NlToolbox::Conversion::db2af(0.f) * 0.25f;
    mFold = 0.5f;
    mAsym = 0.f;

    mMainLevel = 0.5776f * 0.64f;
    mKeypan = 0.f;

    //*************************** State Variables ****************************//
    mShaperStateVar_R = 0.f;
    mShaperStateVar_L = 0.f;

    //**************************** Papram Arrays *****************************//
    mKeyPitch = {0.f};
    mAMix_R = {0.f};
    mAMix_L = {0.f};
    mBMix_R = {0.f};
    mBMix_L = {0.f};
    mCombMix_R = {0.f};
    mCombMix_L = {0.f};
    mSVFilterMix_R = {0.f};
    mSVFilterMix_L = {0.f};

    //******************************* Filters ********************************//
    pHighpass_L = new OnePoleFilters(NlToolbox::Conversion::pitch2freq(8.f), 0.f, OnePoleFilterType::HIGHPASS);
    pHighpass_R = new OnePoleFilters(NlToolbox::Conversion::pitch2freq(8.f), 0.f, OnePoleFilterType::HIGHPASS);

    //****************************** Smoothing *******************************//
    mSmootherMask = 0x0000;

    mALevel_ramp = 1.f;
    mAPan_ramp = 1.f;
    mBLevel_ramp = 1.f;
    mBPan_ramp = 1.f;
    mCombLevel_ramp = 1.f;
    mCombPan_ramp = 1.f;
    mSVFilterLevel_ramp = 1.f;
    mSVFilterPan_ramp = 1.f;
    mDrive_ramp = 1.f;
    mFold_ramp = 1.f;
    mAsym_ramp = 1.f;
    mMainLevel_ramp = 1.f;
    mKeypan_ramp = 1.f;
}



/******************************************************************************/
/** Outputmixer Parameterized Constructor
 * @brief    initialization of the modules local variabels with custom
 *           parameters
*******************************************************************************/

Outputmixer::Outputmixer(float _ALevel,
                         float _APan,
                         float _BLevel,
                         float _BPan,
                         float _CombLevel,
                         float _CombPan,
                         float _SVFilterLevel,
                         float _SVFilterPan,
                         float _drive,
                         float _fold,
                         float _asym,
                         float _mainLevel,
                         float _keyPan)
{    //******************************* Outputs ********************************//
    mSample_L = 0.f;
    mSample_R = 0.f;

    //****************************** Controls ********************************//
    mALevel = _ALevel * 2.f;
    mAPan = (_APan * 0.005f) + 0.5f;
    mBLevel = _BLevel * 2.f;
    mBPan = (_BPan * 0.005f) + 0.5f;
    mCombLevel = _CombLevel * 2.f;
    mCombPan = (_CombPan  * 0.005f) + 0.5f;
    mSVFilterLevel = _SVFilterLevel * 2.f;
    mSVFilterPan = (_SVFilterPan  * 0.005f) + 0.5f;

    mDrive = NlToolbox::Conversion::db2af(_drive) * 0.25f;
    mFold = _fold;
    mAsym = _asym;

    mMainLevel = _mainLevel * _mainLevel * 0.64f;
    mKeypan = _keyPan / 60.f;

    //*************************** State Variables ****************************//
    mShaperStateVar_R = 0.f;
    mShaperStateVar_L = 0.f;

    //**************************** Papram Arrays *****************************//
    mKeyPitch = {0.f};
    mAMix_R = {0.f};
    mAMix_L = {0.f};
    mBMix_R = {0.f};
    mBMix_L = {0.f};
    mCombMix_R = {0.f};
    mCombMix_L = {0.f};
    mSVFilterMix_R = {0.f};
    mSVFilterMix_L = {0.f};

    //******************************* Filters ********************************//
    pHighpass_L = new OnePoleFilters(NlToolbox::Conversion::pitch2freq(8.f), 0.f, OnePoleFilterType::HIGHPASS);
    pHighpass_R = new OnePoleFilters(NlToolbox::Conversion::pitch2freq(8.f), 0.f, OnePoleFilterType::HIGHPASS);

    //****************************** Smoothing *******************************//
    mSmootherMask = 0x0000;

    mALevel_ramp = 1.f;
    mAPan_ramp = 1.f;
    mBLevel_ramp = 1.f;
    mBPan_ramp = 1.f;
    mCombLevel_ramp = 1.f;
    mCombPan_ramp = 1.f;
    mSVFilterLevel_ramp = 1.f;
    mSVFilterPan_ramp = 1.f;
    mDrive_ramp = 1.f;
    mFold_ramp = 1.f;
    mAsym_ramp = 1.f;
    mMainLevel_ramp = 1.f;
    mKeypan_ramp = 1.f;
}



/******************************************************************************/
/** Output Mixer Destructor
 * @brief
*******************************************************************************/

Outputmixer::~Outputmixer()
{
    delete pHighpass_L;
    delete pHighpass_R;
}



/******************************************************************************/
/** @brief  main function which calculates a Mix of the incoming Samples from
 *          the Soundgenerator, Comb anfd SV Filters. the Signale can then be
 *          processed by a shaper. All the samples from all voices are combined
 *          and filtered by a hipass filter
*******************************************************************************/

void Outputmixer::applyOutputMixer(uint32_t _voiceNumber, float _sampleA, float _sampleB, float _sampleComb, float _sampleSVFilter)
{
    //********************** Smoothing and Voice Reset ***********************//
    if (_voiceNumber == 0)           // do once for one voice group cycle
    {
        mSample_L = 0.f;
        mSample_R = 0.f;

        if(mSmootherMask)
        {
            applySmoothers();
        }
    }


    //**************************** Left Pan Mixer ****************************//
    float mainSample = (mAMix_L[_voiceNumber] * _sampleA) + (mBMix_L[_voiceNumber] * _sampleB) +
            (mCombMix_L[_voiceNumber] * _sampleComb) + (mSVFilterMix_L[_voiceNumber] * _sampleSVFilter);

    //************************** Shape Left Sample ***************************//
    mainSample = mDrive * mainSample;
    float holdSample = mainSample;

    mainSample = NlToolbox::Math::sinP3_wrap(mainSample);
    mainSample = NlToolbox::Others::threeRanges(mainSample, holdSample, mFold);

    holdSample = mainSample * mainSample;
    holdSample = holdSample - mShaperStateVar_L;
    mShaperStateVar_L = holdSample * 0.00427428f + mShaperStateVar_L + DNC_CONST;

    mainSample = NlToolbox::Others::parAsym(mainSample, holdSample, mAsym);

    mSample_L += mainSample;


    //*************************** Right Pan Mixer ****************************//
    mainSample = (mAMix_R[_voiceNumber] * _sampleA) + (mBMix_R[_voiceNumber] * _sampleB) +
            (mCombMix_R[_voiceNumber] * _sampleComb) + (mSVFilterMix_R[_voiceNumber] * _sampleSVFilter);

    //************************* Shape Right Sample ***************************//
    mainSample = mDrive * mainSample;
    holdSample = mainSample;

    mainSample = NlToolbox::Math::sinP3_wrap(mainSample);
    mainSample = NlToolbox::Others::threeRanges(mainSample, holdSample, mFold);

    holdSample = mainSample * mainSample;
    holdSample = holdSample - mShaperStateVar_R;
    mShaperStateVar_R = holdSample * 0.00427428f + mShaperStateVar_R + DNC_CONST;

    mainSample = NlToolbox::Others::parAsym(mainSample, holdSample, mAsym);

    mSample_R += mainSample;


    //************************** Voice Combinder *****************************//
    if (_voiceNumber + 1 == NUM_VOICES)
    {
        //************************ 1-Pole Highpass ***************************//
        mSample_L = pHighpass_L->applyFilter(mSample_L);
        mSample_R = pHighpass_R->applyFilter(mSample_R);


        //************************** Main Level ******************************//
        mSample_L *= mMainLevel;
        mSample_R *= mMainLevel;
    }
}



/*****************************************************************************/
/** @brief    sets the local value of the pitch once a key is pressed
 *            and calculates the corresponding pitch pan
******************************************************************************/

void Outputmixer::setKeyPitch(uint32_t _voiceNumber, float _keyPitch)
{
    _keyPitch = _keyPitch - 66.f;
    mKeyPitch[_voiceNumber] = _keyPitch;

    float panVar = _keyPitch * mKeypan;                       /// unison pan!!!

    mAMix_R[_voiceNumber] = (panVar + mAPan) * mALevel;
    mAMix_L[_voiceNumber] = (1.f - (panVar + mAPan)) * mALevel;
    mBMix_R[_voiceNumber] = (panVar + mBPan) * mBLevel;
    mBMix_L[_voiceNumber] = (1.f - (panVar + mBPan)) * mBLevel;
    mCombMix_R[_voiceNumber] = (panVar + mCombPan) * mCombLevel;
    mCombMix_L[_voiceNumber] = (1.f - (panVar + mCombPan)) * mCombLevel;
    mSVFilterMix_R[_voiceNumber] = (panVar + mSVFilterPan) * mSVFilterLevel;
    mSVFilterMix_L[_voiceNumber] = (1.f - (panVar + mSVFilterPan)) * mSVFilterLevel;
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
#ifdef PRINT_PARAMVALUES
            printf("Output Mixer: A Level: %f\n", _ctrlVal);
#endif
            // Set Smoothing Ramp
            mALevel_target = _ctrlVal * 2.f;

            mALevel_base = mALevel;
            mALevel_diff = mALevel_target - mALevel_base;
            mALevel_ramp = 0.0;

            mSmootherMask |= 0x0001;        // ID: 1
            break;

        case CtrlID::A_PAN:
            _ctrlVal = (_ctrlVal / .63f) - 100.f;

            if (_ctrlVal > 100.f)
            {
                _ctrlVal = 100.f;
            }
#ifdef PRINT_PARAMVALUES
            printf("Output Mixer: A Pan: %f\n", _ctrlVal);
#endif
            // Set Smoothing Ramp
            mAPan_target = (_ctrlVal * 0.005f) + 0.5f;

            mAPan_base = mAPan;
            mAPan_diff = mAPan_target - mAPan_base;
            mAPan_ramp = 0.0;

            mSmootherMask|= 0x0002;         // ID: 2
            break;

        case CtrlID::B_LEVEL:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }
#ifdef PRINT_PARAMVALUES
            printf("Output Mixer: B Level: %f\n", _ctrlVal);
#endif
            // Set Smoothing Ramp
            mBLevel_target = _ctrlVal * 2.f;

            mBLevel_base = mBLevel;
            mBLevel_diff = mBLevel_target - mBLevel_base;
            mBLevel_ramp = 0.0;

            mSmootherMask|= 0x0004;         //ID: 3
            break;

        case CtrlID::B_PAN:
            _ctrlVal = (_ctrlVal / .63f) - 100.f;

            if (_ctrlVal > 100.f)
            {
                _ctrlVal = 100.f;
            }
#ifdef PRINT_PARAMVALUES
            printf("Output Mixer: B Pan: %f\n", _ctrlVal);
#endif
            // Set Smoothing Ramp
            mBPan_target = (_ctrlVal * 0.005f) + 0.5f;

            mBPan_base = mBPan;
            mBPan_diff = mBPan_target - mBPan_base;
            mBPan_ramp = 0.0;

            mSmootherMask|= 0x0008;         //ID: 4
            break;

        case CtrlID::COMB_LEVEL:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }
#ifdef PRINT_PARAMVALUES
            printf("Output Mixer: Comb Level: %f\n", _ctrlVal);
#endif
            // Set Smoothing Ramp
            mCombLevel_target = _ctrlVal;

            mCombLevel_base = mCombLevel;
            mCombLevel_diff = mCombLevel_target - mCombLevel_base;
            mCombLevel_ramp = 0.0;

            mSmootherMask|= 0x0010;         //ID: 5
            break;

        case CtrlID::COMB_PAN:
            _ctrlVal = (_ctrlVal / .63f) - 100.f;

            if (_ctrlVal > 100.f)
            {
                _ctrlVal = 100.f;
            }
#ifdef PRINT_PARAMVALUES
            printf("Output Mixer: Comb Pan: %f\n", _ctrlVal);
#endif
            // Set Smoothing Ramp
            mCombPan_target = (_ctrlVal * 0.005f) + 0.5f;

            mCombPan_base = mCombPan;
            mCombPan_diff = mCombPan_target - mCombPan_base;
            mCombPan_ramp = 0.0;

            mSmootherMask|= 0x0020;         //ID: 6
            break;

        case CtrlID::SVFILTER_LEVEL:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }
#ifdef PRINT_PARAMVALUES
            printf("Output Mixer: SV Filter Level: %f\n", _ctrlVal);
#endif
            // Set Smoothing Ramp
            mSVFilterLevel_target = _ctrlVal;

            mSVFilterLevel_base = mSVFilterLevel;
            mSVFilterLevel_diff = mSVFilterLevel_target - mSVFilterLevel_base;
            mSVFilterLevel_ramp = 0.0;

            mSmootherMask|= 0x0040;         //ID: 7
            break;

        case CtrlID::SVFILTER_PAN:
            _ctrlVal = (_ctrlVal / .63f) - 100.f;

            if (_ctrlVal > 100.f)
            {
                _ctrlVal = 100.f;
            }
#ifdef PRINT_PARAMVALUES
            printf("Output Mixer: SV Filter Pan: %f\n", _ctrlVal);
#endif
            // Set Smoothing Ramp
            mSVFilterPan_target = (_ctrlVal * 0.005f) + 0.5f;

            mSVFilterPan_base = mSVFilterPan;
            mSVFilterPan_diff = mSVFilterPan_target - mSVFilterPan_base;
            mSVFilterPan_ramp = 0.0;

            mSmootherMask|= 0x0080;         //ID: 8
            break;

        case CtrlID::DRIVE:
            _ctrlVal = (_ctrlVal / 127.f) * 50.f;
#ifdef PRINT_PARAMVALUES
            printf("Output Mixer: Drive: %f\n", _ctrlVal);
#endif
            // Set Smoothing Ramp
            mDrive_target = NlToolbox::Conversion::db2af(_ctrlVal) * 0.25f;

            mDrive_base = mDrive;
            mDrive_diff = mDrive_target - mDrive_base;
            mDrive_ramp = 0.0;

            mSmootherMask|= 0x0100;         //ID: 9
            break;

        case CtrlID::FOLD:
            _ctrlVal = (_ctrlVal / 127.f);
#ifdef PRINT_PARAMVALUES
            printf("Output Mixer: Fold: %f\n", _ctrlVal);
#endif
            // Set Smoothing Ramp
            mFold_target = _ctrlVal;

            mFold_base = mFold;
            mFold_diff = mFold_target - mFold_base;
            mFold_ramp = 0.0;

            mSmootherMask|= 0x0200;         //ID: 10
            break;

        case CtrlID::ASYM:
            _ctrlVal = (_ctrlVal / 127.f);
#ifdef PRINT_PARAMVALUES
            printf("Output Mixer: Asym: %f\n", _ctrlVal);
#endif
            // Set Smoothing Ramp
            mAsym_target = _ctrlVal;

            mAsym_base = mAsym;
            mAsym_diff = mAsym_target - mAsym_base;
            mAsym_ramp = 0.0;

            mSmootherMask|= 0x0400;         //ID: 11
            break;

        case CtrlID::MAIN_LEVEL:
            _ctrlVal = (_ctrlVal / 63.f);

            if (_ctrlVal > 2.f)
            {
                _ctrlVal = 2.f;
            }
#ifdef PRINT_PARAMVALUES
            printf("Output Mixer: Main Level: %f\n", NlToolbox::Conversion::af2db(_ctrlVal * _ctrlVal));
#endif
            // Set Smoothing Ramp
            mMainLevel_target = _ctrlVal * _ctrlVal * 0.64f;

            mMainLevel_base = mMainLevel;
            mMainLevel_diff = mMainLevel_target - mMainLevel_base;
            mMainLevel_ramp = 0.0;

            mSmootherMask|= 0x0800;         //ID: 12
            break;

        case CtrlID::KEYPAN:
            _ctrlVal = (_ctrlVal / 127.f);
#ifdef PRINT_PARAMVALUES
            printf("Output Mixer: Key Pan: %f\n", _ctrlVal);
#endif
            // Set Smoothing Ramp
            mKeypan_target = _ctrlVal / 60.f;

            mKeypan_base = mKeypan;
            mKeypan_diff = mKeypan_target - mKeypan_base;
            mKeypan_ramp = 0.0;

            mSmootherMask|= 0x1000;         //ID: 13
            break;
    }
}



/******************************************************************************/
/** @brief    calls the smoothing functions for each sample
*******************************************************************************/

void Outputmixer::applySmoothers()
{
    //*********************** ID 1: A Level Smoother *************************//
    if (mALevel_ramp < 1.0)
    {
        mALevel_ramp += SMOOTHER_INC;

        if (mALevel_ramp > 1.0)
        {
            mALevel = mALevel_target;
            mSmootherMask &= 0xFFFE;      // switch first bit to 0
        }
        else
        {
            mALevel = mALevel_base + mALevel_diff * mALevel_ramp;
        }

        for(uint32_t voiceNumber = 0; voiceNumber < NUM_VOICES; voiceNumber++)
        {
            mAMix_R[voiceNumber] = (mKeyPitch[voiceNumber] * mKeypan + mAPan) * mALevel;
            mAMix_L[voiceNumber] = (1.f - (mKeyPitch[voiceNumber] * mKeypan + mAPan)) * mALevel;
        }
    }

    //************************ ID 2: A Pan Smoother **************************//
    if (mAPan_ramp < 1.0)
    {
        mAPan_ramp += SMOOTHER_INC;

        if (mAPan_ramp > 1.0)
        {
            mAPan = mAPan_target;
            mSmootherMask &= 0xFFFD;      // switch second bit to 0
        }
        else
        {
            mAPan = mAPan_base + mAPan_diff * mAPan_ramp;
        }

        for(uint32_t voiceNumber = 0; voiceNumber < NUM_VOICES; voiceNumber++)
        {
            mAMix_R[voiceNumber] = (mKeyPitch[voiceNumber] * mKeypan + mAPan) * mALevel;
            mAMix_L[voiceNumber] = (1.f - (mKeyPitch[voiceNumber] * mKeypan + mAPan)) * mALevel;
        }
    }

    //*********************** ID 3: B Level Smoother *************************//
    if (mBLevel_ramp < 1.0)
    {
        mBLevel_ramp += SMOOTHER_INC;

        if (mBLevel_ramp > 1.0)
        {
            mBLevel = mBLevel_target;
            mSmootherMask &= 0xFFFB;      // switch third bit to 0
        }
        else
        {
            mBLevel = mBLevel_base + mBLevel_diff * mBLevel_ramp;
        }

        for(uint32_t voiceNumber = 0; voiceNumber < NUM_VOICES; voiceNumber++)
        {
            mBMix_R[voiceNumber] = (mKeyPitch[voiceNumber] * mKeypan + mBPan) * mBLevel;
            mBMix_L[voiceNumber] = (1.f - (mKeyPitch[voiceNumber] * mKeypan + mBPan)) * mBLevel;
        }
    }

    //************************ ID 4: B Pan Smoother **************************//
    if (mBPan_ramp < 1.0)
    {
        mBPan_ramp += SMOOTHER_INC;

        if (mBPan_ramp > 1.0)
        {
            mBPan = mBPan_target;
            mSmootherMask &= 0xFFF7;      // switch fourth bit to 0
        }
        else
        {
            mBPan = mBPan_base + mBPan_diff * mBPan_ramp;
        }

        for(uint32_t voiceNumber = 0; voiceNumber < NUM_VOICES; voiceNumber++)
        {
            mBMix_R[voiceNumber] = (mKeyPitch[voiceNumber] * mKeypan + mBPan) * mBLevel;
            mBMix_L[voiceNumber] = (1.f - (mKeyPitch[voiceNumber] * mKeypan + mBPan)) * mBLevel;
        }
    }

    //********************* ID 5: Comb Level Smoother ************************//
    if (mCombLevel_ramp < 1.0)
    {
        mCombLevel_ramp += SMOOTHER_INC;

        if (mCombLevel_ramp > 1.0)
        {
            mCombLevel = mCombLevel_target;
            mSmootherMask &= 0xFFEF;      // switch fifth bit to 0
        }
        else
        {
            mCombLevel = mCombLevel_base + mCombLevel_diff * mCombLevel_ramp;
        }

        for(uint32_t voiceNumber = 0; voiceNumber < NUM_VOICES; voiceNumber++)
        {
            mCombMix_R[voiceNumber] = (mKeyPitch[voiceNumber] * mKeypan + mCombPan) * mCombLevel;
            mCombMix_L[voiceNumber] = (1.f - (mKeyPitch[voiceNumber] * mKeypan + mCombPan)) * mCombLevel;
        }
    }

    //********************** ID 6: Comb Pan Smoother *************************//
    if (mCombPan_ramp < 1.0)
    {
        mCombPan_ramp += SMOOTHER_INC;

        if (mCombPan_ramp > 1.0)
        {
            mCombPan = mCombPan_target;
            mSmootherMask &= 0xFFDF;      // switch sixth bit to 0
        }
        else
        {
            mCombPan = mCombPan_base + mCombPan_diff * mCombPan_ramp;
        }

        for(uint32_t voiceNumber = 0; voiceNumber < NUM_VOICES; voiceNumber++)
        {
            mCombMix_R[voiceNumber] = (mKeyPitch[voiceNumber] * mKeypan + mCombPan) * mCombLevel;
            mCombMix_L[voiceNumber] = (1.f - (mKeyPitch[voiceNumber] * mKeypan + mCombPan)) * mCombLevel;
        }
    }

    //******************* ID 7: SV Filter Level Smoother *********************//
    if (mSVFilterLevel_ramp < 1.0)
    {
        mSVFilterLevel_ramp += SMOOTHER_INC;

        if (mSVFilterLevel_ramp > 1.0)
        {
            mSVFilterLevel = mSVFilterLevel_target;
            mSmootherMask &= 0xFFBF;      // switch seventh bit to 0
        }
        else
        {
            mSVFilterLevel = mSVFilterLevel_base + mSVFilterLevel_diff * mSVFilterLevel_ramp;
        }

        for(uint32_t voiceNumber = 0; voiceNumber < NUM_VOICES; voiceNumber++)
        {
            mSVFilterMix_R[voiceNumber] = (mKeyPitch[voiceNumber] * mKeypan + mSVFilterPan) * mSVFilterLevel;
            mSVFilterMix_L[voiceNumber] = (1.f - (mKeyPitch[voiceNumber] * mKeypan + mSVFilterPan)) * mSVFilterLevel;
        }
    }

    //******************** ID 8: SV Filter Pan Smoother **********************//
    if (mSVFilterPan_ramp < 1.0)
    {
        mSVFilterPan_ramp += SMOOTHER_INC;

        if (mSVFilterPan_ramp > 1.0)
        {
            mSVFilterPan = mSVFilterPan_target;
            mSmootherMask &= 0xFF7F;      // switch eighth bit to 0
        }
        else
        {
            mSVFilterPan = mSVFilterPan_base + mSVFilterPan_diff * mSVFilterPan_ramp;
        }

        for(uint32_t voiceNumber = 0; voiceNumber < NUM_VOICES; voiceNumber++)
        {
            mSVFilterMix_R[voiceNumber] = (mKeyPitch[voiceNumber] * mKeypan + mSVFilterPan) * mSVFilterLevel;
            mSVFilterMix_L[voiceNumber] = (1.f - (mKeyPitch[voiceNumber] * mKeypan + mSVFilterPan)) * mSVFilterLevel;
        }
    }

    //************************ ID 9: Drive Smoother **************************//
    if (mDrive_ramp < 1.0)
    {
        mDrive_ramp += SMOOTHER_INC;

        if (mDrive_ramp > 1.0)
        {
            mDrive = mDrive_target;
            mSmootherMask &= 0xFEFF;      // switch ninth bit to 0
        }
        else
        {
            mDrive = mDrive_base + mDrive_diff * mDrive_ramp;
        }
    }

    //************************ ID 10: Fold Smoother **************************//
    if (mFold_ramp < 1.0)
    {
        mFold_ramp += SMOOTHER_INC;

        if (mFold_ramp > 1.0)
        {
            mFold = mFold_target;
            mSmootherMask &= 0xFDFF;      // switch tenth bit to 0
        }
        else
        {
            mFold = mFold_base + mFold_diff * mFold_ramp;
        }
    }

    //************************ ID 11: Asym Smoother **************************//
    if (mAsym_ramp < 1.0)
    {
        mAsym_ramp += SMOOTHER_INC;

        if (mAsym_ramp > 1.0)
        {
            mAsym = mAsym_target;
            mSmootherMask &= 0xFBFF;      // switch 11th bit to 0
        }
        else
        {
            mAsym = mAsym_base + mAsym_diff * mAsym_ramp;
        }
    }

    //********************* ID 12: Main Level Smoother ***********************//
    if (mMainLevel_ramp < 1.0)
    {
        mMainLevel_ramp += SMOOTHER_INC;

        if (mMainLevel_ramp > 1.0)
        {
            mMainLevel = mMainLevel_target;
            mSmootherMask &= 0xF7FF;      // switch 12th bit to 0
        }
        else
        {
            mMainLevel = mMainLevel_base + mMainLevel_diff * mMainLevel_ramp;
        }
    }

    //********************** ID 13: Key Pan Smoother *************************//
    if (mKeypan_ramp < 1.0)
    {
        mKeypan_ramp += SMOOTHER_INC;

        if (mKeypan_ramp > 1.0)
        {
            mKeypan = mKeypan_target;
            mSmootherMask &= 0xFFF;       // switch 13th bit to 0
        }
        else
        {
            mKeypan = mKeypan_base + mKeypan_diff * mKeypan_ramp;
        }

        for(uint32_t voiceNumber = 0; voiceNumber < NUM_VOICES; voiceNumber++)
        {
            mAMix_R[voiceNumber] = (mKeyPitch[voiceNumber] * mKeypan + mAPan) * mALevel;
            mAMix_L[voiceNumber] = (1.f - (mKeyPitch[voiceNumber] * mKeypan + mAPan)) * mALevel;
            mBMix_R[voiceNumber] = (mKeyPitch[voiceNumber] * mKeypan + mBPan) * mBLevel;
            mBMix_L[voiceNumber] = (1.f - (mKeyPitch[voiceNumber] * mKeypan + mBPan)) * mBLevel;
            mCombMix_R[voiceNumber] = (mKeyPitch[voiceNumber] * mKeypan + mCombPan) * mCombLevel;
            mCombMix_L[voiceNumber] = (1.f - (mKeyPitch[voiceNumber] * mKeypan + mCombPan)) * mCombLevel;
            mSVFilterMix_R[voiceNumber] = (mKeyPitch[voiceNumber] * mKeypan + mSVFilterPan) * mSVFilterLevel;
            mSVFilterMix_L[voiceNumber] = (1.f - (mKeyPitch[voiceNumber] * mKeypan + mSVFilterPan)) * mSVFilterLevel;
        }
    }
}

