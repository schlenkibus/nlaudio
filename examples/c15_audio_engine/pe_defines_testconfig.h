/* configuration of the test project */

#pragma once

#include <stdint.h>

/* needed are control id mappings (maybe for two different devices) and (two ore more) predefined presets */

#define testMidiDevice 0
#define testRecallSequenceLength 21
/* valid triggers */
#define test_P0 1
#define test_P1 2
#define test_P2 3
#define test_P3 4
#define test_P4 5
#define test_P5 6
#define test_P6 7
#define test_P7 8
#define test_P8 9
#define test_P9 10
#define test_P10 11
#define test_P11 12
#define test_P12 13
#define test_P13 14
#define test_P14 15
#define test_P15 16
#define test_P16 17
#define test_P17 18
#define test_P18 19
#define test_P19 20
#define test_P20 21
#define test_TIME 22
#define test_R0 23
#define test_R1 24
#define test_R2 25
#define test_FL 26
#define test_PH 27
#define test_PB 28
#define test_PS 29
#define test_RF 30

const uint32_t testMidiMapping[2][128] = {
    /* device 1 (axiom at home) */
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, test_PS, test_R1, 0, 0,
        test_PH, test_FL, test_PB, test_R0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, test_TIME, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    /* device 2 (remote at work) */
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }
};

const float testPresetData[3][testRecallSequenceLength] = {
    /* preset 0 - null */
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    /* preset 1 - default */
    {
        0, 0.59, 0.5, 0.79, 0, 0.53, 0, 0.5, 0, 0, 0, 0.05, 0, (80.f / 150.f), (100.f / 105.f), 0, 0, 0, (4.f / 6.f), 0.5, 0
    },
    /* preset 2 - null */
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
};
