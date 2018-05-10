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
#define P_OA_CHI    52

#define P_SA_D      53
#define P_SA_DEA    54
#define P_SA_F      55
#define P_SA_A      56
#define P_SA_M      57
#define P_SA_FBM    58
#define P_SA_FBEC   59
#define P_SA_RM     60

#define P_OB_P      61
#define P_OB_PKT    62
#define P_OB_PEC    63
#define P_OB_F      64
#define P_OB_FEC    65
#define P_OB_PMS    66
#define P_OB_PMSEB  67
#define P_OB_PMSSH  68
#define P_OB_PMA    69
#define P_OB_PMAEA  70
#define P_OB_PMASH  71
#define P_OB_PMF    72
#define P_OB_PMFEC  73
#define P_OB_CHI    74

#define P_SB_D      75
#define P_SB_DEB    76
#define P_SB_F      77
#define P_SB_A      78
#define P_SB_M      79
#define P_SB_FBM    80
#define P_SB_FBEC   81
#define P_SB_RM     82

#define P_OM_AL     83
#define P_OM_AP     84
#define P_OM_BL     85
#define P_OM_BP     86

#define P_MA_V      87
#define P_MA_T      88

#define P_KEY_PA    89
#define P_KEY_PB    90
#define P_KEY_NP    91
#define P_KEY_VP    92
#define P_KEY_EC    93
#define P_KEY_VS    94

#endif
