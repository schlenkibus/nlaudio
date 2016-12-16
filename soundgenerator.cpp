/******************************************************************************/
/** @file		soundgenerator.cpp
    @date		2016-07-20
    @version	0.2
    @author		Anton Schmied[2016-07-20]
    @brief		Soundgenerator Class member and method definitions
*******************************************************************************/

#include "soundgenerator.h"

/******************************************************************************/
/** Soundgenerator Default Constructor
 * @brief    initialization of the modules local variabels with default values
 *           SampleRate:            48 kHz
 *           Phase:                 0.f
 *           Pitch Offset:          60.f ct
 *           Key Tracking:          1.f
 *           Main Mix Amount:       0.f
*******************************************************************************/

Soundgenerator::Soundgenerator()                            // Default Constructor
    : mSampleRate(48000.f)
{
    moduleA.mOsc = Oscillator();
    moduleA.mShaper = Shaper();

    moduleA.mShaperMixAmount = 0.f;

    moduleA.mPhase = 0.f;
    moduleA.mPitchOffset = 60.f;
    moduleA.mKeyTracking = 1.f;

    moduleA.mPmSelf = 0.f;
    moduleA.mPmCross = 0.f;
    moduleA.mPmSelfShaper = 0.f;
    moduleA.mPmCrossShaper = 0.f;

    moduleA.mSelfMix = 0.f;
    moduleA.mCrossMix = 0.f;

    moduleA.mRingMod = 0.f;

    moduleB = moduleA;
}



/******************************************************************************/
/** Soundgenerator Parameterized Constructor
 * @brief    initialization of the modules local variabels with custom
 *           parameters
*******************************************************************************/

Soundgenerator::Soundgenerator(uint32_t _sampleRate,        // Default Constructor
                 float _phase,
                 float _pitchOffset,
                 float _keyTracking,
                 float _mainMixAmount)
    : mSampleRate(static_cast<float>(_sampleRate))
{
    moduleA.mShaperMixAmount = _mainMixAmount;

    moduleA.mOsc = Oscillator();
    moduleA.mShaper = Shaper();

    moduleA.mPhase = _phase;
    moduleA.mPitchOffset = _pitchOffset;
    moduleA.mKeyTracking = _keyTracking;

    moduleA.mPmSelf = 0.f;
    moduleA.mPmCross = 0.f;
    moduleA.mPmSelfShaper = 0.f;
    moduleA.mPmCrossShaper = 0.f;

    moduleA.mSelfMix = 0.f;
    moduleA.mCrossMix = 0.f;

    moduleA.mRingMod = 0.f;

    moduleB = moduleA;
}



/******************************************************************************/
/** @brief    main function which calculates the modulated phase depending
 *            on mix amounts (self modulation, cross modulation, feedback modulation),
 *            calculates the samples of the 2 oscillators and 2 shapers
*******************************************************************************/

void Soundgenerator::generateSound()
{
    float modedPhaseA = (moduleA.mSelfMix * moduleA.mPmSelf) + (moduleB.mCrossMix * moduleA.mPmCross);
    float modedPhaseB = (moduleB.mSelfMix * moduleB.mPmSelf) + (moduleA.mCrossMix * moduleB.mPmCross);

    moduleA.mOsc.setModPhase(modedPhaseA);
    moduleB.mOsc.setModPhase(modedPhaseB);

    float oscSampleA = moduleA.mOsc.applyOscillator();
    float oscSampleB = moduleB.mOsc.applyOscillator();

    float shaperSampleA = moduleA.mShaper.applyShaper(oscSampleA);
    float shaperSampleB = moduleB.mShaper.applyShaper(oscSampleB);

    moduleA.mSelfMix = NlToolbox::Crossfades::bipolarCrossFade(oscSampleA, shaperSampleA, moduleA.mPmSelfShaper);
    moduleA.mCrossMix = NlToolbox::Crossfades::bipolarCrossFade(oscSampleA, shaperSampleA, moduleB.mPmCrossShaper);

    moduleB.mSelfMix = NlToolbox::Crossfades::bipolarCrossFade(oscSampleB, shaperSampleB, moduleB.mPmSelfShaper);
    moduleB.mCrossMix = NlToolbox::Crossfades::bipolarCrossFade(oscSampleB, shaperSampleB, moduleA.mPmCrossShaper);

    mSampleA = NlToolbox::Crossfades::bipolarCrossFade(oscSampleA, shaperSampleA, moduleA.mShaperMixAmount);
    mSampleB = NlToolbox::Crossfades::bipolarCrossFade(oscSampleB, shaperSampleB, moduleB.mShaperMixAmount);

    // Ring Modulation
    float crossSample = mSampleA * mSampleB;

    mSampleA = NlToolbox::Crossfades::bipolarCrossFade(mSampleA, crossSample, moduleA.mRingMod);
    mSampleB = NlToolbox::Crossfades::bipolarCrossFade(mSampleB, crossSample, moduleB.mRingMod);
}



/******************************************************************************/
/** @brief    pitch set function, which calls the function for calculating the
 *            individual oscialltor frequencies. Dependant on previously
 *            Key Tracking Amount and Pitch Offset
 *  @param    incoming pitch
*******************************************************************************/

void Soundgenerator::setPitch(float _pitch)
{
    mPitch = _pitch - 60.f;

    moduleA.mOsc.setOscFreq(calcOscFrequency(mPitch, moduleA.mKeyTracking, moduleA.mPitchOffset));
    moduleA.mOsc.calcInc();

    moduleB.mOsc.setOscFreq(calcOscFrequency(mPitch, moduleB.mKeyTracking, moduleB.mPitchOffset));
    moduleB.mOsc.calcInc();
}



/******************************************************************************/
/** @brief    depending on the voice number, the seed in the oscillators is set
 *            for fluctuation/ randomization purposes
 *  @param    voice number 0 - 11
*******************************************************************************/

void Soundgenerator::setVoiceNumber(uint32_t _voiceNumber)
{
    moduleA.mOsc.setSeed(_voiceNumber + 1);
    moduleB.mOsc.setSeed(_voiceNumber + 1 + 111);
}



/******************************************************************************/
/** @brief    phase reset for each oscillator, which happens with every note on
 *            event
*******************************************************************************/

void Soundgenerator::resetPhase()
{
    moduleA.mOsc.resetPhase(moduleA.mPhase);
    moduleB.mOsc.resetPhase(moduleB.mPhase);
}



/*****************************************************************************/
/** @brief    interface method which converts and scales the incoming midi
 *            values and passes these to the respective methods or parameters
 *  @param    midi instrument ID -> enum  InstrID (Soundgenerator.h)
 *  @param    midi control ID -> enum CtrlID (Soundgenerator.h)
 *  @param    midi control value [0 ... 127]
******************************************************************************/

void Soundgenerator::setGenParams(unsigned char _instrID, unsigned char _ctrlID, float _ctrlVal)
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

                moduleA.mOsc.setOscFreq(calcOscFrequency(mPitch, moduleA.mKeyTracking, moduleA.mPitchOffset));
                moduleA.mOsc.calcInc();
                break;

            case CtrlID::KEYTRACKING:
                if (_ctrlVal > 105)
                {
                    _ctrlVal = 105;
                }

                printf("A: Key Tracking: %f\n", _ctrlVal);

                moduleA.mKeyTracking = _ctrlVal / 100.f;

                moduleA.mOsc.setOscFreq(calcOscFrequency(mPitch, moduleA.mKeyTracking, moduleA.mPitchOffset));
                moduleA.mOsc.calcInc();
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

                moduleA.mOsc.setFluctuation(_ctrlVal);
                break;

            case CtrlID::CHIRPFREQ:
                _ctrlVal = (_ctrlVal + 160.f) / 2.f;

                if (_ctrlVal > 140.f)
                {
                    _ctrlVal = 140.f;
                }

                printf("A: Chirp Freq: %f\n", _ctrlVal);

                _ctrlVal = NlToolbox::Conversion::pitch2freq(_ctrlVal - 1.5f);

                moduleA.mOsc.setChirpFreq(_ctrlVal);
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

                moduleA.mShaper.setDrive(_ctrlVal + 0.18f);
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

                moduleA.mShaper.setFold(_ctrlVal);
                break;

            case CtrlID::ASYM:
                _ctrlVal /= 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("A: Asym: %f\n", _ctrlVal);

                moduleA.mShaper.setAsym(_ctrlVal);
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

                moduleB.mOsc.setOscFreq(calcOscFrequency(mPitch, moduleB.mKeyTracking, moduleB.mPitchOffset));
                moduleB.mOsc.calcInc();
                break;

            case CtrlID::KEYTRACKING:
                if (_ctrlVal > 105)
                {
                    _ctrlVal = 105;
                }

                printf("B: Key Tracking: %f\n", _ctrlVal);

                moduleB.mKeyTracking = _ctrlVal / 100.f;

                moduleB.mOsc.setOscFreq(calcOscFrequency(mPitch, moduleB.mKeyTracking, moduleB.mPitchOffset));
                moduleB.mOsc.calcInc();
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

                moduleB.mOsc.setFluctuation(_ctrlVal);
                break;

            case CtrlID::CHIRPFREQ:
                _ctrlVal = (_ctrlVal + 160.f) / 2.f;

                if (_ctrlVal > 140.f)
                {
                    _ctrlVal = 140.f;
                }

                printf("B: Chirp Freq: %f\n", _ctrlVal);

                _ctrlVal = NlToolbox::Conversion::pitch2freq(_ctrlVal - 1.5f);

                moduleB.mOsc.setChirpFreq(_ctrlVal);
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

                moduleB.mShaper.setDrive(_ctrlVal + 0.18f);
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

                moduleB.mShaper.setFold(_ctrlVal);
                break;

            case CtrlID::ASYM:
                _ctrlVal /= 126.f;

                if (_ctrlVal > 1.f)
                {
                    _ctrlVal = 1.f;
                }

                printf("B: Asym: %f\n", _ctrlVal);

                moduleB.mShaper.setAsym(_ctrlVal);
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

inline float Soundgenerator::calcOscFrequency(float _pitch, float _keyTracking, float _pitchOffset)
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
