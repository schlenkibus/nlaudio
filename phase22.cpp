/******************************************************************************/
/** @file		cabinet.cpp
    @date		2016-07-20
    @version	0.2
    @author		Anton Schmied[2016-07-20]
    @brief		Phase22 Class member and method definitions
*******************************************************************************/

#include "phase22.h"

/******************************************************************************/
/** Phase22 Default Constructor
 * @brief    initialization of the modules local variabels with default values
 *           SampleRate:            48 kHz
 *           Phase:                 0.f
 *           Gain:                  0.5f
 *           Pitch Offset:          60.f ct
 *           Key Tracking:          1.f
 *           Main Mix Amount:       0.f
 *           Drive:                 0.18f
 *           Fold:                  0.5
 *           Asym:                  0.f
*******************************************************************************/

Phase22::Phase22()                          // Default Constructor
    : mSampleRate(48000.f)
{
    moduleA.mGain = 0.5f;
    moduleA.mMainMixAmount = 0.f;

    moduleA.mOsc = Oscillator();

    moduleA.mPhase = 0.f;
    moduleA.mPitchOffset = 60.f;
    moduleA.mKeyTracking = 1.f;
    moduleA.mDrive = 0.18f;
    moduleA.mFold = 0.5f;
    moduleA.mAsym = 0.f;

    moduleA.mPmSelf = 0.f;
    moduleA.mPmCross = 0.f;
    moduleA.mPmSelfAmount = 0.f;
    moduleA.mPmCrossAmount = 0.f;
    moduleA.mSelfMix = 0.f;
    moduleA.mCrossMix = 0.f;

    moduleB = moduleA;
}



/******************************************************************************/
/** Phase22 Parameterized Constructor
 * @brief    initialization of the modules local variabels with custom
 *           parameters
*******************************************************************************/

Phase22::Phase22(int _sampleRate,               // Parameterized Constructor
                 float _phase,
                 float _gain,
                 float _pitchOffset,
                 float _keyTracking,
                 float _mainMixAmoount,
                 float _drive,
                 float _fold,
                 float _asym)
    : mSampleRate(static_cast<float>(_sampleRate))
{
    moduleA.mGain = _gain;
    moduleA.mMainMixAmount = _mainMixAmoount;

    moduleA.mOsc = Oscillator();

    moduleA.mPhase = _phase;
    moduleA.mPitchOffset = _pitchOffset;
    moduleA.mKeyTracking = _keyTracking;
    moduleA.mDrive = _drive;
    moduleA.mFold = _fold;
    moduleA.mAsym = _asym;

    moduleA.mPmSelf = 0.f;
    moduleA.mPmCross = 0.f;
    moduleA.mPmSelfAmount = 0.f;
    moduleA.mPmCrossAmount = 0.f;
    moduleA.mSelfMix = 0.f;
    moduleA.mCrossMix = 0.f;

    moduleB = moduleA;
}



/******************************************************************************/
/** @brief    pitch set function, which calls the function for calculating the
 *            individual oscialltor frequencies. Dependant on previously
 *            Key Tracking Amount and Pitch Offset
 *  @param    incoming pitch
*******************************************************************************/

void Phase22::setPitch(float _pitch)
{
    mPitch = _pitch - 60.f;
    moduleA.mFrequency = calcOscFrequency(mPitch, moduleA.mKeyTracking, moduleA.mPitchOffset);
    moduleB.mFrequency = calcOscFrequency(mPitch, moduleB.mKeyTracking, moduleB.mPitchOffset);
}



/******************************************************************************/
/** @brief    depending on the voice number, the seed in the oscillators is set
 *            for fluctuation/ randomization purposes
 *  @param    voice number 0 - 11
*******************************************************************************/

void Phase22::setVoiceNumber(unsigned int _voiceNumber)
{
    moduleA.mOsc.setSeed(_voiceNumber + 1);
    moduleB.mOsc.setSeed(_voiceNumber + 1 + 111);
}



/*****************************************************************************/
/** @brief    interface method which converts and scales the incoming midi
 *            values and passes these to the respective methods or parameters
 *  @param    midi instrument ID -> enum  InstrID (phase22.h)
 *  @param    midi control ID -> enum CtrlID (phase22.h)
 *  @param    midi control value [0 ... 127]
******************************************************************************/

void Phase22::setOscParams(unsigned char _instrID, unsigned char _ctrlID, float _ctrlVal)
{
    switch (_instrID)
    {
        case InstrID::MODULE_A:

        switch (_ctrlID)
        {
            case CtrlID::OFFSETPITCH:
            _ctrlVal -= 20;

            printf("A: Pitch Offset: %f\n", _ctrlVal);

            moduleA.mPitchOffset = _ctrlVal;
            moduleA.mFrequency = calcOscFrequency(mPitch, moduleA.mKeyTracking, moduleA.mPitchOffset);
            break;

            case CtrlID::KEYTRACKING:
            if (_ctrlVal > 105)
            {
                _ctrlVal = 105;
            }

            printf("A: Key Tracking: %f\n", _ctrlVal);

            moduleA.mKeyTracking = _ctrlVal / 100.f;
            moduleA.mFrequency = calcOscFrequency(mPitch, moduleA.mKeyTracking, moduleA.mPitchOffset);
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

            case CtrlID::MAINMIX:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("A: Main Mix: %f\n", _ctrlVal);

            moduleA.mMainMixAmount = _ctrlVal;
            break;

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

            moduleA.mDrive = _ctrlVal + 0.18f;
            break;

            case CtrlID::FOLD:
            _ctrlVal /= 126.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("A: Fold: %f\n", _ctrlVal);

            moduleA.mFold = _ctrlVal;
            break;

            case CtrlID::ASYM:
            _ctrlVal /= 126.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("A: Asym: %f\n", _ctrlVal);

            moduleA.mAsym= _ctrlVal;
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

            case CtrlID::PMSELFAMNT:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("A: Shaper PM Self: %f\n", _ctrlVal);

            moduleA.mPmSelfAmount = _ctrlVal;
            break;

            case CtrlID::PMCROSSAMNT:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("A: Shaper PM B: %f\n", _ctrlVal);

            moduleA.mPmCrossAmount = _ctrlVal;
            break;

            case CtrlID::CHIRPFREQ:
            _ctrlVal = (_ctrlVal + 160.f) / 2.f;

            if (_ctrlVal > 140.f)
            {
                _ctrlVal = 140.f;
            }

            printf("A: Chirp Freq: %f\n", _ctrlVal);

            _ctrlVal = NlToolbox::Conversion::pitch2freq(_ctrlVal - 1.5f);
//            _ctrlVal = pow(2.f, ((_ctrlVal - 1.5f) - 69.f) / 12.f) * 440.f;

            moduleA.mOsc.setupChirpFilter(_ctrlVal);
            break;

            case CtrlID::GAIN:
            _ctrlVal = (_ctrlVal / 126.f);

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("A: Gain: %f\n", _ctrlVal);

            moduleA.mGain = _ctrlVal;
            break;
        }
        break;

        case InstrID::MODULE_B:

        switch (_ctrlID)
        {
            case CtrlID::OFFSETPITCH:
            _ctrlVal -= 20;

            printf("B: Pitch Offset: %f\n", _ctrlVal);

            moduleB.mPitchOffset = _ctrlVal;
            moduleB.mFrequency = calcOscFrequency(mPitch, moduleB.mKeyTracking, moduleB.mPitchOffset);
            break;

            case CtrlID::KEYTRACKING:
            if (_ctrlVal > 105)
            {
                _ctrlVal = 105;
            }

            printf("B: Key Tracking: %f\n", _ctrlVal);

            moduleB.mKeyTracking = _ctrlVal / 100.f;
            moduleB.mFrequency = calcOscFrequency(mPitch, moduleB.mKeyTracking, moduleB.mPitchOffset);
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

            case CtrlID::MAINMIX:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("B: Main Mix: %f\n", _ctrlVal);

            moduleB.mMainMixAmount = _ctrlVal;
            break;

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

            moduleB.mDrive = _ctrlVal + 0.18f;
            break;

            case CtrlID::FOLD:
            _ctrlVal /= 126.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("B: Fold: %f\n", _ctrlVal);

            moduleB.mFold = _ctrlVal;
            break;

            case CtrlID::ASYM:
            _ctrlVal /= 126.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("B: Asym: %f\n", _ctrlVal);

            moduleB.mAsym = _ctrlVal;
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

            case CtrlID::PMSELFAMNT:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("B: Shaper PM Self: %f\n", _ctrlVal);

            moduleB.mPmSelfAmount = _ctrlVal;
            break;

            case CtrlID::PMCROSSAMNT:
            _ctrlVal = (_ctrlVal / 63.f) - 1.f;

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("B: Shaper PM A: %f\n", _ctrlVal);

            moduleB.mPmCrossAmount = _ctrlVal;
            break;

            case CtrlID::CHIRPFREQ:
            _ctrlVal = (_ctrlVal + 160.f) / 2.f;

            if (_ctrlVal > 140.f)
            {
                _ctrlVal = 140.f;
            }

            printf("B: Chirp Freq: %f\n", _ctrlVal);

            _ctrlVal = NlToolbox::Conversion::pitch2freq(_ctrlVal - 1.5f);
//            _ctrlVal = pow(2.f, ((_ctrlVal - 1.5f) - 69.f) / 12.f) * 440.f;

            moduleB.mOsc.setupChirpFilter(_ctrlVal);
            break;

            case CtrlID::GAIN:
            _ctrlVal = (_ctrlVal / 126.f);

            if (_ctrlVal > 1.f)
            {
                _ctrlVal = 1.f;
            }

            printf("B: Gain: %f\n", _ctrlVal);

            moduleB.mGain = _ctrlVal;
            break;
        }
        break;
    }
}



/******************************************************************************/
/** @brief    phase reset for each oscillator, which happens with every note on
 *            event
*******************************************************************************/

void Phase22::resetPhase()
{
    moduleA.mOsc.resetPhase(moduleA.mPhase);
    moduleB.mOsc.resetPhase(moduleB.mPhase);
}



/******************************************************************************/
/** @brief    main function which calculates the modulated phase depending
 *            on mix amounts (self modulation, cross modulation, feedback modulation),
 *            calculates the samples of the 2 oscillators and 2 shapers
 *  @return   a mix of 2 samples deriving from the oscillator and shaper processes
*******************************************************************************/

float Phase22::makeNoise()
{
    float sampleA;          // resulating samples after modulation process
    float sampleB;
    float outputSample;

    float oscSampleA;       // Oscillator samples
    float oscSampleB;

    float shaperSampleA;    // Shaper samples
    float shaperSampleB;

    float modedPhaseA = (moduleA.mSelfMix * moduleA.mPmSelf) + (moduleA.mCrossMix * moduleA.mPmCross);
    float modedPhaseB = (moduleB.mSelfMix * moduleB.mPmSelf) + (moduleB.mCrossMix * moduleB.mPmCross);

    moduleA.mOsc.setModulationRadians(modedPhaseA);
    moduleB.mOsc.setModulationRadians(modedPhaseB);

    moduleA.mOsc.calcInc(moduleA.mFrequency);
    moduleB.mOsc.calcInc(moduleB.mFrequency);

    oscSampleA = moduleA.mOsc.applyOscillator(moduleA.mFrequency);
    oscSampleB = moduleB.mOsc.applyOscillator(moduleB.mFrequency);

    shaperSampleA = applyShaper(oscSampleA, moduleA.mDrive, moduleA.mFold, moduleA.mAsym);
    shaperSampleB = applyShaper(oscSampleB, moduleB.mDrive, moduleB.mFold, moduleB.mAsym);

    ///*das muss noch mal überprüft werden!!! */
    moduleA.mSelfMix = NlToolbox::Crossfades::bipolarCrossFade(oscSampleA, shaperSampleA, moduleA.mPmSelfAmount);
    moduleA.mCrossMix = NlToolbox::Crossfades::bipolarCrossFade(oscSampleB, shaperSampleB, moduleA.mPmCrossAmount);

    moduleB.mSelfMix = NlToolbox::Crossfades::bipolarCrossFade(oscSampleB, shaperSampleB, moduleB.mPmSelfAmount);
    moduleB.mCrossMix = NlToolbox::Crossfades::bipolarCrossFade(oscSampleA, shaperSampleA, moduleB.mPmCrossAmount);

    sampleA = NlToolbox::Crossfades::bipolarCrossFade(oscSampleA, shaperSampleA, moduleA.mMainMixAmount);
    sampleB = NlToolbox::Crossfades::bipolarCrossFade(oscSampleB, shaperSampleB, moduleB.mMainMixAmount);

    outputSample = NlToolbox::Crossfades::crossFade(sampleA, sampleB, moduleA.mGain, moduleB.mGain);

    return outputSample;
}



/******************************************************************************/
/** @brief    shaper function
 *  @param    raw sample
 *  @param    drive amount
 *  @param    fold amount
 *  @param    asymmetry amount
 *  @return   processed sample
*******************************************************************************/

float Phase22::applyShaper(float _currSample, float _drive, float _fold, float _asym)
{
    _currSample *= _drive;

    float ctrl = _currSample;

    _currSample = NlToolbox::Math::sinP3(_currSample);
    _currSample = NlToolbox::Others::threeRanges(_currSample, ctrl, _fold);

    float currSample_square = _currSample * _currSample + (-0.5f);

    _currSample = NlToolbox::Others::parAsym(_currSample, currSample_square, _asym);

    return _currSample;
}



/******************************************************************************/
/** @brief    frequency calculation depending on the incoming pitch,
 *            key tracking amount and pitch offset
 *  @param    incoming pitch - 60ct
 *  @param    key tracking amount
 *  @param    pitch offset
 *  @return   frequency in Hz
*******************************************************************************/

inline float Phase22::calcOscFrequency(float _pitch, float _keyTracking, float _pitchOffset)
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

    float frequency = pow(2.f, (_pitch - 69) / 12.f) * 440.f;

    return frequency;
}
