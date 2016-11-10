/******************************************************************************/
/** @file		voicemanager.cpp
    @date		2016-07-26
    @version	0.1
    @author		Anton Schmied[2016-07-26]
    @brief		..
*******************************************************************************/

#include "voicemanager.h"

float gSoundGenOut_A[NUM_VOICES] = {};
float gSoundGenOut_B[NUM_VOICES] = {};

float gKeyPitch[NUM_VOICES] = {};
float gVoiceVelocity[NUM_VOICES] = {};

/******************************************************************************/
/** Voice Manager Default Constructor
 * @brief    initialization of the modules local variabels with default values
*******************************************************************************/

VoiceManager::VoiceManager()
{
    mSoundGenerator[NUM_VOICES] = Soundgenerator();          // Soundgenerator for each Voice
    mOutputMixer = Outputmixer();                            // Outputmixer

    monoModule_L.mCabinet = Cabinet();                       // Cabinet Effect
    monoModule_L.mEcho = Echo();                             // Echo Effect

    monoModule_R = monoModule_L;                             // Module Copy

    for (unsigned int i = 0; i < NUM_VOICES; i++)
    {
        mSoundGenerator[i].setVoiceNumber(i);
    }

    vallocInit();

    mainOut_L = 0.f;
    mainOut_R = 0.f;
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

            for(unsigned int i = 0; i < NUM_VOICES; i++)
            {
                // Soundgenerator Parameters
                mSoundGenerator[i].setGenParams(_instrID, _ctrlID, _ctrlVal);
            }

            break;

        case InstrID::CABINET_PARAM:

            // Cabinet Parameters
            monoModule_L.mCabinet.setCabinetParams(_ctrlVal, _ctrlID);
            monoModule_R.mCabinet.setCabinetParams(_ctrlVal, _ctrlID);

            break;

        case InstrID::OUTPUTMIXER_PARAM:

            mOutputMixer.setOutputmixerParams(_ctrlID, _ctrlVal);
            break;

        case InstrID::KEYUP_0:
        case InstrID::KEYUP_1:
        case InstrID::KEYUP_2:
        case InstrID::KEYUP_3:

            vallocProcess(_instrID, _ctrlID, _ctrlVal);

            break;

        case InstrID::KEYDOWN_0:
        case InstrID::KEYDOWN_1:
        case InstrID::KEYDOWN_2:
        case InstrID::KEYDOWN_3:

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
    for(unsigned int i = 0; i < NUM_VOICES; i++)
    {
        mSoundGenerator[i].generateSound();

        gSoundGenOut_A[i] = mSoundGenerator[i].mSampleA;
        gSoundGenOut_B[i] = mSoundGenerator[i].mSampleB;
    }

    mOutputMixer.applyOutputmixer();

    mainOut_R = mOutputMixer.mSample_R;
    mainOut_L = mOutputMixer.mSample_L;
}


#if 1
/******************************************************************************/
/** @brief    voice allocation initialization
 *
*******************************************************************************/

void VoiceManager::vallocInit()
{
    for (int i = 0; i < NUM_VOICES; i++)
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

    for (int i = 0; i < NUM_VOICES - 1; i++)
    {
        vNextReleased[i] = i + 1;
    }
}

/******************************************************************************/
/** @brief    voice allocation process
 *  @param    ID for keyUp or keyDown
*******************************************************************************/

void VoiceManager::vallocProcess(unsigned char _keyDirection, float _pitch, float _velocity)
{
    if (_keyDirection == InstrID::KEYDOWN_0 || _keyDirection == InstrID::KEYDOWN_1
            || _keyDirection == InstrID::KEYDOWN_2 || _keyDirection == InstrID::KEYDOWN_3)  // key down
    {
        int v;

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

        mSoundGenerator[v].setPitch(_pitch);
        mSoundGenerator[v].resetPhase();

        gKeyPitch[v] = _pitch;
        gVoiceVelocity[v] = _velocity/127.f;
    }

    else if (_keyDirection == InstrID::KEYUP_0 || _keyDirection == InstrID::KEYUP_1
             || _keyDirection == InstrID::KEYUP_2 || _keyDirection == InstrID::KEYUP_3)  // key up
    {
        int v;

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
                gKeyPitch[v] = -1;
                gVoiceVelocity[v] = 0.f;

                break;
            }
        }
    }
}
#endif
