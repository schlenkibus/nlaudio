/******************************************************************************/
/** @file		soundgenerator.cpp
    @date		2016-12-01
    @version	0.1
    @author		Anton Schmied[2016-12-01]
    @brief		OneSoundgenerator Class member and method definitions
*******************************************************************************/

#include "onesoundgenerator.h"

/******************************************************************************/
/** Soundgenerator Default Constructor
 * @brief    initialization of the modules local variabels with default values
 *           SampleRate:            48 kHz
 *           Phase:                 0.f
 *           Pitch Offset:          60.f ct
 *           Key Tracking:          1.f
 *           Main Mix Amount:       0.f
*******************************************************************************/

OneSoundgenerator::OneSoundgenerator()
    : mSampleRate(48000.f)
{
    /*erst mal 12 Oscillatoren*/
    for(uint32_t i = 0; i < NUM_VOICES; i++)
    {
        moduleA.mOsc[i] = Oscillator();
        moduleB.mOsc[i] = Oscillator();

        //State Variables
        moduleA.mSelfMix[i] = 0.f;
        moduleA.mCrossMix[i] = 0.f;

        moduleB.mSelfMix[i] = 0.f;
        moduleB.mCrossMix[i] = 0.f;

        mSampleA[i] = 0.f;
        mSampleB[i] = 0.f;

        // Set Seed of each Oscillator
        moduleA.mOsc[i].setSeed(i + 1);
        moduleB.mOsc[i].setSeed(i + 1 + 111);
    }

    // Shaper Controls
    moduleA.mDrive = 0.18f;
    moduleA.mFold = 0.5f;
    moduleA.mAsym = 0.f;

    moduleB.mDrive = 0.18f;
    moduleB.mFold = 0.5f;
    moduleB.mAsym = 0.f;

    // Osc Controls
    moduleA.mPhase = 0.f;
    moduleA.mPitchOffset = 60.f;
    moduleA.mKeyTracking = 1.f;

    moduleA.mPmSelf = 0.f;
    moduleA.mPmCross = 0.f;
    moduleA.mPmSelfShaper = 0.f;
    moduleA.mPmCrossShaper = 0.f;

    moduleB.mPhase = 0.f;
    moduleB.mPitchOffset = 60.f;
    moduleB.mKeyTracking = 1.f;

    moduleB.mPmSelf = 0.f;
    moduleB.mPmCross = 0.f;
    moduleB.mPmSelfShaper = 0.f;
    moduleB.mPmCrossShaper = 0.f;

    // Shaper Controls
    /*drive, asym and fold are passed directly to the shaper*/
    moduleA.mShaperMixAmount = 0.f;
    moduleA.mRingMod = 0.f;

    moduleB.mShaperMixAmount = 0.f;
    moduleB.mRingMod = 0.f;
}



/******************************************************************************/
/** @brief    main function which calculates the modulated phase depending
 *            on mix amounts (self modulation, cross modulation, feedback modulation),
 *            calculates the samples of the 2 oscillators and 2 shapers
*******************************************************************************/

void OneSoundgenerator::generateSound()
{
    //******************************** Temp. Variables **********************************//

    float tempSample;
    float shaperSampleA, shaperSampleB;


    //*********************************** Oscillator ************************************//

    // calculate for all voices
    for(uint32_t voiceNumber = 0; voiceNumber < NUM_VOICES; voiceNumber++)
    {
        float modedPhaseA = (moduleA.mSelfMix[voiceNumber] * moduleA.mPmSelf) + (moduleB.mCrossMix[voiceNumber] * moduleA.mPmCross);
        float modedPhaseB = (moduleB.mSelfMix[voiceNumber] * moduleB.mPmSelf) + (moduleA.mCrossMix[voiceNumber] * moduleB.mPmCross);

        moduleA.mOsc[voiceNumber].setModPhase(modedPhaseA);
        moduleB.mOsc[voiceNumber].setModPhase(modedPhaseB);

        float oscSampleA = moduleA.mOsc[voiceNumber].applyOscillator();
        float oscSampleB = moduleB.mOsc[voiceNumber].applyOscillator();

//        float shaperSampleA = moduleA.mShaper.applyShaper(oscSampleA);
//        float shaperSampleB = moduleB.mShaper.applyShaper(oscSampleB);

        //************************************ Shaper A *************************************//

        shaperSampleA = oscSampleA * moduleA.mDrive;
        tempSample = shaperSampleA;

        shaperSampleA = NlToolbox::Math::sinP3(shaperSampleA);
        shaperSampleA = NlToolbox::Others::threeRanges(shaperSampleA, tempSample, moduleA.mFold);

        tempSample = shaperSampleA * shaperSampleA + (-0.5f);

        shaperSampleA = NlToolbox::Others::parAsym(shaperSampleA, tempSample, moduleA.mAsym);


        //************************************ Shaper B *************************************//

        shaperSampleB = oscSampleB * moduleB.mDrive;
        tempSample = shaperSampleB;

        shaperSampleB = NlToolbox::Math::sinP3(shaperSampleB);
        shaperSampleB = NlToolbox::Others::threeRanges(shaperSampleB, tempSample, moduleB.mFold);

        tempSample = shaperSampleB * shaperSampleB + (-0.5f);

        shaperSampleB = NlToolbox::Others::parAsym(shaperSampleB, tempSample, moduleB.mAsym);


        //*********************************** Crossfades ************************************//

        moduleA.mSelfMix[voiceNumber] = NlToolbox::Crossfades::bipolarCrossFade(oscSampleA, shaperSampleA, moduleA.mPmSelfShaper);
        moduleA.mCrossMix[voiceNumber] = NlToolbox::Crossfades::bipolarCrossFade(oscSampleA, shaperSampleA, moduleB.mPmCrossShaper);

        moduleB.mSelfMix[voiceNumber] = NlToolbox::Crossfades::bipolarCrossFade(oscSampleB, shaperSampleB, moduleB.mPmSelfShaper);
        moduleB.mCrossMix[voiceNumber] = NlToolbox::Crossfades::bipolarCrossFade(oscSampleB, shaperSampleB, moduleA.mPmCrossShaper);

        mSampleA[voiceNumber] = NlToolbox::Crossfades::bipolarCrossFade(oscSampleA, shaperSampleA, moduleA.mShaperMixAmount);
        mSampleB[voiceNumber] = NlToolbox::Crossfades::bipolarCrossFade(oscSampleB, shaperSampleB, moduleB.mShaperMixAmount);

        // Ring Modulation
        float crossSample = mSampleA[voiceNumber] * mSampleB[voiceNumber];

        mSampleA[voiceNumber] = NlToolbox::Crossfades::bipolarCrossFade(mSampleA[voiceNumber], crossSample, moduleA.mRingMod);
        mSampleB[voiceNumber] = NlToolbox::Crossfades::bipolarCrossFade(mSampleB[voiceNumber], crossSample, moduleB.mRingMod);
    }
}



/******************************************************************************/
/** @brief    pitch set function, which calls the function for calculating the
 *            individual oscialltor frequencies. Dependant on previously
 *            Key Tracking Amount and Pitch Offset
 *  @param    incoming pitch of the pressed key
 *  @param    voice number for the correct oscillator initialization
*******************************************************************************/

void OneSoundgenerator::setPitch(float _pitch, uint32_t _voiceNumber)
{
    mPitch[_voiceNumber] = _pitch - 60.f;

    moduleA.mOsc[_voiceNumber].setOscFreq(calcOscFrequency(mPitch[_voiceNumber], moduleA.mKeyTracking, moduleA.mPitchOffset));
    moduleA.mOsc[_voiceNumber].calcInc();

    moduleB.mOsc[_voiceNumber].setOscFreq(calcOscFrequency(mPitch[_voiceNumber], moduleB.mKeyTracking, moduleB.mPitchOffset));
    moduleB.mOsc[_voiceNumber].calcInc();
}



/******************************************************************************/
/** @brief    phase reset for each oscillator, which happens with every note on
 *            event
*******************************************************************************/

void OneSoundgenerator::resetPhase(uint32_t _voiceNumber)
{
        moduleA.mOsc[_voiceNumber].resetPhase(moduleA.mPhase);
        moduleB.mOsc[_voiceNumber].resetPhase(moduleB.mPhase);
}



/*****************************************************************************/
/** @brief    interface method which converts and scales the incoming midi
 *            values and passes these to the respective methods or parameters
 *  @param    midi instrument ID -> enum  InstrID (Soundgenerator.h)
 *  @param    midi control ID -> enum CtrlID (Soundgenerator.h)
 *  @param    midi control value [0 ... 127]
******************************************************************************/

void OneSoundgenerator::setGenParams(unsigned char _instrID, unsigned char _ctrlID, float _ctrlVal)
{
    switch (_instrID)
    {
        case InstrID::MODULE_A:

        switch (_ctrlID)
        {
            /*********************** Oscillator Controls ***********************/

            case CtrlID::OFFSETPITCH:
                _ctrlVal -= 20;

                printf("A: Pitch Offset: %f\n", _ctrlVal);

                moduleA.mPitchOffset = _ctrlVal;

                for(uint32_t voiceNumber = 0; voiceNumber < NUM_VOICES; voiceNumber++)
                {
                    moduleA.mOsc[voiceNumber].setOscFreq(calcOscFrequency(mPitch[voiceNumber], moduleA.mKeyTracking, moduleA.mPitchOffset));
                    moduleA.mOsc[voiceNumber].calcInc();
                }
                break;

            case CtrlID::KEYTRACKING:
                if (_ctrlVal > 105)
                {
                    _ctrlVal = 105;
                }

                printf("A: Key Tracking: %f\n", _ctrlVal);

                moduleA.mKeyTracking = _ctrlVal / 100.f;

                for(uint32_t voiceNumber = 0; voiceNumber < NUM_VOICES; voiceNumber++)
                {
                    moduleA.mOsc[voiceNumber].setOscFreq(calcOscFrequency(mPitch[voiceNumber], moduleA.mKeyTracking, moduleA.mPitchOffset));
                    moduleA.mOsc[voiceNumber].calcInc();
                }
                break;

            case CtrlID::PHASE:
                _ctrlVal = (_ctrlVal / 126.f) - 0.5f;

                if (_ctrlVal > 0.5f)
                {
                    _ctrlVal = 0.5f;
                }

                printf("A: Phase: %f\n", _ctrlVal);

                moduleA.mPhase = _ctrlVal;
                break;

            case CtrlID::FLUCT:
                _ctrlVal /= 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("A: Fluct: %f\n", _ctrlVal);

                _ctrlVal = (_ctrlVal * _ctrlVal * 0.95f);

                for(uint32_t voiceNumber = 0; voiceNumber < NUM_VOICES; voiceNumber++)
                {
                    moduleA.mOsc[voiceNumber].setFluctuation(_ctrlVal);
                }
                break;

            case CtrlID::CHIRPFREQ:
                _ctrlVal = (_ctrlVal + 160.f) / 2.f;

                if (_ctrlVal > 140.f)
                {
                    _ctrlVal = 140.f;
                }

                printf("A: Chirp Freq: %f\n", _ctrlVal);

                _ctrlVal = NlToolbox::Conversion::pitch2freq(_ctrlVal - 1.5f);

                for(uint32_t voiceNumber = 0; voiceNumber < NUM_VOICES; voiceNumber++)
                {
                    moduleA.mOsc[voiceNumber].setChirpFreq(_ctrlVal);
                }
                break;

            case CtrlID::PMSELF:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("A: PM Self: %f\n", _ctrlVal);

                moduleA.mPmSelf = fabs(_ctrlVal) * _ctrlVal * 0.5f;
                break;

            case CtrlID::PMCROSS:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("A: PM B: %f\n", _ctrlVal);

                moduleA.mPmCross = fabs(_ctrlVal) * _ctrlVal;
                break;

            case CtrlID::PMSELFSHAPER:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("A: Shaper PM Self: %f\n", _ctrlVal);

                moduleA.mPmSelfShaper = _ctrlVal;
                break;

            case CtrlID::PMCROSSSHAPER:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("A: Shaper PM B: %f\n", _ctrlVal);

                moduleA.mPmCrossShaper = _ctrlVal;
                break;



            /*********************** Shaper Controls ***********************/

            case CtrlID::DRIVE:
                _ctrlVal = _ctrlVal / 2.f;

                if (_ctrlVal > 43.f)
                {
                    _ctrlVal = 43.f;
                }

                _ctrlVal = pow(10.f, (_ctrlVal / 20.f)) * 0.18f - 0.18f;

                if (_ctrlVal > 25.f)
                {
                    _ctrlVal = 25.f;
                }

                printf("A: Drive: %f\n", _ctrlVal);

//                moduleA.mShaper.setDrive(_ctrlVal + 0.18f);
                moduleA.mDrive = _ctrlVal + 0.18f;
                break;

            case CtrlID::MAINMIX:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("A: Main Mix: %f\n", _ctrlVal);

                moduleA.mShaperMixAmount = _ctrlVal;
                break;

            case CtrlID::RING:
                _ctrlVal /= 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("A: Ring Modulation: %f\n", _ctrlVal);

                moduleA.mRingMod = _ctrlVal;
                break;

            case CtrlID::FOLD:
                _ctrlVal /= 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("A: Fold: %f\n", _ctrlVal);

//                moduleA.mShaper.setFold(_ctrlVal);
                moduleA.mFold = _ctrlVal;
                break;

            case CtrlID::ASYM:
                _ctrlVal /= 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("A: Asym: %f\n", _ctrlVal);

//                moduleA.mShaper.setAsym(_ctrlVal);
                moduleA.mAsym = _ctrlVal;
                break;

        }
        break;

        case InstrID::MODULE_B:

        switch (_ctrlID)
        {
            /*********************** Oscillator Controls ***********************/

            case CtrlID::OFFSETPITCH:
                _ctrlVal -= 20;

                printf("B: Pitch Offset: %f\n", _ctrlVal);

                moduleB.mPitchOffset = _ctrlVal;

                for(uint32_t voiceNumber = 0; voiceNumber < NUM_VOICES; voiceNumber++)
                {
                    moduleB.mOsc[voiceNumber].setOscFreq(calcOscFrequency(mPitch[voiceNumber], moduleB.mKeyTracking, moduleB.mPitchOffset));
                    moduleB.mOsc[voiceNumber].calcInc();
                }
                break;

            case CtrlID::KEYTRACKING:
                if (_ctrlVal > 105)
                {
                    _ctrlVal = 105;
                }

                printf("B: Key Tracking: %f\n", _ctrlVal);

                moduleB.mKeyTracking = _ctrlVal / 100.f;

                for(uint32_t voiceNumber = 0; voiceNumber < NUM_VOICES; voiceNumber++)
                {
                    moduleB.mOsc[voiceNumber].setOscFreq(calcOscFrequency(mPitch[voiceNumber], moduleB.mKeyTracking, moduleB.mPitchOffset));
                    moduleB.mOsc[voiceNumber].calcInc();
                }
                break;

            case CtrlID::PHASE:
                _ctrlVal = (_ctrlVal / 126.f) - 0.5f;

                if (_ctrlVal > 0.5f)
                {
                    _ctrlVal = 0.5f;
                }

                printf("B: Phase: %f\n", _ctrlVal);

                moduleB.mPhase = _ctrlVal;
                break;

            case CtrlID::FLUCT:
                _ctrlVal /= 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("B: Fluct: %f\n", _ctrlVal);

                _ctrlVal = (_ctrlVal * _ctrlVal * 0.95f);

                for(uint32_t voiceNumber = 0; voiceNumber < NUM_VOICES; voiceNumber++)
                {
                    moduleB.mOsc[voiceNumber].setFluctuation(_ctrlVal);
                }
                break;

            case CtrlID::CHIRPFREQ:
                _ctrlVal = (_ctrlVal + 160.f) / 2.f;

                if (_ctrlVal > 140.f)
                {
                    _ctrlVal = 140.f;
                }

                printf("B: Chirp Freq: %f\n", _ctrlVal);

                _ctrlVal = NlToolbox::Conversion::pitch2freq(_ctrlVal - 1.5f);

                for(uint32_t voiceNumber = 0; voiceNumber < NUM_VOICES; voiceNumber++)
                {
                    moduleB.mOsc[voiceNumber].setChirpFreq(_ctrlVal);
                }
                break;

            case CtrlID::PMSELF:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("B: PM Self: %f\n", _ctrlVal);

                moduleB.mPmSelf = fabs(_ctrlVal) * _ctrlVal * 0.5f;
                break;

            case CtrlID::PMCROSS:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("B: PM A: %f\n", _ctrlVal);

                moduleB.mPmCross = fabs(_ctrlVal) * _ctrlVal;
                break;

            case CtrlID::PMSELFSHAPER:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("B: Shaper PM Self: %f\n", _ctrlVal);

                moduleB.mPmSelfShaper = _ctrlVal;
                break;

            case CtrlID::PMCROSSSHAPER:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("B: Shaper PM A: %f\n", _ctrlVal);

                moduleB.mPmCrossShaper = _ctrlVal;
                break;



            /*********************** Shaper Controls ***********************/

            case CtrlID::DRIVE:
                _ctrlVal = _ctrlVal / 2.f;

                if (_ctrlVal > 43.f)
                {
                    _ctrlVal = 43.f;
                }

                _ctrlVal = pow(10.f, (_ctrlVal / 20.f)) * 0.18f - 0.18f;

                if (_ctrlVal > 25.f)
                {
                    _ctrlVal = 25.f;
                }

                printf("B: Drive: %f\n", _ctrlVal);

//                moduleB.mShaper.setDrive(_ctrlVal + 0.18f);
                moduleB.mDrive = _ctrlVal + 0.18f;
                break;

            case CtrlID::MAINMIX:
                _ctrlVal = (_ctrlVal / 63.f) - 1.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("B: Main Mix: %f\n", _ctrlVal);

                moduleB.mShaperMixAmount = _ctrlVal;
                break;

            case CtrlID::RING:
                _ctrlVal /= 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("B: Ring Modulation: %f\n", _ctrlVal);

                moduleB.mRingMod = _ctrlVal;
                break;

            case CtrlID::FOLD:
                _ctrlVal /= 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("B: Fold: %f\n", _ctrlVal);

//                moduleB.mShaper.setFold(_ctrlVal);
                moduleB.mFold = _ctrlVal;
                break;

            case CtrlID::ASYM:
                _ctrlVal /= 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("B: Asym: %f\n", _ctrlVal);

//                moduleB.mShaper.setAsym(_ctrlVal);
                moduleB.mAsym = _ctrlVal;
                break;

        }
        break;
    }
}

/******************************************************************************/
/** @brief    frequency calculation depending on the incoming pitch,
 *            key tracking amount and pitch offset
 *  @param    incoming pitch - 60ct
 *  @param    key tracking amount
 *  @param    pitch offset
 *  @return   frequency in Hz
*******************************************************************************/

inline float OneSoundgenerator::calcOscFrequency(float _pitch, float _keyTracking, float _pitchOffset)
{
    _pitch = _pitch * _keyTracking + _pitchOffset;

    if (_pitch < 0.f)
    {
        if (_pitch > -20.f)
        {
            _pitch = ((300.f/13.f) * _pitch) / ((280.f/13.f) + _pitch);
        }

        else if (_pitch < -20.f)
        {
            _pitch = -300.f;
        }
    }

    return NlToolbox::Conversion::pitch2freq(_pitch);
}
