/******************************************************************************/
/** @file		envelopes.h
    @date		2017-07-11
    @version	1.0
    @author		Anton Schmied[2017-07-11]
    @brief		An implementation of a simple Envelope which
                decays from the velocity level to 0

    @todo
*******************************************************************************/

#pragma once

#include "nlglobaldefines.h"
#include "nltoolbox.h"

class Envelopes
{

public:
    Envelopes();     // default Constructor

//    Envelope();     // parametrized Constructor

    ~Envelopes(){}

    float mEnvRamp_A, mEnvRamp_B, mEnvRamp_C;
    float mGateRamp;

    void setEnvelopePramas(unsigned char _ctrlID, float _ctrlVal);
    void applyEnvelope();
    void setEnvelope(float _velocity);
    void killEnvelope();

private:

    float mVelocity;
    float mVelocity_diff;

    float mInternalRamp_A;
    float mInternalRamp_B;
    float mInternalRamp_C;
    float mInternalRamp_Gate;

    float mDecayDx_A, mDecayDx_B, mDecayDx_C;
    float mReleaseDx;

    //**************************** Enevelope State ****************************//

    enum EnvState
    {
        env_off, env_decay, env_release,
        gate_on, gate_off
    };

    EnvState mEnvState_A;
    EnvState mEnvState_B;
    EnvState mEnvState_C;
    EnvState mGateState;

    //****************************** Controls IDs *****************************//

    enum CtrlID: unsigned char
    {
#ifdef REMOTE61                     // novation ReMOTE61
        DECAYAMOUNT_A   = 0x15,
        DECAYAMOUNT_B   = 0x16,
        DECAYAMOUNT_C   = 0x17,
#else
        DECAYRATE,
        DECAYTARGETRATIO,
#endif
    };

};
