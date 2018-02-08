/******************************************************************************/
/** @file		envelopes.cpp
    @date		2017-07-11
    @version	1.0
    @author		Anton Schmied[2017-07-11]
    @brief		Envelope Class member and method definitions
*******************************************************************************/

#include "envelopes.h"

/******************************************************************************/
/** Soundgenerator Default Constructor
 * @brief    initialization of the modules local variabels with default values
*******************************************************************************/

Envelopes::Envelopes()
{
    mEnvRamp_A = 0.f;
    mEnvRamp_B = 0.f;
    mEnvRamp_C = 0.f;
    mGateRamp = 0.f;

    mEnvState_A = env_off;
    mEnvState_B = env_off;
    mEnvState_C = env_off;
    mGateState = gate_closed;

    mInternalRamp_A = 0.f;
    mInternalRamp_B = 0.f;
    mInternalRamp_C = 0.f;

    mDecayDx_A = 1.f - (1.f / ((800.f/ 1000.f) * SAMPLERATE + 1.f));
    mDecayDx_B = 1.f - (1.f / ((800.f/ 1000.f) * SAMPLERATE + 1.f));
    mDecayDx_C = 1.f - (1.f / ((800.f/ 1000.f) * SAMPLERATE + 1.f));
    mReleaseDx = 1.f - (1.f / ((10.f/ 1000.f) * SAMPLERATE + 1.f));
}



/*****************************************************************************/
/** @brief    interface method which converts and scales the incoming midi
 *            values and passes these to the respective methods
 *  @param    midi control value [0 ... 127]
 *  @param    midi control ID -> enum CtrlID (envelope.h)
******************************************************************************/

void Envelopes::setEnvelopePramas(unsigned char _ctrlID, float _ctrlVal)
{
    switch (_ctrlID)
    {
        case CtrlID::DECAYAMOUNT_A:
            _ctrlVal = (_ctrlVal / 127.f) * 1000.f;
            _ctrlVal = (_ctrlVal * 0.208165f * 0.5f) - 20.f;
            _ctrlVal = NlToolbox::Conversion::db2af(_ctrlVal);

            printf("Envelope - A Decay: %f\n", _ctrlVal);

            _ctrlVal = 1.f / ((_ctrlVal/ 1000.f) * SAMPLERATE + 1.f);
            mDecayDx_A = 1.f - _ctrlVal;
            break;

        case CtrlID::DECAYAMOUNT_B:
            _ctrlVal = (_ctrlVal / 127.f) * 1000.f;
            _ctrlVal = (_ctrlVal * 0.208165f * 0.5f) - 20.f;
            _ctrlVal = NlToolbox::Conversion::db2af(_ctrlVal);

            printf("Envelope - B Decay: %f\n", _ctrlVal);

            _ctrlVal = 1.f / ((_ctrlVal/ 1000.f) * SAMPLERATE + 1.f);
            mDecayDx_B = 1.f - _ctrlVal;
            break;

        case CtrlID::DECAYAMOUNT_C:
            _ctrlVal = (_ctrlVal / 127.f) * 1000.f;
            _ctrlVal = (_ctrlVal * 0.208165f * 0.5f) - 20.f;
            _ctrlVal = NlToolbox::Conversion::db2af(_ctrlVal);

            printf("Envelope - C Decay: %f\n", _ctrlVal);

            _ctrlVal = 1.f / ((_ctrlVal/ 1000.f) * SAMPLERATE + 1.f);
            mDecayDx_C = 1.f - _ctrlVal;
            break;
    }

}

/******************************************************************************/
/** @brief    main function which calculates the vlaues of all envelopes
 *            if the ramp has not yet reached 0.0 and the note is still
 *            active by a Note-On Event
*******************************************************************************/

void Envelopes::applyEnvelope()
{
    //**************************** Envelope A ********************************//

    if (mEnvState_A == env_decay)
    {
        mEnvRamp_A = mVelocity + mVelocity_diff * (1.f - mInternalRamp_A);
        mInternalRamp_A = mInternalRamp_A * mDecayDx_A;

        if (mInternalRamp_A < 1.e-9f)
        {
            mEnvRamp_A = 0.f;
            mEnvState_A = env_off;
        }
    }
    else if (mEnvState_A == env_release)
    {
        mEnvRamp_A = mVelocity + mVelocity_diff * (1.f - mInternalRamp_A);
        mInternalRamp_A = mInternalRamp_A * mReleaseDx;

        if (mInternalRamp_A < 1.e-9f)
        {
            mEnvRamp_A = 0.f;
            mEnvState_A = env_off;
        }
    }


    //**************************** Envelope B ********************************//

    if (mEnvState_B == env_decay)
    {
        mEnvRamp_B = mVelocity + mVelocity_diff * (1.f - mInternalRamp_B);
        mInternalRamp_B = mInternalRamp_B * mDecayDx_B;

        if (mInternalRamp_B < 1.e-9f)
        {
            mEnvRamp_B = 0.f;
            mEnvState_B = env_off;
        }
    }
    else if (mEnvState_B == env_release)
    {
        mEnvRamp_B = mVelocity + mVelocity_diff * (1.f - mInternalRamp_B);
        mInternalRamp_B = mInternalRamp_B * mReleaseDx;

        if (mInternalRamp_B < 1.e-9f)
        {
            mEnvRamp_B = 0.f;
            mEnvState_B = env_off;
        }
    }


    //**************************** Envelope C ********************************//

    if (mEnvState_C == env_decay)
    {
        mEnvRamp_C = mVelocity + mVelocity_diff * (1.f - mInternalRamp_C);
        mInternalRamp_C = mInternalRamp_C * mDecayDx_C;

        if (mInternalRamp_C < 1.e-9f)
        {
            mEnvRamp_C = 0.f;
            mEnvState_C = env_off;
        }
    }
    else if (mEnvState_C == env_release)
    {
        mEnvRamp_C = mVelocity + mVelocity_diff * (1.f - mInternalRamp_C);
        mInternalRamp_C = mInternalRamp_C * mReleaseDx;

        if (mInternalRamp_C < 1.e-9f)
        {
            mEnvRamp_C = 0.f;
            mEnvState_C = env_off;
        }
    }


    //******************************* Gate ***********************************//

    if (mGateState == gate_open)
    {
        mGateRamp = 1.f;
    }
    else if (mGateState == gate_release)
    {
        mGateRamp = mInternalRamp_Gate;
        mInternalRamp_Gate = mInternalRamp_Gate * mReleaseDx;

        if (mInternalRamp_Gate < 1.e-9f)
        {
            mGateRamp = 0.f;
            mGateState = gate_closed;
        }
    }
    else if (mGateState == gate_closed)
    {
        mGateRamp = 0.f;
    }
}


/******************************************************************************/
/** @brief    main function which calculates the vlaues of all envelopes
 *            if the ramp has not yet reached 0.0 and the note is still
 *            active by a Note-On Event -> writes the values to a global array
*******************************************************************************/

void Envelopes::applyEnvelope(float *polyPtr)
{
    //**************************** Envelope A ********************************//

    if (mEnvState_A == env_decay)
    {
        polyPtr[0] = mVelocity + mVelocity_diff * (1.f - mInternalRamp_A);
        mInternalRamp_A = mInternalRamp_A * mDecayDx_A;

        if (mInternalRamp_A < 1.e-9f)
        {
            polyPtr[0] = 0.f;
            mEnvState_A = env_off;
        }
    }
    else if (mEnvState_A == env_release)
    {
        polyPtr[0] = mVelocity + mVelocity_diff * (1.f - mInternalRamp_A);
        mInternalRamp_A = mInternalRamp_A * mReleaseDx;

        if (mInternalRamp_A < 1.e-9f)
        {
            polyPtr[0] = 0.f;
            mEnvState_A = env_off;
        }
    }

    //**************************** Envelope B ********************************//

    if (mEnvState_B == env_decay)
    {
        polyPtr[2] = mVelocity + mVelocity_diff * (1.f - mInternalRamp_B);
        mInternalRamp_B = mInternalRamp_B * mDecayDx_B;

        if (mInternalRamp_B < 1.e-9f)
        {
            polyPtr[2] = 0.f;
            mEnvState_B = env_off;
        }
    }
    else if (mEnvState_B == env_release)
    {
        polyPtr[2] = mVelocity + mVelocity_diff * (1.f - mInternalRamp_B);
        mInternalRamp_B = mInternalRamp_B * mReleaseDx;

        if (mInternalRamp_B < 1.e-9f)
        {
            polyPtr[2] = 0.f;
            mEnvState_B = env_off;
        }
    }

    //**************************** Envelope C ********************************//

    if (mEnvState_C == env_decay)
    {
        polyPtr[4] = mVelocity + mVelocity_diff * (1.f - mInternalRamp_C);
        mInternalRamp_C = mInternalRamp_C * mDecayDx_C;

        if (mInternalRamp_C < 1.e-9f)
        {
            polyPtr[4] = 0.f;
            mEnvState_C = env_off;
        }
    }
    else if (mEnvState_C == env_release)
    {
        polyPtr[4] = mVelocity + mVelocity_diff * (1.f - mInternalRamp_C);
        mInternalRamp_C = mInternalRamp_C * mReleaseDx;

        if (mInternalRamp_C < 1.e-9f)
        {
            polyPtr[4] = 0.f;
            mEnvState_C = env_off;
        }
    }

    //******************************* Gate ***********************************//

    if (mGateState == gate_open)
    {
        polyPtr[5] = 1.f;
    }
    else if (mGateState == gate_release)
    {
        polyPtr[5] = mInternalRamp_Gate;
        mInternalRamp_Gate = mInternalRamp_Gate * mReleaseDx;

        if (mInternalRamp_Gate < 1.e-9f)
        {
            polyPtr[5] = 0.f;
            mGateState = gate_closed;
        }
    }
    else if (mGateState == gate_closed)
    {
        polyPtr[5] = 0.f;
    }

}


/******************************************************************************/
/** @brief    main function which calculates the vlaues of all envelopes
 *            if the ramp has not yet reached 0.0 and the note is still
 *            active by a Note-On Event -> writes the values to a global array
*******************************************************************************/

void Envelopes::applyEnvelope(uint32_t _vn)
{
    //**************************** Envelope A ********************************//

    if (mEnvState_A == env_decay)
    {
        ParamSignalData::instance().setSignalValue(_vn, ENV_A, mVelocity + mVelocity_diff * (1.f - mInternalRamp_A));
        mInternalRamp_A = mInternalRamp_A * mDecayDx_A;

        if (mInternalRamp_A < 1.e-9f)
        {
            ParamSignalData::instance().setSignalValue(_vn, ENV_A, 0.f);
            mEnvState_A = env_off;
        }
    }
    else if (mEnvState_A == env_release)
    {
        ParamSignalData::instance().setSignalValue(_vn, ENV_A, mVelocity + mVelocity_diff * (1.f - mInternalRamp_A));
        mInternalRamp_A = mInternalRamp_A * mReleaseDx;

        if (mInternalRamp_A < 1.e-9f)
        {
            ParamSignalData::instance().setSignalValue(_vn, ENV_A, 0.f);
            mEnvState_A = env_off;
        }
    }

    //**************************** Envelope B ********************************//

    if (mEnvState_B == env_decay)
    {
        ParamSignalData::instance().setSignalValue(_vn, ENV_B, mVelocity + mVelocity_diff * (1.f - mInternalRamp_B));
        mInternalRamp_B = mInternalRamp_B * mDecayDx_B;

        if (mInternalRamp_B < 1.e-9f)
        {
            ParamSignalData::instance().setSignalValue(_vn, ENV_B, 0.f);
            mEnvState_B = env_off;
        }
    }
    else if (mEnvState_B == env_release)
    {
        ParamSignalData::instance().setSignalValue(_vn, ENV_B, mVelocity + mVelocity_diff * (1.f - mInternalRamp_B));
        mInternalRamp_B = mInternalRamp_B * mReleaseDx;

        if (mInternalRamp_B < 1.e-9f)
        {
            ParamSignalData::instance().setSignalValue(_vn, ENV_B, 0.f);
            mEnvState_B = env_off;
        }
    }

    //**************************** Envelope C ********************************//

    if (mEnvState_C == env_decay)
    {
        ParamSignalData::instance().setSignalValue(_vn, ENV_C, mVelocity + mVelocity_diff * (1.f - mInternalRamp_C));
        mInternalRamp_C = mInternalRamp_C * mDecayDx_C;

        if (mInternalRamp_C < 1.e-9f)
        {
            ParamSignalData::instance().setSignalValue(_vn, ENV_C, 0.f);
            mEnvState_C = env_off;
        }
    }
    else if (mEnvState_C == env_release)
    {
        ParamSignalData::instance().setSignalValue(_vn, ENV_C, mVelocity + mVelocity_diff * (1.f - mInternalRamp_C));
        mInternalRamp_C = mInternalRamp_C * mReleaseDx;

        if (mInternalRamp_C < 1.e-9f)
        {
            ParamSignalData::instance().setSignalValue(_vn, ENV_C, 0.f);
            mEnvState_C = env_off;
        }
    }

    //******************************* Gate ***********************************//

    if (mGateState == gate_open)
    {
        ParamSignalData::instance().setSignalValue(_vn, ENV_GATE, 1.f);
    }
    else if (mGateState == gate_release)
    {
        ParamSignalData::instance().setSignalValue(_vn, ENV_GATE, mInternalRamp_Gate);
        mInternalRamp_Gate = mInternalRamp_Gate * mReleaseDx;

        if (mInternalRamp_Gate < 1.e-9f)
        {
            ParamSignalData::instance().setSignalValue(_vn, ENV_GATE, 0.f);
            mGateState = gate_closed;
        }
    }
    else if (mGateState == gate_closed)
    {
        ParamSignalData::instance().setSignalValue(_vn, ENV_GATE, 0.f);
    }
}



/******************************************************************************/
/** @brief    sets the envelope ramps to the value of the incoming velocity
*******************************************************************************/

void Envelopes::setEnvelope(float _velocity)
{
    mVelocity = _velocity;
    mVelocity_diff = 0.f - _velocity;

    mEnvState_A = env_decay;
    mEnvState_B = env_decay;
    mEnvState_C = env_decay;
    mGateState = gate_open;

    mInternalRamp_A = 1.f;
    mInternalRamp_B = 1.f;
    mInternalRamp_C = 1.f;
}



/******************************************************************************/
/** @brief    sets the envelope ramps to 0.f -> instant off
*******************************************************************************/

void Envelopes::killEnvelope()
{
    mEnvState_A = env_release;
    mEnvState_B = env_release;
    mEnvState_C = env_release;

    mGateState = gate_release;

    mInternalRamp_Gate = 1.f;
}


