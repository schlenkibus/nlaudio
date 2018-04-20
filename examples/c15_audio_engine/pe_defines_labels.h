/******************************************************************************/
/** @file       pe_defines_labels.h
    @date       2018-04-20
    @version    1.0
    @author     Matthias Seeber
    @brief      c15 parameter labels (for better code readability)
                NOTE:
                The provided IDs represent the rendering items of the parameter
                engine, NOT the elements of the shared m_paramsignaldata[][] array!
                ..
    @todo
*******************************************************************************/

#pragma once

#include "dsp_defines_session.h"

/* Envelope Parameter Labels - access parameter with (envIndex + ID) */

#define E_ATT       0
#define E_DEC1      1
#define E_BP        2
#define E_DEC2      3
#define E_SUS       4
#define E_REL       5
#define E_LV        7
#define E_AV        8
#define E_RV        9
#define E_LKT       10
#define E_TKT       11
#define E_AC        12

/* 'normal' Parameter Labels - access parameter with m_head[ID] - maybe, the rendering index would be much more efficient? */

#if DSP_TEST_MODE==1

#define P_EA_GAIN   6

#define P_OA_P      13
#define P_OA_PKT    14
#define P_OA_PMS    16
#define P_OA_PMSEA  17
#define P_OA_CHI    18

#define P_MA_T      20

#define P_KEY_VS    21
#define P_KEY_NP    22
#define P_KEY_PA    24

#elif DSP_TEST_MODE==2

#define P_EA_GAIN   6
#define P_EB_GAIN   19

#define P_OA_P      39
#define P_OA_PKT    40
#define P_OA_PEC    41
#define P_OA_F      42
#define P_OA_FEC    43
#define P_OA_PMS    44
#define P_OA_PMSEA  45
#define P_OA_PMSSH  46
#define P_OA_PMB    47
#define P_OA_PMBEB  48
#define P_OA_PMBSH  49
#define P_OA_PMF    50
#define P_OA_PMFEC  51
#define P_OA_PHS    52
#define P_OA_CHI    53

#define P_SA_D      54
#define P_SA_DEA    55
#define P_SA_F      56
#define P_SA_A      57
#define P_SA_M      58
#define P_SA_FBM    59
#define P_SA_FBEC   60
#define P_SA_RM     61

#define P_OB_P      62
#define P_OB_PKT    63
#define P_OB_PEC    64
#define P_OB_F      65
#define P_OB_FEC    66
#define P_OB_PMS    67
#define P_OB_PMSEA  68
#define P_OB_PMSSH  69
#define P_OB_PMB    70
#define P_OB_PMBEB  71
#define P_OB_PMBSH  72
#define P_OB_PMF    73
#define P_OB_PMFEC  74
#define P_OB_PHS    75
#define P_OB_CHI    76

#define P_SB_D      77
#define P_SB_DEA    78
#define P_SB_F      79
#define P_SB_A      80
#define P_SB_M      81
#define P_SB_FBM    82
#define P_SB_FBEC   83
#define P_SB_RM     84

#define P_OM_AL     85
#define P_OM_AP     86
#define P_OM_BL     87
#define P_OM_BP     88

#define P_MA_V      89
#define P_MA_T      90

#define P_KEY_PA    91
#define P_KEY_PB    92
#define P_KEY_NP    93
#define P_KEY_VP    94
#define P_KEY_EC    95
#define P_KEY_VS    96

#endif
