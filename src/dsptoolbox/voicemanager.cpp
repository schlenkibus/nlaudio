/******************************************************************************/
/** @file		voicemanager.cpp
    @date		2016-07-26
    @version	0.1
    @author		Anton Schmied[2016-07-26]
    @brief		..
*******************************************************************************/

#include "voicemanager.h"

/******************************************************************************/
/** Voice Manager Default Constructor
 * @brief    initialization of the modules local variabels with default values
*******************************************************************************/

VoiceManager::VoiceManager()
    : mainOut_L(0.f)
    , mainOut_R(0.f)
{
#ifdef MANYGENS
    for(int i = 0; i < NUM_VOICES; i++)
    {
        mSoundGenerator[i] = Soundgenerator();          // Soundgenerator for each Voice
        mCombFilter[i] = CombFilter();                  // Comb Filter for each voice
    }
#endif
#ifdef ONEGEN
    mSoundGenerator = OneSoundgenerator();
#endif

    mOutputMixer = Outputmixer();                            // Outputmixer

    mLeftCabinet = Cabinet();
    mRightCabinet = Cabinet();

    mEcho = Echo();

#ifdef MANYGENS
    for (uint32_t i = 0; i < NUM_VOICES; i++)
    {
        mSoundGenerator[i].setVoiceNumber(i);
    }
#endif

    vallocInit();
}



/******************************************************************************/
/** @brief    Parameter setter function
*******************************************************************************/

void VoiceManager::evalMidiEvents(unsigned char _instrID, unsigned char _ctrlID, float _ctrlVal)
{
    switch (_instrID)
    {
        case InstrID::SG_A_PARAM:
        case InstrID::SG_B_PARAM:
#ifdef MANYGENS
            for(uint32_t i = 0; i < NUM_VOICES; i++)
            {
                mSoundGenerator[i].setGenParams(_instrID, _ctrlID, _ctrlVal);
            }
#endif
#ifdef ONEGEN
            mSoundGenerator.setGenParams(_instrID, _ctrlID, _ctrlVal);
#endif
            break;

        case InstrID::COMBFILTER_PARAM:

            for(uint32_t i = 0; i < NUM_VOICES; i++)
            {
                mCombFilter[i].setCombFilterParams(_ctrlID, _ctrlVal);
            }
            break;

        case InstrID::CABINET_PARAM:

            mLeftCabinet.setCabinetParams(_ctrlID, _ctrlVal);
            mRightCabinet.setCabinetParams(_ctrlID, _ctrlVal);
            break;

        case InstrID::OUTPUTMIXER_PARAM:

            mOutputMixer.setOutputmixerParams(_ctrlID, _ctrlVal);
            break;

        case InstrID::ECHO_PARAM:

            mEcho.setEchoParams(_ctrlID, _ctrlVal);
            break;

        case InstrID::KEYUP_0:
        case InstrID::KEYUP_1:
        case InstrID::KEYUP_2:
        case InstrID::KEYUP_3:
        case InstrID::KEYUP_4:

            vallocProcess(_instrID, _ctrlID, _ctrlVal);
            break;

        case InstrID::KEYDOWN_0:
        case InstrID::KEYDOWN_1:
        case InstrID::KEYDOWN_2:
        case InstrID::KEYDOWN_3:
        case InstrID::KEYDOWN_4:

            vallocProcess(_instrID, _ctrlID, _ctrlVal);
            break;
    }
}



/******************************************************************************/
/** @brief    voice loop function which calculates all module output values
 *            for all voices
*******************************************************************************/

void VoiceManager::voiceLoop()
{
#ifdef MANYGENS
    // main dsp loop
    for(uint32_t i = 0; i < NUM_VOICES; i++)
    {
        mSoundGenerator[i].generateSound();         // hier ist für jede Stimme ein Generator!! -> soundgenerator.cpp/.h
        mCombFilter[i].applyCombFilter(mSoundGenerator[i].mSampleA, mSoundGenerator[i].mSampleB);
        mOutputMixer.applyMixer(mSoundGenerator[i].mSampleA, mSoundGenerator[i].mSampleB, mCombFilter[i].mCombFilterOut, 0.f);
    }
#endif
#ifdef ONEGEN
    mSoundGenerator.generateSound();            // hier ist ein Generator für alle Stimmen!! -> onesoundgenerator.cpp/.h

    for(uint32_t i = 0; i < NUM_VOICES; i++)
    {
        mOutputMixer.applyMixer(mSoundGenerator.mSampleA[i], mSoundGenerator.mSampleB[i], 0.f, 0.f);
    }
#endif

    ///hier kommen die effekte hin: Cab, Echo usw.!
    // Cabinet
    mRightCabinet.applyCab(mOutputMixer.mSample_R);
    mLeftCabinet.applyCab(mOutputMixer.mSample_L);

    // Echo
    mEcho.applyEcho(mLeftCabinet.mCabinetOut, mRightCabinet.mCabinetOut);

    mainOut_L = mEcho.mDelayOutL;
    mainOut_R = mEcho.mDelayOutR;
}


#if 1
/******************************************************************************/
/** @brief    voice allocation initialization
 *
*******************************************************************************/

void VoiceManager::vallocInit()
{
    for (uint32_t i = 0; i < NUM_VOICES; i++)
    {
        vVoiceState[i] = -1;
        vNextAssigned[i] = 0;
        vPreviousAssigned[i] = 0;
    }

    vNumAssigned = 0;
    vOldestAssigned = 0;
    vOldestReleased = 0;
    vYoungestAssigned = 0;
    vYoungestReleased = NUM_VOICES - 1;

    for (uint32_t i = 0; i < NUM_VOICES - 1; i++)
    {
        vNextReleased[i] = i + 1;
    }
}

/******************************************************************************/
/** @brief    voice allocation process
 *  @param    ID for keyUp or keyDown
*******************************************************************************/

void VoiceManager::vallocProcess(unsigned char _keyDirection, float _pitch, float _velocity)        // key down
{
    if (_keyDirection == InstrID::KEYDOWN_0 || _keyDirection == InstrID::KEYDOWN_1
            || _keyDirection == InstrID::KEYDOWN_2 || _keyDirection == InstrID::KEYDOWN_3
            || _keyDirection == InstrID::KEYDOWN_4)
    {
        uint32_t v;

        if (vNumAssigned < NUM_VOICES)
        {
            if (vNumAssigned == 0)
            {
                vOldestAssigned = vOldestReleased;
                vYoungestAssigned = vOldestReleased;
            }

            v = vOldestReleased;
            vOldestReleased = vNextReleased[v];

            vNumAssigned++;
        }
        else
        {
            v = vOldestAssigned;
            vOldestAssigned = vNextAssigned[v];
        }

        vPreviousAssigned[v] = vYoungestAssigned;
        vNextAssigned[vYoungestAssigned] = v;

        vYoungestAssigned = v;

        vVoiceState[v] = _pitch;

        ////////// Passing the Pitch and Velocity values to all Modules for calculation //////

#ifdef MANYGENS
        mSoundGenerator[v].setPitch(_pitch);
        mSoundGenerator[v].resetPhase();

        mCombFilter[v].setMainFreq(_pitch);
#endif
#ifdef ONEGEN
        mSoundGenerator.setPitch(_pitch, v);
        mSoundGenerator.resetPhase(v);
#endif
        mOutputMixer.setKeyPitch(v, _pitch);
        mOutputMixer.setVelocity(v, _velocity/127.f);

        ///////////////////////////////////////////////////////////////////////////////////////
    }

    else if (_keyDirection == InstrID::KEYUP_0 || _keyDirection == InstrID::KEYUP_1         // key up
             || _keyDirection == InstrID::KEYUP_2 || _keyDirection == InstrID::KEYUP_3
             || _keyDirection == InstrID::KEYUP_4)
    {
        uint32_t v;

        for (v = 0; v < NUM_VOICES; v++)
        {
            if (vVoiceState[v] == _pitch)
            {
                vNextReleased[vYoungestReleased] = v;
                vYoungestReleased = v;

                if (vNumAssigned == NUM_VOICES)
                {
                    vOldestReleased = v;
                }

                vNumAssigned--;

                if (vOldestAssigned == v)
                {
                    vOldestAssigned = vNextAssigned[v];

                }
                else if (vYoungestAssigned == v)
                {
                    vYoungestAssigned = vPreviousAssigned[v];
                }
                else
                {
                    vNextAssigned[vPreviousAssigned[v]] = vNextAssigned[v];
                    vPreviousAssigned[vNextAssigned[v]] = vPreviousAssigned[v];
                }

                vVoiceState[v] = -1;

                ///////////////// Reseting Pitch and Velocity Values in all modules ///////////////////

                mOutputMixer.setVelocity(v, 0.f);

                ///////////////////////////////////////////////////////////////////////////////////////
                break;
            }
        }
    }
}
#endif
