#ifndef SHAREDOUTPUTS_H
#define SHAREDOUTPUTS_H

#define NUM_VOICES 12
#define NUM_CHANNELS 2

extern float gSoundGenOut_A[NUM_VOICES];
extern float gSoundGenOut_B[NUM_VOICES];

extern float gKeyPitch[NUM_VOICES];

#if 0
struct SharedModuleOutputs
{
    float mSoundGenOut_A[NUM_VOICES];
    float mSoundGenOut_B[NUM_VOICES];
//        float mEnvOut_A;
//        float mEnvOut_B;
//        float mEnvOut_C;
//        float mGateOut;
//        float mCombFilterOut;
//        float mVRFilterOut;
//        float mFeedbackMixerOut;

};
#endif

#endif // SHAREDOUTPUTS_H
