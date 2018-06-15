/******************************************************************************/
/** @file       dsp_defines_signallabels.h
    @date       2018-03-22
    @version    1.0
    @author     Matthias Seeber & Anton Schmied
    @brief      Defines for the individual Signal Names in the great
                m_paramsignaldata[][] array
    @todo       add all the names to the signal names
*******************************************************************************/

#pragma once

#include "dsp_defines_session.h"

#define ENV_A_SIG       0
#define ENV_B_SIG       1
#define ENV_C_SIG       2
#define ENV_G_SIG       3

#if DSP_TEST_MODE==1

#define OSC_A_FRQ       4
#define OSC_A_FLUEC     5
#define OSC_A_PMSEA     6
#define OSC_A_PHS       7
#define OSC_A_CHI       8
#define MST_VOL         9

#elif DSP_TEST_MODE==2

#define OSC_A_FRQ       4
#define OSC_A_FLUEC     5
#define OSC_A_PMSEA     6
#define OSC_A_PMSSH     7
#define OSC_A_PMBEB     8
#define OSC_A_PMBSH     9
#define OSC_A_PMFEC     10
#define OSC_A_CHI       11

#define SHP_A_DRVEA     12
#define SHP_A_FLD       13
#define SHP_A_ASM       14
#define SHP_A_MIX       15
#define SHP_A_FBM       16
#define SHP_A_FBEC      17
#define SHP_A_RM        18

#define OSC_B_FRQ       19
#define OSC_B_FLUEC     20
#define OSC_B_PMSEB     21
#define OSC_B_PMSSH     22
#define OSC_B_PMAEA     23
#define OSC_B_PMASH     24
#define OSC_B_PMFEC     25
#define OSC_B_CHI       26

#define SHP_B_DRVEB     27
#define SHP_B_FLD       28
#define SHP_B_ASM       29
#define SHP_B_MIX       30
#define SHP_B_FBM       31
#define SHP_B_FBEC      32
#define SHP_B_RM        33

#define OUT_A_L         34
#define OUT_A_R         35
#define OUT_B_L         36
#define OUT_B_R         37

#define MST_VOL         38

#define CMB_FRQ         1000
#define CMB_LPF         1001
#define CMB_APF         1002
#define CMB_APR         1003
#define CMB_DEC         1004
#define CMB_BYP         1005

#elif DSP_TEST_MODE==3

#define OSC_A_FRQ       4
#define OSC_A_FLUEC     5
#define OSC_A_PMSEA     6
#define OSC_A_PMSSH     7
#define OSC_A_PMBEB     8
#define OSC_A_PMBSH     9
#define OSC_A_PMFEC     10
#define OSC_A_CHI       11

#define SHP_A_DRVEA     12
#define SHP_A_FLD       13
#define SHP_A_ASM       14
#define SHP_A_MIX       15
#define SHP_A_FBM       16
#define SHP_A_FBEC      17
#define SHP_A_RM        18

#define OSC_B_FRQ       19
#define OSC_B_FLUEC     20
#define OSC_B_PMSEB     21
#define OSC_B_PMSSH     22
#define OSC_B_PMAEA     23
#define OSC_B_PMASH     24
#define OSC_B_PMFEC     25
#define OSC_B_CHI       26

#define SHP_B_DRVEB     27
#define SHP_B_FLD       28
#define SHP_B_ASM       29
#define SHP_B_MIX       30
#define SHP_B_FBM       31
#define SHP_B_FBEC      32
#define SHP_B_RM        33

#define CMB_AB          34
#define CMB_FRQ         35
#define CMB_BYP         36
#define CMB_FEC         37
#define CMB_DEC         38
#define CMB_APF         39
#define CMB_APR         40
#define CMB_LPF         41
#define CMB_PM          42
#define CMB_PMAB        43

#define OUT_A_L         44
#define OUT_A_R         45
#define OUT_B_L         46
#define OUT_B_R         47
#define OUT_CMB_L       48
#define OUT_CMB_R       49

#define MST_VOL         50

/*
#define CMB_F           1000    // rename (FRQ)!
#define CMB_LPF         1001
#define CMB_APF         1002
#define CMB_APRES       1003    // rename (APR)!
#define CMB_DT          1004    // rename (DEC)!
#define CMB_BYP         1005
*/

#endif
